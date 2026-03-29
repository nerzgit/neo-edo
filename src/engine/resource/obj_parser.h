#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct ObjData {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  // フェイスを構成する頂点ごとの (posIdx, normIdx)、1-based
  std::vector<std::pair<int, int>> faceVerts; // 3要素ごとに1三角形
};

// OBJ ファイルを読み込んで中間データを返す（Mesh 非依存）
ObjData parseObj(const std::string &path);

/**
 * 頂点データの行をパースして data.positions に追加する。
 * 頂点データとは、頂点の位置を表す行で、"v" で始まる。フォーマットは "v x y z"
 * で、x, y, z は頂点の座標を表す浮動小数点数。
 * 例: v 0.123 0.234 0.345
 */
static void parseVertex(std::istringstream &ss, ObjData &data);

/**
 * 法線データの行をパースして data.normals に追加する。
 * 法線データとは、頂点の法線ベクトルを表す行で、"vn" で始まる。フォーマットは
 * "vn x y z" で、x, y, z は法線ベクトルの成分を表す浮動小数点数。
 * 例: vn 0.707 0.000 0.707
 */
static void parseNormal(std::istringstream &ss, ObjData &data);

/**
 * フェイスデータの行をパースして data.faceVerts に追加する。
 * フェイスデータとは、頂点インデックスと法線インデックスを表す行で、"f"
 * で始まる。フォーマットは "f v1//vn1 v2//vn2 v3//vn3 ..." で、v
 * は頂点インデックス、vn は法線インデックスを表す整数。
 * インデックスは1-basedで、頂点インデックスと法線インデックスはスラッシュで区切られる。
 * 例: f 1//1 2//2 3//3
 */
static void parseFace(std::istringstream &ss, ObjData &data);