#include "input_handler.h"
#include <GLFW/glfw3.h>

InputHandler::InputHandler(GLFWwindow *window) : window_(window) {}

InputHandler::~InputHandler() {
  glfwSetWindowUserPointer(window_, nullptr);
  glfwSetCursorPosCallback(window_, nullptr);
}

void InputHandler::bindKey(int glfwKey, KeyCallback callback) {
  keyBindings_[glfwKey] = std::move(callback);
}

void InputHandler::onMouseMove(MouseCallback callback) {
  mouseCallback_ = std::move(callback);
  glfwSetWindowUserPointer(window_, this);
  glfwSetCursorPosCallback(window_, cursorCallback);
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputHandler::update(float dt) {
  for (auto &[key, cb] : keyBindings_) {
    if (glfwGetKey(window_, key) == GLFW_PRESS)
      cb(dt);
  }
}

void InputHandler::cursorCallback(GLFWwindow *win, double x, double y) {
  auto *self = static_cast<InputHandler *>(glfwGetWindowUserPointer(win));
  if (!self || !self->mouseCallback_) return;

  if (self->firstMouse_) {
    self->lastX_      = x;
    self->lastY_      = y;
    self->firstMouse_ = false;
    return;
  }

  float dx = static_cast<float>(x - self->lastX_);
  float dy = static_cast<float>(y - self->lastY_);
  self->lastX_ = x;
  self->lastY_ = y;

  self->mouseCallback_(dx, dy);
}
