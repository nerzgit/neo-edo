#include "scene.h"
#include "../renderer/draw/draw.h"
#include "../renderer/shadow_map.h"

Entity &Scene::createEntity() {
  entities_.push_back(std::make_unique<Entity>());
  return *entities_.back();
}

void Scene::removeEntity(const Entity *entity) {
  entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
                                 [entity](const std::unique_ptr<Entity> &e) {
                                   return e.get() == entity;
                                 }),
                  entities_.end());
}

void Scene::update(float dt) {
  for (auto &e : entities_) {
    (void)dt;
  }
}

void Scene::render(Renderer &renderer) {
  for (auto &e : entities_) {
    if (e->mesh && e->material)
      renderer.draw(*e->mesh, DrawParams{e->transform, *e->material});
  }
}

void Scene::renderDepthPass(Renderer &renderer, Shader &depthShader,
                            const glm::mat4 &lightSpaceMatrix) {
  for (auto &e : entities_) {
    if (e->mesh)
      renderer.drawDepth(*e->mesh,
                         DrawDepthParams{e->transform, depthShader, lightSpaceMatrix});
  }
}

void Scene::renderLit(Renderer &renderer, Shader &litShader,
                      const glm::mat4 &lightSpaceMatrix,
                      const ShadowMap &shadowMap, const glm::vec3 &lightDir,
                      float ambientStrength) {
  for (auto &e : entities_) {
    if (e->mesh && e->material)
      renderer.drawLit(*e->mesh,
                       DrawLitParams{e->transform, *e->material, litShader,
                                     lightSpaceMatrix, shadowMap, lightDir,
                                     ambientStrength});
  }
}
