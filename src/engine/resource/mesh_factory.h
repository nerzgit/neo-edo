#pragma once

#include "../renderer/mesh.h"
#include <memory>

class MeshFactory {
public:
  // 1x1x1 の箱
  static std::unique_ptr<Mesh> cube(float size = 1.0f);

  // XZ平面上のグリッド（subdivで分割数を指定）
  static std::unique_ptr<Mesh> plane(float width = 1.0f, float depth = 1.0f,
                                     int subdivW = 1, int subdivD = 1);

  // UV球
  static std::unique_ptr<Mesh> sphere(float radius = 0.5f, int stacks = 16,
                                      int slices = 16);

  // 円柱（上下キャップ付き）
  static std::unique_ptr<Mesh> cylinder(float radius = 0.5f,
                                        float height = 1.0f, int slices = 16);

  // 円錐（底面キャップ付き）
  static std::unique_ptr<Mesh> cone(float radius = 0.5f, float height = 1.0f,
                                    int slices = 16);

  // XZ平面上の円盤
  static std::unique_ptr<Mesh> circle(float radius = 0.5f, int segments = 32);

  // NDCフルスクリーンクワッド（ポストエフェクト用）
  static std::unique_ptr<Mesh> screenQuad();
};
