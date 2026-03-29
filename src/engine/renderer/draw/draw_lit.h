#pragma once

#include "../../camera/camera.h"
#include <glm/glm.hpp>

struct Transform;
struct Material;
class Shader;
class ShadowMap;

struct DrawLitParams {
  const Transform  &transform;
  const Material   &material;
  const Shader     &shader;
  CameraMatrices    camera;
  glm::mat4         lightSpaceMatrix;
  const ShadowMap  &shadowMap;
  glm::vec3         lightDir;
  float             ambientStrength;
};
