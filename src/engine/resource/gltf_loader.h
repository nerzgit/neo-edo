#pragma once

#include "../renderer/mesh.h"
#include "gltf_parser.h"
#include <memory>
#include <string>
#include <utility>

// glTF / glb ファイルからメッシュをロードするローダー。
// 頂点フォーマット: x, y, z, nx, ny, nz (6 floats/vertex)
// 最初のメッシュの最初のプリミティブを対象とする。
class GltfLoader {
public:
    // エンジン組み込みアセット (ENGINE_ASSETS_DIR/<name>) から読み込む
    static std::unique_ptr<Mesh> loadEngine(const std::string& name);

    // ユーザーアセット (ASSETS_DIR/<name>) から読み込む
    static std::unique_ptr<Mesh> loadUser(const std::string& name);

    // 任意の絶対パスから読み込む (.gltf / .glb 両対応)
    static std::unique_ptr<Mesh> load(const std::string& path);

    // スキニング用: dynamic Mesh と GltfData を同時に返す
    static std::pair<std::unique_ptr<Mesh>, GltfData> loadSkinned(const std::string& path);
    static std::pair<std::unique_ptr<Mesh>, GltfData> loadSkinnedUser(const std::string& name);
};
