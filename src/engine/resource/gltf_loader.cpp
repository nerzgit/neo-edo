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
static std::unique_ptr<Mesh> buildMesh(const GltfData& data) {
    const size_t count = data.positions.size();
    const bool hasNormals = !data.normals.empty();

    std::vector<float> flatVerts;
    flatVerts.reserve(count * 6);

    for (size_t i = 0; i < count; i++) {
        const float* ptr = glm::value_ptr(data.positions[i]);
        flatVerts.insert(flatVerts.end(), ptr, ptr + 3);

        const glm::vec3& n = hasNormals ? data.normals[i] : glm::vec3(0.f, 1.f, 0.f);
        const float* nptr = glm::value_ptr(n);
        flatVerts.insert(flatVerts.end(), nptr, nptr + 3);
    }

    return std::make_unique<Mesh>(flatVerts, data.indices);
}

// GltfData → ダイナミック Mesh 変換（スキニング用）
static std::unique_ptr<Mesh> buildDynamicMesh(const GltfData& data) {
    const size_t count = data.positions.size();
    const bool hasNormals = !data.normals.empty();

    std::vector<float> flatVerts;
    flatVerts.reserve(count * 6);

    for (size_t i = 0; i < count; i++) {
        const float* ptr = glm::value_ptr(data.positions[i]);
        flatVerts.insert(flatVerts.end(), ptr, ptr + 3);

        const glm::vec3& n = hasNormals ? data.normals[i] : glm::vec3(0.f, 1.f, 0.f);
        const float* nptr = glm::value_ptr(n);
        flatVerts.insert(flatVerts.end(), nptr, nptr + 3);
    }

    // dynamic=true で作成
    return std::make_unique<Mesh>(flatVerts, data.indices, true);
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
