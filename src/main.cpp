#include "engine/camera/camera.h"
#include "engine/input/input_handler.h"
#include "engine/renderer/draw/draw.h"
#include "engine/renderer/shadow_map.h"
#include "engine/resource/mesh_factory.h"
#include "engine/resource/mesh_manager.h"
#include "engine/resource/shader_manager.h"
#include "engine/renderer/material.h"
#include "engine/scene/directional_light.h"
#include "engine/scene/scene.h"
#include "engine/window/window.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

static const std::string kShaderDir = std::string(ASSETS_DIR) + "/shaders/";

int main() {
    try {
        Window window(1280, 720, "Neo-Edo");
        Renderer renderer;

        Camera camera(glm::vec3(0.f, 3.f, 8.f));
        InputHandler input(window.handle());

        input.bindKey(GLFW_KEY_W,      [&](float dt){ camera.move(Camera::Direction::Forward,  dt); });
        input.bindKey(GLFW_KEY_S,      [&](float dt){ camera.move(Camera::Direction::Backward, dt); });
        input.bindKey(GLFW_KEY_A,      [&](float dt){ camera.move(Camera::Direction::Left,     dt); });
        input.bindKey(GLFW_KEY_D,      [&](float dt){ camera.move(Camera::Direction::Right,    dt); });
        input.bindKey(GLFW_KEY_E,      [&](float dt){ camera.move(Camera::Direction::Up,       dt); });
        input.bindKey(GLFW_KEY_Q,      [&](float dt){ camera.move(Camera::Direction::Down,     dt); });
        input.onMouseMove([&](float dx, float dy){ camera.rotate(dx, dy); });
        input.bindKey(GLFW_KEY_ESCAPE, [&](float){ window.close(); });

        ShaderManager shaders;
        shaders.loadFromFile("shadow_depth",
                             kShaderDir + "shadow_depth.vert",
                             kShaderDir + "shadow_depth.frag");
        shaders.loadFromFile("lit", kShaderDir + "lit.vert", kShaderDir + "lit.frag");

        MeshManager meshes;
        meshes.loadMesh("sphere", MeshFactory::sphere());
        meshes.loadMesh("cube",   MeshFactory::cube());
        meshes.loadMesh("plane",  MeshFactory::plane());

        Material sphereMat{&shaders.get("lit"), glm::vec4(0.8f, 0.3f, 0.3f, 1.f)};
        Material cubeMat  {&shaders.get("lit"), glm::vec4(0.3f, 0.5f, 0.8f, 1.f)};
        Material planeMat {&shaders.get("lit"), glm::vec4(0.5f, 0.5f, 0.5f, 1.f)};

        Scene scene;

        Entity& sphereEntity = scene.createEntity();
        sphereEntity.mesh     = &meshes.get("sphere");
        sphereEntity.material = &sphereMat;
        sphereEntity.transform.position = glm::vec3(-1.5f, 0.5f, 0.f);

        Entity& cubeEntity = scene.createEntity();
        cubeEntity.mesh     = &meshes.get("cube");
        cubeEntity.material = &cubeMat;
        cubeEntity.transform.position = glm::vec3(1.5f, 0.5f, 0.f);

        Entity& planeEntity = scene.createEntity();
        planeEntity.mesh     = &meshes.get("plane");
        planeEntity.material = &planeMat;
        planeEntity.transform.position = glm::vec3(0.f, 0.f, 0.f);
        planeEntity.transform.scale    = glm::vec3(12.f, 1.f, 12.f);

        DirectionalLight light;
        light.direction       = glm::normalize(glm::vec3(-1.f, -2.5f, -1.f));
        light.ambientStrength = 0.15f;
        ShadowMap shadowMap(1024, 1024);

        float lastTime = static_cast<float>(glfwGetTime());

        while (!window.shouldClose()) {
            const float now = static_cast<float>(glfwGetTime());
            const float dt  = now - lastTime;
            lastTime = now;

            input.update(dt);
            scene.update(dt);

            CameraMatrices cam{
                camera.getViewMatrix(),
                glm::perspective(glm::radians(45.f), 1280.f / 720.f, 0.1f, 100.f)
            };
            glm::mat4 lightSpace = light.getLightSpaceMatrix();

            renderer.beginShadowPass(shadowMap);
            scene.renderDepthPass(renderer, shaders.get("shadow_depth"), lightSpace);
            renderer.endShadowPass(1280, 720);

            renderer.clear();
            scene.renderLit(renderer, shaders.get("lit"),
                            cam, lightSpace, shadowMap,
                            light.direction, light.ambientStrength);

            window.swapBuffers();
            window.pollEvents();
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
