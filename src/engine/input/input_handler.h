#pragma once

#include <functional>
#include <unordered_map>

struct GLFWwindow;

class InputHandler {
public:
  using KeyCallback   = std::function<void(float dt)>;
  using MouseCallback = std::function<void(float dx, float dy)>;

  explicit InputHandler(GLFWwindow *window);
  ~InputHandler();

  // キーを押し続けている間、毎フレーム callback(dt) を呼ぶ
  void bindKey(int glfwKey, KeyCallback callback);

  // マウス移動時に callback(dx, dy) を呼ぶ
  // 登録するとカーソルが非表示になる
  void onMouseMove(MouseCallback callback);

  // 毎フレーム呼ぶ: 押下キーの確認 + マウスデルタの処理
  void update(float dt);

private:
  GLFWwindow *window_;
  std::unordered_map<int, KeyCallback> keyBindings_;
  MouseCallback mouseCallback_;
  double lastX_      = 0.0;
  double lastY_      = 0.0;
  bool   firstMouse_ = true;

  static void cursorCallback(GLFWwindow *win, double x, double y);
};
