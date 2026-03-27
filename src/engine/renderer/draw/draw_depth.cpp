#include "draw.h"
#include "../mesh.h"
#include "../shader.h"
#include "../shadow_map.h"
#include "../../scene/transform.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

void Renderer::beginShadowPass(const ShadowMap &shadowMap) {
  shadowMap.bindForWriting();
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
}

void Renderer::endShadowPass(int vpWidth, int vpHeight) {
  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, vpWidth, vpHeight);
}

void Renderer::drawDepth(const Mesh &mesh, const DrawDepthParams &params) {
  const Transform &transform = params.transform;

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, transform.position);
  model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, transform.scale);

  params.shader.bind();
  params.shader.setMat4("model", model);
  params.shader.setMat4("lightSpaceMatrix", params.lightSpaceMatrix);

  mesh.bind();
  glDrawElements(GL_TRIANGLES, mesh.indexCount(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
