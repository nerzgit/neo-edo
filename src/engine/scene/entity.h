#pragma once

#include "transform.h"

class Mesh;
struct Material;

struct Entity {
  Mesh *mesh = nullptr;
  Transform transform;
  Material *material = nullptr;
};
