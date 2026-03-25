#include "engine/renderer/material.h"
#include "engine/renderer/renderer.h"
#include "engine/resource/mesh_factory.h"
#include "engine/resource/mesh_manager.h"
#include "engine/resource/shader_manager.h"
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

    // --- Shader ---
    ShaderManager shaderManager;
    shaderManager.loadFromFile("basic", kShaderDir + "basic.vert",
                               kShaderDir + "basic.frag");

    // --- Mesh（MeshFactory で生成して MeshManager に登録）---
    MeshManager meshManager;
    meshManager.loadMesh("cube",   MeshFactory::cube());
    meshManager.loadMesh("sphere", MeshFactory::sphere());
    meshManager.loadMesh("plane",  MeshFactory::plane(2.0f, 2.0f));

    // --- Material ---
    Material orangeMat;
    orangeMat.shader = &shaderManager.get("basic");
    orangeMat.color = glm::vec4(0.8f, 0.5f, 0.2f, 1.0f);

    Material blueMat;
    blueMat.shader = &shaderManager.get("basic");
    blueMat.color = glm::vec4(0.2f, 0.5f, 0.9f, 1.0f);

    // --- Scene ---
    Scene scene;

    Entity &player = scene.createEntity();
    player.mesh = &meshManager.get("cube");
    player.material = &orangeMat;
    player.transform.position = glm::vec3(-1.0f, 0.0f, 0.0f);

    Entity &enemy = scene.createEntity();
    enemy.mesh = &meshManager.get("sphere");
    enemy.material = &blueMat;
    enemy.transform.position = glm::vec3(1.0f, 0.0f, 0.0f);

    float lastTime = static_cast<float>(glfwGetTime());

    while (!window.shouldClose()) {
      float now = static_cast<float>(glfwGetTime());
      float dt = now - lastTime;
      lastTime = now;

      // アニメーション（Sceneのupdateに移動予定）
      player.transform.rotation.y += dt;
      enemy.transform.rotation.y -= dt;

      scene.update(dt);

      renderer.beginFrame();
      scene.render(renderer);

      window.swapBuffers();
      window.pollEvents();
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  return 0;
}
