#pragma once

#include "../../camera/camera.h"

struct Transform;
struct Material;

struct DrawParams {
  const Transform  &transform;
  const Material   &material;
  CameraMatrices    camera;
};
