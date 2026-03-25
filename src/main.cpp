#include "engine/renderer/material.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/shader.h"
#include "engine/scene/entity.h"
#include "engine/window/window.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

static const char *kVertSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

static const char *kFragSrc = R"(
#version 330 core
uniform vec4 color;
out vec4 FragColor;
void main() {
    FragColor = color;
}
)";

int main() {
  try {
    Window window(800, 600, "neo-edo");
    Renderer renderer;

    // Mesh（同じMeshを複数Entityで使い回せる）
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f, 0.5f,  -0.5f, 0.0f,
        0.5f,  0.5f,  0.0f, -0.5f, 0.5f,  0.0f,
    };
    std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    Mesh quadMesh(vertices, indices);

    // Shader / Material
    Shader defaultShader(kVertSrc, kFragSrc);

    Material orangeMat;
    orangeMat.shader = &defaultShader;
    orangeMat.color = glm::vec4(0.8f, 0.5f, 0.2f, 1.0f);

    Material blueMat;
    blueMat.shader = &defaultShader;
    blueMat.color = glm::vec4(0.2f, 0.5f, 0.9f, 1.0f);

    // Entity（同じMeshを使い回しつつ位置・色が異なる）
    Entity player;
    player.mesh = &quadMesh;
    player.material = &orangeMat;
    player.transform.position = glm::vec3(-1.0f, 0.0f, 0.0f);

    Entity enemy;
    enemy.mesh = &quadMesh;
    enemy.material = &blueMat;
    enemy.transform.position = glm::vec3(1.0f, 0.0f, 0.0f);

    std::vector<Entity *> entities = {&player, &enemy};

    while (!window.shouldClose()) {
      float time = static_cast<float>(glfwGetTime());

      // アニメーション
      player.transform.rotation.y = time;
      enemy.transform.rotation.y = -time;

      renderer.beginFrame();
      for (auto *e : entities) {
        renderer.draw(*e->mesh, e->transform, *e->material);
      }

      window.swapBuffers();
      window.pollEvents();
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  return 0;
}
