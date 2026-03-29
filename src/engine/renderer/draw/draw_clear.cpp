#include "draw.h"
#include <glad/glad.h>

Renderer::Renderer() { glEnable(GL_DEPTH_TEST); }

void Renderer::clear(const DrawClearParams &) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
