#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

Camera::Camera(glm::vec3 position) : position_(position) {
  updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::move(Direction dir, float dt) {
  float v = speed * dt;
  switch (dir) {
    case Direction::Forward:  position_ += front_ * v; break;
    case Direction::Backward: position_ -= front_ * v; break;
    case Direction::Left:     position_ -= right_ * v; break;
    case Direction::Right:    position_ += right_ * v; break;
    case Direction::Up:       position_ += up_    * v; break;
    case Direction::Down:     position_ -= up_    * v; break;
  }
}

void Camera::rotate(float dx, float dy) {
  yaw_   += dx * sensitivity;
  pitch_ -= dy * sensitivity;
  pitch_  = std::clamp(pitch_, -89.0f, 89.0f);
  updateVectors();
}

void Camera::updateVectors() {
  float yawR   = glm::radians(yaw_);
  float pitchR = glm::radians(pitch_);
  front_ = glm::normalize(glm::vec3(
      std::cos(yawR) * std::cos(pitchR),
      std::sin(pitchR),
      std::sin(yawR) * std::cos(pitchR)));
  right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f)));
  up_    = glm::normalize(glm::cross(right_, front_));
}
