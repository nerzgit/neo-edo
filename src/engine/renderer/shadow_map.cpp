#include "shadow_map.h"
#include <stdexcept>

ShadowMap::ShadowMap(int width, int height) : width_(width), height_(height) {
  // デプステクスチャ生成
  glGenTextures(1, &depthTexture_);
  glBindTexture(GL_TEXTURE_2D, depthTexture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // フラスタム外は影なし (borderColor = 1.0)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glBindTexture(GL_TEXTURE_2D, 0);

  // FBO生成してデプステクスチャをアタッチ
  glGenFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthTexture_, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    throw std::runtime_error("ShadowMap: framebuffer not complete");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap() {
  glDeleteFramebuffers(1, &fbo_);
  glDeleteTextures(1, &depthTexture_);
}

void ShadowMap::bindForWriting() const {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glViewport(0, 0, width_, height_);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::unbindForWriting(int vpWidth, int vpHeight) const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, vpWidth, vpHeight);
}

void ShadowMap::bindDepthTexture(int unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, depthTexture_);
}
