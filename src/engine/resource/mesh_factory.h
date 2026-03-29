#pragma once

#include "gltf_loader.h"
#include "obj_loader.h"
#include "../renderer/mesh.h"
#include <memory>
#include <string>

// エンジン組み込みプリミティブ (resource/assets/*.obj) をロードするファクトリ。
// ユーザー定義モデルは fromObj() / fromGltf() で root/assets/ から読み込む。
struct MeshFactory {
    static std::unique_ptr<Mesh> cube()       { return ObjLoader::loadEngine("cube"); }
    static std::unique_ptr<Mesh> plane()      { return ObjLoader::loadEngine("plane"); }
    static std::unique_ptr<Mesh> sphere()     { return ObjLoader::loadEngine("sphere"); }
    static std::unique_ptr<Mesh> cylinder()   { return ObjLoader::loadEngine("cylinder"); }
    static std::unique_ptr<Mesh> cone()       { return ObjLoader::loadEngine("cone"); }
    static std::unique_ptr<Mesh> circle()     { return ObjLoader::loadEngine("circle"); }
    static std::unique_ptr<Mesh> screenQuad() { return ObjLoader::loadEngine("screen_quad"); }

    // root/assets/<name>.obj
    static std::unique_ptr<Mesh> fromObj(const std::string& name)  { return ObjLoader::loadUser(name); }
    // root/assets/<name>  (.gltf / .glb)
    static std::unique_ptr<Mesh> fromGltf(const std::string& name) { return GltfLoader::loadUser(name); }
};
