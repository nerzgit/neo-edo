#include "gltf_loader.h"
#include "gltf_parser.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <utility>

#ifndef ENGINE_ASSETS_DIR
#define ENGINE_ASSETS_DIR "src/engine/resource/assets"
#endif
#ifndef ASSETS_DIR
#define ASSETS_DIR "assets"
#endif

// -----------------------------------------------------------------------
// GltfData → 静的 Mesh 変換
// -----------------------------------------------------------------------
static std::vector<float> buildFlatVerts(const GltfData& data) {
    const size_t count       = data.positions.size();
    const bool   hasNormals  = !data.normals.empty();
    const bool   hasTexcoords = !data.texcoords.empty();

    std::vector<float> flatVerts;
    flatVerts.reserve(count * 8);

    for (size_t i = 0; i < count; i++) {
        const float* ptr = glm::value_ptr(data.positions[i]);
        flatVerts.insert(flatVerts.end(), ptr, ptr + 3);

        const glm::vec3& n = hasNormals ? data.normals[i] : glm::vec3(0.f, 1.f, 0.f);
        const float* nptr = glm::value_ptr(n);
        flatVerts.insert(flatVerts.end(), nptr, nptr + 3);

        const glm::vec2 uv = hasTexcoords ? data.texcoords[i] : glm::vec2(0.f);
        flatVerts.push_back(uv.x);
        flatVerts.push_back(uv.y);
    }

    return flatVerts;
}

static std::unique_ptr<Mesh> buildMesh(const GltfData& data) {
    return std::make_unique<Mesh>(buildFlatVerts(data), data.indices);
}

// GltfData → ダイナミック Mesh 変換（スキニング用）
static std::unique_ptr<Mesh> buildDynamicMesh(const GltfData& data) {
    return std::make_unique<Mesh>(buildFlatVerts(data), data.indices, true);
}

static GLuint uploadTexture(const TextureImage& img) {
    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 img.width, img.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, img.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texId;
}

// -----------------------------------------------------------------------
// 公開 API
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> GltfLoader::load(const std::string& path) {
    return buildMesh(parseGltf(path));
}

std::unique_ptr<Mesh> GltfLoader::loadEngine(const std::string& name) {
    return buildMesh(parseGltf(std::string(ENGINE_ASSETS_DIR) + "/" + name));
}

std::unique_ptr<Mesh> GltfLoader::loadUser(const std::string& name) {
    return buildMesh(parseGltf(std::string(ASSETS_DIR) + "/" + name));
}

std::pair<std::unique_ptr<Mesh>, GltfData> GltfLoader::loadSkinned(const std::string& path) {
    GltfData data = parseGltf(path);
    std::unique_ptr<Mesh> mesh = buildDynamicMesh(data);
    return std::make_pair(std::move(mesh), std::move(data));
}

std::pair<std::unique_ptr<Mesh>, GltfData> GltfLoader::loadSkinnedUser(const std::string& name) {
    return loadSkinned(std::string(ASSETS_DIR) + "/" + name);
}

GltfLoader::SkinnedTextured GltfLoader::loadSkinnedUserTextured(const std::string& name) {
    GltfData data = parseGltf(std::string(ASSETS_DIR) + "/" + name);
    auto mesh = buildDynamicMesh(data);
    GLuint texId = 0;
    if (!data.images.empty())
        texId = uploadTexture(data.images[0]);
    return {std::move(mesh), std::move(data), texId};
}

std::pair<std::unique_ptr<Mesh>, GLuint> GltfLoader::loadUserTextured(const std::string& name) {
    GltfData data = parseGltf(std::string(ASSETS_DIR) + "/" + name);
    auto mesh = buildMesh(data);
    GLuint texId = 0;
    if (!data.images.empty())
        texId = uploadTexture(data.images[0]);
    return {std::move(mesh), texId};
}
