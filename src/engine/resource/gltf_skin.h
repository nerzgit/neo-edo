#pragma once

#include "gltf_parser.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

// srcAnim のノードインデックスを dstNodes のボーン名ベースで再マッピングする。
// マッピングはボーン名の文字列一致で行われる。
// srcNodes と dstNodes でボーン名が一致するチャンネルのみ移植され、
// 一致しないチャンネルは無視される。
// 異なるリグ間で流用する場合はボーン名が揃っていることが前提。
AnimData retargetAnim(const AnimData&              srcAnim,
                      const std::vector<NodeData>& srcNodes,
                      const std::vector<NodeData>& dstNodes);

class GltfSkin {
public:
    explicit GltfSkin(const GltfData& data);

    // animName のアニメーションを time 秒でサンプルして頂点を更新
    // 戻り値: x,y,z,nx,ny,nz のフラット配列
    std::vector<float> update(const std::string& animName, float time);

    float duration(const std::string& animName) const;

    // update() 後に指定ボーン名のグローバル行列を返す
    // ボーン名が見つからない場合は単位行列を返す
    glm::mat4 getBoneWorldMatrix(const std::string& boneName) const;

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

    // update() でサンプルした TRS をキャッシュ（getBoneWorldMatrix で使用）
    std::vector<glm::vec3> cachedTranslations_;
    std::vector<glm::quat> cachedRotations_;
    std::vector<glm::vec3> cachedScales_;
};
