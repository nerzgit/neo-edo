#pragma once

#include <glad/glad.h>
#include <vector>

class Mesh {
public:
  Mesh(const std::vector<float> &vertices,
       const std::vector<unsigned int> &indices);
  ~Mesh();

  void bind() const;
  int indexCount() const { return indexCount_; }

private:
  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  int indexCount_;
};
