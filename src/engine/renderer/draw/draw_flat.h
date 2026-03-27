#pragma once

struct Transform;
struct Material;

struct DrawParams {
  const Transform &transform;
  const Material  &material;
};
