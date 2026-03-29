#pragma once

#include "draw_clear.h"
#include "draw_flat.h"
#include "draw_depth.h"
#include "draw_lit.h"

class Mesh;
class ShadowMap;

class Renderer {
public:
  Renderer();
  void clear(const DrawClearParams &params = {});
  void draw(const Mesh &mesh, const DrawParams &params);
  void beginShadowPass(const ShadowMap &shadowMap);
  void drawDepth(const Mesh &mesh, const DrawDepthParams &params);
  void endShadowPass(int vpWidth, int vpHeight);
  void drawLit(const Mesh &mesh, const DrawLitParams &params);
};
