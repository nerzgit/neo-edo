#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader;

struct Material {
  Shader *shader  = nullptr;
  glm::vec4 color{1.0f};
  GLuint albedoTex = 0; // 0 = テクスチャなし (color を使用)
};
