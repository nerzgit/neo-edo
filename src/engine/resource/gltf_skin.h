#pragma once

#include "gltf_parser.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

class GltfSkin {
public:
    explicit GltfSkin(const GltfData& data);

    // animName のアニメーションを time 秒でサンプルして頂点を更新
    // 戻り値: x,y,z,nx,ny,nz のフラット配列
    std::vector<float> update(const std::string& animName, float time);

    float duration(const std::string& animName) const;

private:
    // アニメーションサンプリング
    glm::vec3 sampleVec3(const AnimSampler& s, float t) const;
    glm::quat sampleQuat(const AnimSampler& s, float t) const;

    // ノードのグローバル行列を再帰計算
    glm::mat4 globalMatrix(int nodeIdx,
                            const std::vector<glm::vec3>& translations,
                            const std::vector<glm::quat>& rotations,
                            const std::vector<glm::vec3>& scales) const;

    // 法線をインデックスから再計算（フラット法線）
    void recomputeNormals(std::vector<glm::vec3>& normals,
                          const std::vector<glm::vec3>& positions) const;

    const GltfData& data_;
};
