#include "draw.h"
#include "../material.h"
#include "../mesh.h"
#include "../shader.h"
#include "../../scene/transform.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

void Renderer::draw(const Mesh &mesh, const DrawParams &params) {
  const Transform &transform = params.transform;
  const Material  &material  = params.material;

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, transform.position);
  model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, transform.scale);

  material.shader->bind();
  material.shader->setMat4("model", model);
  material.shader->setMat4("view", params.camera.view);
  material.shader->setMat4("projection", params.camera.projection);
  material.shader->setVec4("color", material.color);

  mesh.bind();
  glDrawElements(GL_TRIANGLES, mesh.indexCount(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
