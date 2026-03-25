#version 330 core

// Post-processing blur shader.
// Expects a fullscreen quad with a screen texture bound to unit 0.
// Use with a framebuffer: render the scene to a texture, then draw
// a fullscreen quad with this shader.

uniform sampler2D screenTexture;
uniform vec2 texelSize;   // vec2(1.0 / screenWidth, 1.0 / screenHeight)
uniform float radius;     // blur radius in pixels (e.g. 1.0)

in vec2 TexCoord;

out vec4 FragColor;

void main() {
    vec4 result = vec4(0.0);
    float total  = 0.0;

    int steps = int(radius);
    for (int x = -steps; x <= steps; x++) {
        for (int y = -steps; y <= steps; y++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(screenTexture, TexCoord + offset);
            total  += 1.0;
        }
    }

    FragColor = result / total;
}
