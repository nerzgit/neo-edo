#pragma once

#include <glad/glad.h>
#include <memory>

class Shader;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void beginFrame();
    void draw();

private:
    std::unique_ptr<Shader> shader_;
    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;
};
