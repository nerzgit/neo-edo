#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

struct TextureImage {
    int width  = 0;
    int height = 0;
    std::vector<unsigned char> pixels; // RGBA 8bit
};

// ノード1つ分のローカル TRS
struct NodeData {
    int         parent = -1;
    std::string name;
    glm::vec3   translation{0.f};
    glm::quat   rotation{1.f, 0.f, 0.f, 0.f};
    glm::vec3   scale{1.f};
};

enum class AnimPath { Translation, Rotation, Scale };

struct AnimSampler {
    std::vector<float>     times;
    std::vector<glm::vec4> values; // T/S: xyz 使用, R: xyzw (quat)
};

struct AnimChannel {
    int      nodeIdx;
    AnimPath path;
    int      samplerIdx;
};

struct AnimData {
    std::string              name;
    std::vector<AnimSampler> samplers;
    std::vector<AnimChannel> channels;
};

struct GltfData {
    // geometry
    std::vector<glm::vec3>    positions;
    std::vector<glm::vec3>    normals;
    std::vector<glm::vec2>    texcoords; // TEXCOORD_0
    std::vector<unsigned int> indices;
    // textures
    std::vector<TextureImage> images;    // baseColorTexture
    // skinning
    std::vector<glm::uvec4>   joints;   // JOINTS_0
    std::vector<glm::vec4>    weights;  // WEIGHTS_0
    // skeleton
    std::vector<NodeData>     nodes;
    std::vector<int>          skinJoints;          // joint の node インデックス
    std::vector<glm::mat4>    inverseBindMatrices;
    // animations
    std::vector<AnimData>     animations;
};

// glTF / glb ファイルを読み込んで中間データを返す（Mesh 非依存）
// 最初のメッシュの最初のプリミティブを対象とする。
GltfData parseGltf(const std::string &path);
