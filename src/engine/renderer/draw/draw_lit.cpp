#include "draw.h"
#include "../material.h"
#include "../mesh.h"
#include "../shader.h"
#include "../shadow_map.h"
#include "../../scene/transform.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

void Renderer::drawLit(const Mesh &mesh, const DrawLitParams &params) {
  const Transform &transform = params.transform;
  const Material  &material  = params.material;

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, transform.position);
  model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model, transform.scale);

  params.shadowMap.bindDepthTexture(1);

  params.shader.bind();
  params.shader.setMat4("model", model);
  params.shader.setMat4("view", params.camera.view);
  params.shader.setMat4("projection", params.camera.projection);
  params.shader.setMat4("lightSpaceMatrix", params.lightSpaceMatrix);
  params.shader.setVec4("color", material.color);
  params.shader.setVec3("lightDir", params.lightDir);
  params.shader.setFloat("ambientStrength", params.ambientStrength);
  params.shader.setInt("shadowMap", 1);

  mesh.bind();
  glDrawElements(GL_TRIANGLES, mesh.indexCount(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
