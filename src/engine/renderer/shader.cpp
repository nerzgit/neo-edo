#include "shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

static GLuint compileShader(GLenum type, const char *src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  GLint ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    glDeleteShader(shader);
    throw std::runtime_error(std::string("Shader compile error: ") + log);
  }
  return shader;
}

Shader::Shader(const char *vertexSrc, const char *fragmentSrc) {
  GLuint vert = compileShader(GL_VERTEX_SHADER, vertexSrc);
  GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

  program_ = glCreateProgram();
  glAttachShader(program_, vert);
  glAttachShader(program_, frag);
  glLinkProgram(program_);

  glDeleteShader(vert);
  glDeleteShader(frag);

  GLint ok;
  glGetProgramiv(program_, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
    throw std::runtime_error(std::string("Shader link error: ") + log);
  }
}

Shader::~Shader() { glDeleteProgram(program_); }

void Shader::bind() const { glUseProgram(program_); }

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(mat));
}

void Shader::setVec4(const std::string &name, const glm::vec4 &v) const {
  glUniform4fv(glGetUniformLocation(program_, name.c_str()), 1,
               glm::value_ptr(v));
}
