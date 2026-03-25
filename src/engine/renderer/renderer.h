#pragma once

class Mesh;
struct Transform;
struct Material;

class Renderer {
public:
  Renderer();

  void beginFrame();
  void draw(const Mesh &mesh, const Transform &transform,
            const Material &material);
};
