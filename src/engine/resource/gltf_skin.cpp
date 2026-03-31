#include "gltf_skin.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <unordered_map>

// -----------------------------------------------------------------------
// retargetAnim: ボーン名ベースでノードインデックスを再マッピング
// -----------------------------------------------------------------------
AnimData retargetAnim(const AnimData&              srcAnim,
                      const std::vector<NodeData>& srcNodes,
                      const std::vector<NodeData>& dstNodes)
{
    // dstNodes の名前 → インデックスのマップを構築
    std::unordered_map<std::string, int> dstNameToIdx;
    for (size_t i = 0; i < dstNodes.size(); i++)
        dstNameToIdx[dstNodes[i].name] = static_cast<int>(i);

    AnimData result;
    result.name     = srcAnim.name;
    result.samplers = srcAnim.samplers; // サンプラーデータはそのままコピー

    for (const AnimChannel& ch : srcAnim.channels) {
        if (ch.nodeIdx < 0 || static_cast<size_t>(ch.nodeIdx) >= srcNodes.size())
            continue;

        const std::string& boneName = srcNodes[static_cast<size_t>(ch.nodeIdx)].name;
        auto it = dstNameToIdx.find(boneName);
        if (it == dstNameToIdx.end())
            continue; // ターゲットに対応するボーンがなければスキップ

        AnimChannel newCh  = ch;
        newCh.nodeIdx      = it->second;
        result.channels.push_back(newCh);
    }

    return result;
}

// -----------------------------------------------------------------------
// コンストラクタ
// -----------------------------------------------------------------------
GltfSkin::GltfSkin(const GltfData& data)
    : data_(data)
{
}

// -----------------------------------------------------------------------
// duration: 指定アニメーションの最大時間を返す
// -----------------------------------------------------------------------
float GltfSkin::duration(const std::string& animName) const {
    for (const AnimData& anim : data_.animations) {
        if (anim.name == animName) {
            float maxTime = 0.f;
            for (const AnimSampler& s : anim.samplers) {
                if (!s.times.empty())
                    maxTime = std::max(maxTime, s.times.back());
            }
            return maxTime;
        }
    }
    return 0.f;
}

// -----------------------------------------------------------------------
// sampleVec3: 線形補間（LERP）
// -----------------------------------------------------------------------
glm::vec3 GltfSkin::sampleVec3(const AnimSampler& s, float t) const {
    if (s.times.empty())
        return glm::vec3(0.f);

    if (t <= s.times.front())
        return glm::vec3(s.values.front());
    if (t >= s.times.back())
        return glm::vec3(s.values.back());

    // 区間を二分探索
    size_t hi = 1;
    while (hi < s.times.size() && s.times[hi] < t)
        ++hi;
    const size_t lo = hi - 1;

    const float range = s.times[hi] - s.times[lo];
    const float alpha = (range > 1e-7f) ? (t - s.times[lo]) / range : 0.f;

    return glm::mix(glm::vec3(s.values[lo]), glm::vec3(s.values[hi]), alpha);
}

// -----------------------------------------------------------------------
// sampleQuat: 球面線形補間（SLERP）
// glTF 格納順: values[i] = (x, y, z, w)
// -----------------------------------------------------------------------
glm::quat GltfSkin::sampleQuat(const AnimSampler& s, float t) const {
    if (s.times.empty())
        return glm::quat(1.f, 0.f, 0.f, 0.f);

    // glTF の quat は vec4(x,y,z,w) → glm::quat(w,x,y,z)
    auto toQuat = [](const glm::vec4& v) -> glm::quat {
        return glm::quat(v.w, v.x, v.y, v.z);
    };

    if (t <= s.times.front())
        return toQuat(s.values.front());
    if (t >= s.times.back())
        return toQuat(s.values.back());

    size_t hi = 1;
    while (hi < s.times.size() && s.times[hi] < t)
        ++hi;
    const size_t lo = hi - 1;

    const float range = s.times[hi] - s.times[lo];
    const float alpha = (range > 1e-7f) ? (t - s.times[lo]) / range : 0.f;

    return glm::slerp(toQuat(s.values[lo]), toQuat(s.values[hi]), alpha);
}

// -----------------------------------------------------------------------
// globalMatrix: ノードのグローバル変換行列を再帰計算
// -----------------------------------------------------------------------
glm::mat4 GltfSkin::globalMatrix(int nodeIdx,
                                   const std::vector<glm::vec3>& translations,
                                   const std::vector<glm::quat>& rotations,
                                   const std::vector<glm::vec3>& scales) const {
    const glm::mat4 T = glm::translate(glm::mat4(1.f), translations[static_cast<size_t>(nodeIdx)]);
    const glm::mat4 R = glm::mat4_cast(rotations[static_cast<size_t>(nodeIdx)]);
    const glm::mat4 S = glm::scale(glm::mat4(1.f), scales[static_cast<size_t>(nodeIdx)]);
    const glm::mat4 local = T * R * S;

    const int parent = data_.nodes[static_cast<size_t>(nodeIdx)].parent;
    if (parent < 0)
        return local;

    return globalMatrix(parent, translations, rotations, scales) * local;
}

// -----------------------------------------------------------------------
// recomputeNormals: フラット法線を再計算
// -----------------------------------------------------------------------
void GltfSkin::recomputeNormals(std::vector<glm::vec3>& normals,
                                  const std::vector<glm::vec3>& positions) const {
    const size_t vertCount = positions.size();
    normals.assign(vertCount, glm::vec3(0.f));

    const std::vector<unsigned int>& idx = data_.indices;
    const size_t triCount = idx.size() / 3;

    for (size_t t = 0; t < triCount; t++) {
        const unsigned int i0 = idx[t * 3];
        const unsigned int i1 = idx[t * 3 + 1];
        const unsigned int i2 = idx[t * 3 + 2];

        const glm::vec3 edge1 = positions[i1] - positions[i0];
        const glm::vec3 edge2 = positions[i2] - positions[i0];
        const glm::vec3 faceN = glm::cross(edge1, edge2);

        normals[i0] += faceN;
        normals[i1] += faceN;
        normals[i2] += faceN;
    }

    for (size_t i = 0; i < vertCount; i++) {
        const float len = glm::length(normals[i]);
        if (len > 1e-7f)
            normals[i] /= len;
        else
            normals[i] = glm::vec3(0.f, 1.f, 0.f);
    }
}

// -----------------------------------------------------------------------
// getBoneWorldMatrix: update() 後に指定名のボーングローバル行列を返す
// -----------------------------------------------------------------------
glm::mat4 GltfSkin::getBoneWorldMatrix(const std::string& boneName) const {
    if (cachedTranslations_.empty())
        return glm::mat4(1.f);

    for (size_t i = 0; i < data_.nodes.size(); i++) {
        if (data_.nodes[i].name == boneName)
            return globalMatrix(static_cast<int>(i),
                                cachedTranslations_,
                                cachedRotations_,
                                cachedScales_);
    }
    return glm::mat4(1.f);
}

// -----------------------------------------------------------------------
// update: アニメーションを time でサンプルし CPU スキニングを実行
// 戻り値: フラット頂点配列 (x,y,z,nx,ny,nz) * vertexCount
// -----------------------------------------------------------------------
std::vector<float> GltfSkin::update(const std::string& animName, float time) {
    // 1. アニメーション検索
    const AnimData* animPtr = nullptr;
    for (const AnimData& a : data_.animations) {
        if (a.name == animName) {
            animPtr = &a;
            break;
        }
    }
    if (!animPtr)
        throw std::runtime_error("GltfSkin: animation not found: " + animName);

    const AnimData& anim = *animPtr;
    const size_t    nodeCount = data_.nodes.size();

    // 2. 全ノードの初期 TRS をコピー
    std::vector<glm::vec3> translations(nodeCount);
    std::vector<glm::quat> rotations(nodeCount);
    std::vector<glm::vec3> scaleVecs(nodeCount);

    for (size_t i = 0; i < nodeCount; i++) {
        translations[i] = data_.nodes[i].translation;
        rotations[i]    = data_.nodes[i].rotation;
        scaleVecs[i]    = data_.nodes[i].scale;
    }

    // 3. 各チャンネルをサンプルしてノード TRS を上書き
    for (const AnimChannel& ch : anim.channels) {
        if (ch.nodeIdx < 0 || static_cast<size_t>(ch.nodeIdx) >= nodeCount)
            continue;
        const AnimSampler& sampler = anim.samplers[static_cast<size_t>(ch.samplerIdx)];
        const size_t       ni      = static_cast<size_t>(ch.nodeIdx);

        switch (ch.path) {
        case AnimPath::Translation:
            translations[ni] = sampleVec3(sampler, time);
            break;
        case AnimPath::Rotation:
            rotations[ni] = sampleQuat(sampler, time);
            break;
        case AnimPath::Scale:
            scaleVecs[ni] = sampleVec3(sampler, time);
            break;
        }
    }

    // 3b. TRS をキャッシュ（getBoneWorldMatrix で参照）
    cachedTranslations_ = translations;
    cachedRotations_    = rotations;
    cachedScales_       = scaleVecs;

    // 4. スキン行列を計算: globalMatrix(joint) * inverseBindMatrix
    const size_t jointCount = data_.skinJoints.size();
    std::vector<glm::mat4> skinMatrices(jointCount);
    for (size_t j = 0; j < jointCount; j++) {
        const int nodeIdx = data_.skinJoints[j];
        skinMatrices[j] = globalMatrix(nodeIdx, translations, rotations, scaleVecs)
                          * data_.inverseBindMatrices[j];
    }

    // 5. 各頂点に 4 ボーンブレンド
    const size_t vertCount = data_.positions.size();
    std::vector<glm::vec3> skinnedPositions(vertCount);

    const bool hasSkinning = !data_.joints.empty() && !data_.weights.empty()
                             && jointCount > 0;

    for (size_t i = 0; i < vertCount; i++) {
        if (!hasSkinning) {
            skinnedPositions[i] = data_.positions[i];
            continue;
        }

        const glm::uvec4& j = data_.joints[i];
        const glm::vec4&  w = data_.weights[i];

        glm::mat4 blended = w.x * skinMatrices[j.x]
                          + w.y * skinMatrices[j.y]
                          + w.z * skinMatrices[j.z]
                          + w.w * skinMatrices[j.w];

        const glm::vec4 pos4 = blended * glm::vec4(data_.positions[i], 1.f);
        skinnedPositions[i] = glm::vec3(pos4);
    }

    // 6. 法線を再計算
    std::vector<glm::vec3> normals;
    recomputeNormals(normals, skinnedPositions);

    // 7. フラット配列 (x,y,z,nx,ny,nz,u,v) を生成して返す
    const bool hasTexcoords = !data_.texcoords.empty();
    std::vector<float> out;
    out.reserve(vertCount * 8);
    for (size_t i = 0; i < vertCount; i++) {
        out.push_back(skinnedPositions[i].x);
        out.push_back(skinnedPositions[i].y);
        out.push_back(skinnedPositions[i].z);
        out.push_back(normals[i].x);
        out.push_back(normals[i].y);
        out.push_back(normals[i].z);
        if (hasTexcoords && i < data_.texcoords.size()) {
            out.push_back(data_.texcoords[i].x);
            out.push_back(data_.texcoords[i].y);
        } else {
            out.push_back(0.f);
            out.push_back(0.f);
        }
    }
    return out;
}
