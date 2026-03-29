#pragma once

#include <glm/glm.hpp>

struct CameraMatrices {
  glm::mat4 view;
  glm::mat4 projection;
};

class Camera {
public:
  enum class Direction { Forward, Backward, Left, Right, Up, Down };

  explicit Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f));

  glm::mat4 getViewMatrix() const;
  glm::vec3 position() const { return position_; }

  void move(Direction dir, float dt);
  void rotate(float dx, float dy);

  float speed       = 3.0f;
  float sensitivity = 0.1f;

private:
  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 right_;
  glm::vec3 up_;
  float yaw_   = -90.0f;
  float pitch_ =   0.0f;

  void updateVectors();
};
