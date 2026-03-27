#pragma once

#include <glad/glad.h>

class ShadowMap {
public:
  explicit ShadowMap(int width = 1024, int height = 1024);
  ~ShadowMap();

  // シャドウパス開始: FBOにバインドしてビューポートをシャドウ解像度に設定
  void bindForWriting() const;

  // シャドウパス終了: デフォルトFBOに戻してビューポートを復元
  void unbindForWriting(int vpWidth, int vpHeight) const;

  // メインパスでデプステクスチャをバインド
  void bindDepthTexture(int unit) const;

  int width() const { return width_; }
  int height() const { return height_; }

private:
  GLuint fbo_;
  GLuint depthTexture_;
  int width_;
  int height_;
};
