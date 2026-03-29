#pragma once

#include <glad/glad.h>
#include <vector>

class Mesh {
public:
  // dynamic=true にすると GL_DYNAMIC_DRAW で VBO を確保し updateVertices が使える
  Mesh(const std::vector<float> &vertices,
       const std::vector<unsigned int> &indices,
       bool dynamic = false);
  ~Mesh();

  // dynamic=true で作ったメッシュの頂点データを毎フレーム書き換える
  // vertices のサイズは構築時と同じであること
  void updateVertices(const std::vector<float> &vertices);

  void bind() const;
  int indexCount() const { return indexCount_; }

private:
  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  int indexCount_;
};
