#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
  Shader(const char *vertexSrc, const char *fragmentSrc);
  ~Shader();

  void bind() const;
  void setMat4(const std::string &name, const glm::mat4 &mat) const;
  void setVec4(const std::string &name, const glm::vec4 &v) const;

private:
  GLuint program_;
};
