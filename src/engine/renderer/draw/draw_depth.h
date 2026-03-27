#pragma once

#include <glm/glm.hpp>

struct Transform;
class Shader;

struct DrawDepthParams {
  const Transform  &transform;
  const Shader     &shader;
  glm::mat4         lightSpaceMatrix;
};
