#pragma once

#include "entity.h"
#include <memory>
#include <vector>

class Renderer;

class Scene {
public:
  Entity &createEntity();
  void removeEntity(const Entity *entity);

  void update(float dt);
  void render(Renderer &renderer);

private:
  // unique_ptr でヒープに固定 → emplace_back による再確保後も参照が有効
  std::vector<std::unique_ptr<Entity>> entities_;
};
