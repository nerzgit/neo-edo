#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
  Window(int width, int height, const std::string &title);
  ~Window();

  bool shouldClose() const;
  void swapBuffers();
  void pollEvents();

  GLFWwindow *handle() const { return window_; }

private:
  GLFWwindow *window_ = nullptr;
};
