#pragma once

#include "entity.h"
#include "../camera/camera.h"
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

  void render(Renderer &renderer, const CameraMatrices &camera);

  void renderDepthPass(Renderer &renderer, Shader &depthShader,
                       const glm::mat4 &lightSpaceMatrix);

  void renderLit(Renderer &renderer, Shader &litShader,
                 const CameraMatrices &camera,
                 const glm::mat4 &lightSpaceMatrix, const ShadowMap &shadowMap,
                 const glm::vec3 &lightDir, float ambientStrength);

private:
  std::vector<std::unique_ptr<Entity>> entities_;
};
