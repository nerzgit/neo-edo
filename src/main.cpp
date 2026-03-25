#include "engine/window/window.h"
#include "engine/renderer/renderer.h"
#include <iostream>

int main() {
    try {
        Window window(800, 600, "neo-edo");
        Renderer renderer;

        while (!window.shouldClose()) {
            renderer.beginFrame();
            renderer.draw();

            window.swapBuffers();
            window.pollEvents();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}
