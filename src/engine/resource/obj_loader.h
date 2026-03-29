#pragma once

#include "../renderer/mesh.h"
#include "obj_parser.h"
#include <memory>
#include <string>

// OBJ ファイルからメッシュをロードするローダー。
// 頂点フォーマット: x, y, z, nx, ny, nz (6 floats/vertex)
class ObjLoader {
public:
  // エンジン組み込みアセット (ENGINE_ASSETS_DIR/<name>.obj) から読み込む
  static std::unique_ptr<Mesh> loadEngine(const std::string &name);

  // ユーザーアセット (ASSETS_DIR/<name>.obj) から読み込む
  static std::unique_ptr<Mesh> loadUser(const std::string &name);

private:
  // ObjData を Mesh に変換する。法線がない場合は (0, 1, 0) を入れる。
  static std::unique_ptr<Mesh> buildMesh(const ObjData &data);

  // glm::vec3 をフラットな float 配列に変換して buf に追加する
  static void pushVec3(std::vector<float> &buf, const glm::vec3 &v);
};
