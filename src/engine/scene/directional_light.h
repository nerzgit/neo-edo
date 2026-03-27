#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 平行光源 (太陽光などの無限遠光源)
struct DirectionalLight {
  // ライトが照らす方向 (ライトからシーンへ)
  glm::vec3 direction = glm::normalize(glm::vec3(-1.0f, -2.0f, -1.0f));
  glm::vec3 color     = glm::vec3(1.0f);
  float ambientStrength = 0.15f;

  // シャドウマッピング用: ライト視点からのビュー*プロジェクション行列
  // sceneCenterはシーンの中心, sceneRadiusはシーンをカバーする半径
  glm::mat4 getLightSpaceMatrix(const glm::vec3 &sceneCenter = glm::vec3(0.0f),
                                float sceneRadius = 6.0f) const {
    glm::vec3 lightPos = sceneCenter - glm::normalize(direction) * (sceneRadius * 2.0f);
    glm::mat4 lightView =
        glm::lookAt(lightPos, sceneCenter, glm::vec3(0.0f, 1.0f, 0.0f));
    float r = sceneRadius;
    glm::mat4 lightProj =
        glm::ortho(-r, r, -r, r, 0.1f, sceneRadius * 6.0f);
    return lightProj * lightView;
  }
};
