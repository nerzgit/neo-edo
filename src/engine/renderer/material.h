#pragma once

#include <glm/glm.hpp>

class Shader;

struct Material {
  Shader *shader = nullptr;
  glm::vec4 color{1.0f};
};
