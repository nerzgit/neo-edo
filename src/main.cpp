#include "engine/renderer/material.h"
#include "engine/renderer/draw/draw.h"
#include "engine/renderer/shadow_map.h"
#include "engine/resource/mesh_factory.h"
#include "engine/resource/mesh_manager.h"
#include "engine/resource/shader_manager.h"
#include "engine/scene/directional_light.h"
#include "engine/scene/scene.h"
#include "engine/window/window.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

static const std::string kShaderDir = std::string(ASSETS_DIR) + "/shaders/";

int main() {
  try {
    Window window(800, 600, "neo-edo");
    Renderer renderer;

    // --- シェーダー ---
    ShaderManager shaderManager;
    shaderManager.loadFromFile("basic", kShaderDir + "basic.vert",
                               kShaderDir + "basic.frag");
    shaderManager.loadFromFile("shadow_depth",
                               kShaderDir + "shadow_depth.vert",
                               kShaderDir + "shadow_depth.frag");
    shaderManager.loadFromFile("lit", kShaderDir + "lit.vert",
                               kShaderDir + "lit.frag");

    // --- メッシュ ---
    MeshManager meshManager;
    meshManager.loadMesh("cube",   MeshFactory::cube());
    meshManager.loadMesh("sphere", MeshFactory::sphere());
    meshManager.loadMesh("plane",  MeshFactory::plane(6.0f, 6.0f));

    // --- マテリアル ---
    Material orangeMat;
    orangeMat.shader = &shaderManager.get("lit");
    orangeMat.color  = glm::vec4(0.8f, 0.5f, 0.2f, 1.0f);

    Material blueMat;
    blueMat.shader = &shaderManager.get("lit");
    blueMat.color  = glm::vec4(0.2f, 0.5f, 0.9f, 1.0f);

    Material grayMat;
    grayMat.shader = &shaderManager.get("lit");
    grayMat.color  = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);

    // --- シーン ---
    Scene scene;

    Entity &player = scene.createEntity();
    player.mesh                 = &meshManager.get("cube");
    player.material             = &orangeMat;
    player.transform.position   = glm::vec3(-1.0f, 0.0f, 0.0f);

    Entity &enemy = scene.createEntity();
    enemy.mesh                 = &meshManager.get("sphere");
    enemy.material             = &blueMat;
    enemy.transform.position   = glm::vec3(1.0f, 0.0f, 0.0f);

    // 影を受け取る床
    Entity &floor = scene.createEntity();
    floor.mesh               = &meshManager.get("plane");
    floor.material           = &grayMat;
    floor.transform.position = glm::vec3(0.0f, -0.75f, 0.0f);

    // --- ライトとシャドウマップ ---
    DirectionalLight light;
    light.direction       = glm::normalize(glm::vec3(-1.0f, -2.5f, -1.0f));
    light.ambientStrength = 0.15f;

    ShadowMap shadowMap(1024, 1024);

    float lastTime = static_cast<float>(glfwGetTime());

    while (!window.shouldClose()) {
      float now = static_cast<float>(glfwGetTime());
      float dt  = now - lastTime;
      lastTime  = now;

      // アニメーション
      player.transform.rotation.y += dt;
      enemy.transform.rotation.y  -= dt;

      scene.update(dt);

      // ライト空間行列を計算
      glm::mat4 lightSpaceMatrix = light.getLightSpaceMatrix();

      // ===== パス1: シャドウパス (ライト視点でデプス描画) =====
      renderer.beginShadowPass(shadowMap);
      scene.renderDepthPass(renderer, shaderManager.get("shadow_depth"),
                            lightSpaceMatrix);
      renderer.endShadowPass(800, 600);

      // ===== パス2: ライティングパス (シャドウマップ使用) =====
      renderer.beginFrame();
      scene.renderLit(renderer, shaderManager.get("lit"), lightSpaceMatrix,
                      shadowMap, light.direction, light.ambientStrength);

      window.swapBuffers();
      window.pollEvents();
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  return 0;
}
