#include "window.h"
#include <stdexcept>

Window::Window(int width, int height, const std::string &title) {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(window_);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLAD");
  }
}

Window::~Window() { glfwTerminate(); }

bool Window::shouldClose() const { return glfwWindowShouldClose(window_); }

void Window::close() { glfwSetWindowShouldClose(window_, true); }

void Window::swapBuffers() { glfwSwapBuffers(window_); }

void Window::pollEvents() { glfwPollEvents(); }
