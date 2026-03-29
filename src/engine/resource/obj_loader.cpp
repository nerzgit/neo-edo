#include "obj_loader.h"
#include "obj_parser.h"

#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <stdexcept>
#include <vector>

// ENGINE_ASSETS_DIR / ASSETS_DIR はビルド時に CMake が定義する
#ifndef ENGINE_ASSETS_DIR
#define ENGINE_ASSETS_DIR "src/engine/resource/assets"
#endif
#ifndef ASSETS_DIR
#define ASSETS_DIR "assets"
#endif

// -----------------------------------------------------------------------
// ObjData → Mesh 変換
// -----------------------------------------------------------------------
void ObjLoader::pushVec3(std::vector<float>& buf, const glm::vec3& v) {
    buf.insert(buf.end(), glm::value_ptr(v), glm::value_ptr(v) + 3);
}

std::unique_ptr<Mesh> ObjLoader::buildMesh(const ObjData& data) {
    std::map<std::pair<int, int>, unsigned int> vertexMap;
    std::vector<float>        flatVerts;
    std::vector<unsigned int> indices;

    for (const auto& [posIdx, normIdx] : data.faceVerts) {
        auto key = std::make_pair(posIdx, normIdx);
        auto it  = vertexMap.find(key);
        if (it == vertexMap.end()) {
            auto vi = static_cast<unsigned int>(flatVerts.size() / 6);
            vertexMap[key] = vi;
            indices.push_back(vi);
            pushVec3(flatVerts, data.positions[posIdx - 1]);
            pushVec3(flatVerts, normIdx > 0 ? data.normals[normIdx - 1]
                                            : glm::vec3(0.f, 1.f, 0.f));
        } else {
            indices.push_back(it->second);
        }
    }

    if (flatVerts.empty())
        throw std::runtime_error("ObjLoader: no geometry");

    return std::make_unique<Mesh>(flatVerts, indices);
}

// -----------------------------------------------------------------------
// 公開 API
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> ObjLoader::loadEngine(const std::string& name) {
    return buildMesh(parseObj(std::string(ENGINE_ASSETS_DIR) + "/" + name + ".obj"));
}

std::unique_ptr<Mesh> ObjLoader::loadUser(const std::string& name) {
    return buildMesh(parseObj(std::string(ASSETS_DIR) + "/" + name + ".obj"));
}
