#include "scene.h"
#include "../renderer/renderer.h"

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
    // TODO: ロジック更新（物理・スクリプトなど）
  }
}

void Scene::render(Renderer &renderer) {
  for (auto &e : entities_) {
    if (e->mesh && e->material) {
      renderer.draw(*e->mesh, e->transform, *e->material);
    }
  }
}
