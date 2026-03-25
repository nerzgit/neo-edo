#pragma once

#include "../renderer/mesh.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class MeshManager {
public:
  // 頂点・インデックス配列から直接ロード
  void load(const std::string &name, const std::vector<float> &vertices,
            const std::vector<unsigned int> &indices) {
    meshes_[name] = std::make_unique<Mesh>(vertices, indices);
  }

  // MeshFactory など外部で生成した Mesh を登録
  void loadMesh(const std::string &name, std::unique_ptr<Mesh> mesh) {
    meshes_[name] = std::move(mesh);
  }

  Mesh &get(const std::string &name) {
    auto it = meshes_.find(name);
    if (it == meshes_.end())
      throw std::runtime_error("Mesh not found: " + name);
    return *it->second;
  }

private:
  std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes_;
};
