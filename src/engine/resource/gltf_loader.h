#pragma once

#include "../renderer/mesh.h"
#include "gltf_parser.h"
#include <glad/glad.h>
#include <memory>
#include <string>
#include <utility>

// glTF / glb ファイルからメッシュをロードするローダー。
// 頂点フォーマット: x, y, z, nx, ny, nz, u, v (8 floats/vertex)
// 最初のメッシュの最初のプリミティブを対象とする。
class GltfLoader {
public:
    // エンジン組み込みアセット (ENGINE_ASSETS_DIR/<name>) から読み込む
    static std::unique_ptr<Mesh> loadEngine(const std::string& name);

    // ユーザーアセット (ASSETS_DIR/<name>) から読み込む
    static std::unique_ptr<Mesh> loadUser(const std::string& name);

    // 任意の絶対パスから読み込む (.gltf / .glb 両対応)
    static std::unique_ptr<Mesh> load(const std::string& path);

    // テクスチャ付き: Mesh と OpenGL テクスチャ ID を同時に返す (texId=0 はテクスチャなし)
    static std::pair<std::unique_ptr<Mesh>, GLuint> loadUserTextured(const std::string& name);

    // スキニング用: dynamic Mesh と GltfData を同時に返す
    static std::pair<std::unique_ptr<Mesh>, GltfData> loadSkinned(const std::string& path);
    static std::pair<std::unique_ptr<Mesh>, GltfData> loadSkinnedUser(const std::string& name);

    // スキニング＋テクスチャ: dynamic Mesh, GltfData, テクスチャ ID を同時に返す
    struct SkinnedTextured {
        std::unique_ptr<Mesh> mesh;
        GltfData              data;
        GLuint                texId = 0;
    };
    static SkinnedTextured loadSkinnedUserTextured(const std::string& name);
};
