#pragma once

#include "entity.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Renderer;
class Shader;
class ShadowMap;

class Scene {
public:
  Entity &createEntity();
  void removeEntity(const Entity *entity);

  void update(float dt);

  // フラットカラー描画 (既存)
  void render(Renderer &renderer);

  // シャドウパス: デプスのみ描画
  void renderDepthPass(Renderer &renderer, Shader &depthShader,
                       const glm::mat4 &lightSpaceMatrix);

  // ライティングパス: シャドウマップ使用
  void renderLit(Renderer &renderer, Shader &litShader,
                 const glm::mat4 &lightSpaceMatrix, const ShadowMap &shadowMap,
                 const glm::vec3 &lightDir, float ambientStrength);

private:
  // unique_ptr でヒープに固定 → emplace_back による再確保後も参照が有効
  std::vector<std::unique_ptr<Entity>> entities_;
};
