#include "gltf_parser.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <cstring>

// -----------------------------------------------------------------------
// アクセサからバイト列ポインタを取得するヘルパー
// -----------------------------------------------------------------------
static const uint8_t* accessorRawPtr(const tinygltf::Model& model, int accessorIdx) {
    const tinygltf::Accessor&   acc  = model.accessors[accessorIdx];
    const tinygltf::BufferView& view = model.bufferViews[acc.bufferView];
    const tinygltf::Buffer&     buf  = model.buffers[view.buffer];
    return buf.data.data() + view.byteOffset + acc.byteOffset;
}

static const float* accessorDataPtr(const tinygltf::Model& model, int accessorIdx) {
    return reinterpret_cast<const float*>(accessorRawPtr(model, accessorIdx));
}

// -----------------------------------------------------------------------
// 各属性の抽出
// -----------------------------------------------------------------------
static std::vector<glm::vec3> extractPositions(const tinygltf::Model& model,
                                               const tinygltf::Primitive& prim) {
    std::map<std::string, int>::const_iterator it = prim.attributes.find("POSITION");
    if (it == prim.attributes.end())
        throw std::runtime_error("GltfParser: missing POSITION attribute");

    const float* data  = accessorDataPtr(model, it->second);
    const size_t count = model.accessors[it->second].count;

    std::vector<glm::vec3> out(count);
    for (size_t i = 0; i < count; i++)
        out[i] = {data[i * 3], data[i * 3 + 1], data[i * 3 + 2]};
    return out;
}

static std::vector<glm::vec3> extractNormals(const tinygltf::Model& model,
                                             const tinygltf::Primitive& prim) {
    std::map<std::string, int>::const_iterator it = prim.attributes.find("NORMAL");
    if (it == prim.attributes.end())
        return {};  // 法線なし

    const float* data  = accessorDataPtr(model, it->second);
    const size_t count = model.accessors[it->second].count;

    std::vector<glm::vec3> out(count);
    for (size_t i = 0; i < count; i++)
        out[i] = {data[i * 3], data[i * 3 + 1], data[i * 3 + 2]};
    return out;
}

static std::vector<unsigned int> extractIndices(const tinygltf::Model& model,
                                                const tinygltf::Primitive& prim,
                                                size_t vertexCount) {
    if (prim.indices < 0) {
        // インデックスなし → 連番
        std::vector<unsigned int> out(vertexCount);
        for (size_t i = 0; i < vertexCount; i++)
            out[i] = static_cast<unsigned int>(i);
        return out;
    }

    const tinygltf::Accessor&   acc  = model.accessors[prim.indices];
    const tinygltf::BufferView& view = model.bufferViews[acc.bufferView];
    const tinygltf::Buffer&     buf  = model.buffers[view.buffer];
    const uint8_t*              raw  = buf.data.data() + view.byteOffset + acc.byteOffset;

    std::vector<unsigned int> out;
    out.reserve(acc.count);

    for (size_t i = 0; i < acc.count; i++) {
        switch (acc.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            out.push_back(raw[i]);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            out.push_back(reinterpret_cast<const uint16_t*>(raw)[i]);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            out.push_back(reinterpret_cast<const uint32_t*>(raw)[i]);
            break;
        default:
            throw std::runtime_error("GltfParser: unsupported index component type");
        }
    }
    return out;
}

// JOINTS_0: componentType が UNSIGNED_BYTE or UNSIGNED_SHORT
static std::vector<glm::uvec4> extractJoints(const tinygltf::Model& model,
                                              const tinygltf::Primitive& prim) {
    std::map<std::string, int>::const_iterator it = prim.attributes.find("JOINTS_0");
    if (it == prim.attributes.end())
        return {};

    const tinygltf::Accessor& acc   = model.accessors[it->second];
    const uint8_t*            raw   = accessorRawPtr(model, it->second);
    const size_t              count = acc.count;

    std::vector<glm::uvec4> out(count);
    for (size_t i = 0; i < count; i++) {
        if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
            out[i] = glm::uvec4(raw[i * 4],
                                raw[i * 4 + 1],
                                raw[i * 4 + 2],
                                raw[i * 4 + 3]);
        } else { // UNSIGNED_SHORT
            const uint16_t* s = reinterpret_cast<const uint16_t*>(raw);
            out[i] = glm::uvec4(s[i * 4],
                                s[i * 4 + 1],
                                s[i * 4 + 2],
                                s[i * 4 + 3]);
        }
    }
    return out;
}

// WEIGHTS_0: float VEC4
static std::vector<glm::vec4> extractWeights(const tinygltf::Model& model,
                                              const tinygltf::Primitive& prim) {
    std::map<std::string, int>::const_iterator it = prim.attributes.find("WEIGHTS_0");
    if (it == prim.attributes.end())
        return {};

    const float* data  = accessorDataPtr(model, it->second);
    const size_t count = model.accessors[it->second].count;

    std::vector<glm::vec4> out(count);
    for (size_t i = 0; i < count; i++)
        out[i] = {data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]};
    return out;
}

// 全ノードの parent/translation/rotation/scale を抽出
// 親は children リストから逆引き
static std::vector<NodeData> extractNodes(const tinygltf::Model& model) {
    const size_t nodeCount = model.nodes.size();
    std::vector<NodeData> out(nodeCount);

    // 初期 TRS を設定
    for (size_t i = 0; i < nodeCount; i++) {
        const tinygltf::Node& n = model.nodes[i];

        if (n.translation.size() == 3) {
            out[i].translation = glm::vec3(
                static_cast<float>(n.translation[0]),
                static_cast<float>(n.translation[1]),
                static_cast<float>(n.translation[2]));
        }
        if (n.rotation.size() == 4) {
            // glTF: [x, y, z, w]
            out[i].rotation = glm::quat(
                static_cast<float>(n.rotation[3]),  // w
                static_cast<float>(n.rotation[0]),  // x
                static_cast<float>(n.rotation[1]),  // y
                static_cast<float>(n.rotation[2])); // z
        }
        if (n.scale.size() == 3) {
            out[i].scale = glm::vec3(
                static_cast<float>(n.scale[0]),
                static_cast<float>(n.scale[1]),
                static_cast<float>(n.scale[2]));
        }
        // matrix が指定されている場合は無視（TRS 優先）
    }

    // 子→親の逆引き
    for (size_t i = 0; i < nodeCount; i++) {
        const tinygltf::Node& n = model.nodes[i];
        for (int childIdx : n.children) {
            out[static_cast<size_t>(childIdx)].parent = static_cast<int>(i);
        }
    }

    return out;
}

// skins[0] の joints リストと inverseBindMatrices を抽出
static void extractSkin(const tinygltf::Model& model,
                         std::vector<int>&      skinJoints,
                         std::vector<glm::mat4>& invBindMatrices) {
    if (model.skins.empty())
        return;

    const tinygltf::Skin& skin = model.skins[0];
    skinJoints.resize(skin.joints.size());
    for (size_t i = 0; i < skin.joints.size(); i++)
        skinJoints[i] = skin.joints[i];

    // inverseBindMatrices アクセサ
    if (skin.inverseBindMatrices >= 0) {
        const float* data  = accessorDataPtr(model, skin.inverseBindMatrices);
        const size_t count = model.accessors[skin.inverseBindMatrices].count;
        invBindMatrices.resize(count);
        for (size_t i = 0; i < count; i++) {
            glm::mat4 m;
            std::memcpy(glm::value_ptr(m), data + i * 16, sizeof(float) * 16);
            invBindMatrices[i] = m;
        }
    } else {
        // 省略されている場合は単位行列
        invBindMatrices.assign(skin.joints.size(), glm::mat4(1.f));
    }
}

// 全アニメーションを抽出
static std::vector<AnimData> extractAnimations(const tinygltf::Model& model) {
    std::vector<AnimData> out;
    out.reserve(model.animations.size());

    for (const tinygltf::Animation& anim : model.animations) {
        AnimData ad;
        ad.name = anim.name;

        // サンプラー
        ad.samplers.reserve(anim.samplers.size());
        for (const tinygltf::AnimationSampler& s : anim.samplers) {
            AnimSampler as;

            // input (times)
            {
                const float* data  = accessorDataPtr(model, s.input);
                const size_t count = model.accessors[s.input].count;
                as.times.resize(count);
                for (size_t i = 0; i < count; i++)
                    as.times[i] = data[i];
            }

            // output (values)
            {
                const tinygltf::Accessor& acc  = model.accessors[s.output];
                const float*              data = accessorDataPtr(model, s.output);
                const size_t              count = acc.count;
                as.values.resize(count);

                if (acc.type == TINYGLTF_TYPE_VEC3) {
                    for (size_t i = 0; i < count; i++)
                        as.values[i] = glm::vec4(data[i * 3], data[i * 3 + 1],
                                                  data[i * 3 + 2], 0.f);
                } else if (acc.type == TINYGLTF_TYPE_VEC4) {
                    for (size_t i = 0; i < count; i++)
                        as.values[i] = glm::vec4(data[i * 4], data[i * 4 + 1],
                                                  data[i * 4 + 2], data[i * 4 + 3]);
                }
            }

            ad.samplers.push_back(std::move(as));
        }

        // チャンネル
        ad.channels.reserve(anim.channels.size());
        for (const tinygltf::AnimationChannel& ch : anim.channels) {
            AnimChannel ac;
            ac.nodeIdx    = ch.target_node;
            ac.samplerIdx = ch.sampler;

            if (ch.target_path == "translation")
                ac.path = AnimPath::Translation;
            else if (ch.target_path == "rotation")
                ac.path = AnimPath::Rotation;
            else if (ch.target_path == "scale")
                ac.path = AnimPath::Scale;
            else
                continue; // weights など未対応パスはスキップ

            ad.channels.push_back(ac);
        }

        out.push_back(std::move(ad));
    }

    return out;
}

// -----------------------------------------------------------------------
// ファイルロード
// -----------------------------------------------------------------------
static tinygltf::Model loadModel(const std::string& path) {
    tinygltf::Model    model;
    tinygltf::TinyGLTF loader;
    std::string        err, warn;

    bool ok = (path.size() >= 4 && path.substr(path.size() - 4) == ".glb")
        ? loader.LoadBinaryFromFile(&model, &err, &warn, path)
        : loader.LoadASCIIFromFile(&model, &err, &warn, path);

    if (!ok)
        throw std::runtime_error("GltfParser: failed to load '" + path + "': " + err);
    if (model.meshes.empty())
        throw std::runtime_error("GltfParser: no meshes in '" + path + "'");

    return model;
}

// -----------------------------------------------------------------------
// 公開 API
// -----------------------------------------------------------------------
GltfData parseGltf(const std::string& path) {
    const tinygltf::Model      model = loadModel(path);
    const tinygltf::Primitive& prim  = model.meshes[0].primitives[0];

    GltfData data;
    data.positions = extractPositions(model, prim);
    data.normals   = extractNormals(model, prim);
    data.indices   = extractIndices(model, prim, data.positions.size());
    data.joints    = extractJoints(model, prim);
    data.weights   = extractWeights(model, prim);
    data.nodes     = extractNodes(model);
    extractSkin(model, data.skinJoints, data.inverseBindMatrices);
    data.animations = extractAnimations(model);
    return data;
}
