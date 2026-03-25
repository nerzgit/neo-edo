#include "mesh_factory.h"
#include <cmath>
#include <vector>

static constexpr float kPi = 3.14159265358979323846f;

// -----------------------------------------------------------------------
// Cube
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::cube(float size) {
  float h = size * 0.5f;

  //       7----6
  //      /|   /|
  //     3----2 |
  //     | 4--|-5
  //     |/   |/
  //     0----1

  std::vector<float> v = {
      -h, -h, -h, // 0
       h, -h, -h, // 1
       h,  h, -h, // 2
      -h,  h, -h, // 3
      -h, -h,  h, // 4
       h, -h,  h, // 5
       h,  h,  h, // 6
      -h,  h,  h, // 7
  };

  std::vector<unsigned int> idx = {
      4, 5, 6, 4, 6, 7, // front  (+z)
      1, 0, 3, 1, 3, 2, // back   (-z)
      0, 4, 7, 0, 7, 3, // left   (-x)
      5, 1, 2, 5, 2, 6, // right  (+x)
      0, 1, 5, 0, 5, 4, // bottom (-y)
      3, 7, 6, 3, 6, 2, // top    (+y)
  };

  return std::make_unique<Mesh>(v, idx);
}

// -----------------------------------------------------------------------
// Plane  (XZ平面, 法線 +Y)
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::plane(float width, float depth, int subdivW,
                                         int subdivD) {
  std::vector<float> v;
  std::vector<unsigned int> idx;

  for (int j = 0; j <= subdivD; j++) {
    for (int i = 0; i <= subdivW; i++) {
      float x = (i / float(subdivW) - 0.5f) * width;
      float z = (j / float(subdivD) - 0.5f) * depth;
      v.push_back(x);
      v.push_back(0.0f);
      v.push_back(z);
    }
  }

  for (int j = 0; j < subdivD; j++) {
    for (int i = 0; i < subdivW; i++) {
      unsigned int tl = j * (subdivW + 1) + i;
      unsigned int tr = tl + 1;
      unsigned int bl = tl + (subdivW + 1);
      unsigned int br = bl + 1;
      // CCW from +Y
      idx.push_back(tl); idx.push_back(bl); idx.push_back(br);
      idx.push_back(tl); idx.push_back(br); idx.push_back(tr);
    }
  }

  return std::make_unique<Mesh>(v, idx);
}

// -----------------------------------------------------------------------
// Sphere  (UV球, Y軸が極)
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::sphere(float radius, int stacks, int slices) {
  std::vector<float> v;
  std::vector<unsigned int> idx;

  for (int i = 0; i <= stacks; i++) {
    float phi = kPi * i / float(stacks); // 0 (top) → π (bottom)
    for (int j = 0; j <= slices; j++) {
      float theta = 2.0f * kPi * j / float(slices);
      v.push_back(radius * std::sin(phi) * std::cos(theta)); // x
      v.push_back(radius * std::cos(phi));                   // y
      v.push_back(radius * std::sin(phi) * std::sin(theta)); // z
    }
  }

  for (int i = 0; i < stacks; i++) {
    for (int j = 0; j < slices; j++) {
      unsigned int tl = i * (slices + 1) + j;
      unsigned int tr = tl + 1;
      unsigned int bl = tl + (slices + 1);
      unsigned int br = bl + 1;
      idx.push_back(tl); idx.push_back(bl); idx.push_back(br);
      idx.push_back(tl); idx.push_back(br); idx.push_back(tr);
    }
  }

  return std::make_unique<Mesh>(v, idx);
}

// -----------------------------------------------------------------------
// Cylinder  (側面 + 上下キャップ)
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::cylinder(float radius, float height,
                                             int slices) {
  std::vector<float> v;
  std::vector<unsigned int> idx;
  float halfH = height * 0.5f;

  // 底リング [0, slices-1]、上リング [slices, 2*slices-1]
  for (int i = 0; i < slices; i++) {
    float theta = 2.0f * kPi * i / float(slices);
    float x = radius * std::cos(theta);
    float z = radius * std::sin(theta);
    v.push_back(x); v.push_back(-halfH); v.push_back(z); // 底
    v.push_back(x); v.push_back( halfH); v.push_back(z); // 上
  }

  // 側面
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    unsigned int b0 = i * 2,       t0 = i * 2 + 1;
    unsigned int b1 = next * 2,    t1 = next * 2 + 1;
    idx.push_back(b0); idx.push_back(b1); idx.push_back(t0);
    idx.push_back(t0); idx.push_back(b1); idx.push_back(t1);
  }

  // 底キャップ中心 [2*slices]
  unsigned int bottomCenter = static_cast<unsigned int>(v.size() / 3);
  v.push_back(0.0f); v.push_back(-halfH); v.push_back(0.0f);
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    idx.push_back(bottomCenter);
    idx.push_back(static_cast<unsigned int>(next * 2));
    idx.push_back(static_cast<unsigned int>(i * 2));
  }

  // 上キャップ中心 [2*slices+1]
  unsigned int topCenter = static_cast<unsigned int>(v.size() / 3);
  v.push_back(0.0f); v.push_back(halfH); v.push_back(0.0f);
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    idx.push_back(topCenter);
    idx.push_back(static_cast<unsigned int>(i * 2 + 1));
    idx.push_back(static_cast<unsigned int>(next * 2 + 1));
  }

  return std::make_unique<Mesh>(v, idx);
}

// -----------------------------------------------------------------------
// Cone  (側面 + 底面キャップ)
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::cone(float radius, float height, int slices) {
  std::vector<float> v;
  std::vector<unsigned int> idx;
  float halfH = height * 0.5f;

  // 底リング [0, slices-1]
  for (int i = 0; i < slices; i++) {
    float theta = 2.0f * kPi * i / float(slices);
    v.push_back(radius * std::cos(theta));
    v.push_back(-halfH);
    v.push_back(radius * std::sin(theta));
  }

  // 頂点 [slices]
  unsigned int apex = static_cast<unsigned int>(v.size() / 3);
  v.push_back(0.0f); v.push_back(halfH); v.push_back(0.0f);

  // 底キャップ中心 [slices+1]
  unsigned int bottomCenter = static_cast<unsigned int>(v.size() / 3);
  v.push_back(0.0f); v.push_back(-halfH); v.push_back(0.0f);

  // 側面
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    idx.push_back(static_cast<unsigned int>(i));
    idx.push_back(static_cast<unsigned int>(next));
    idx.push_back(apex);
  }

  // 底面
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    idx.push_back(bottomCenter);
    idx.push_back(static_cast<unsigned int>(next));
    idx.push_back(static_cast<unsigned int>(i));
  }

  return std::make_unique<Mesh>(v, idx);
}

// -----------------------------------------------------------------------
// Circle  (XZ平面上の円盤, 法線 +Y)
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::circle(float radius, int segments) {
  std::vector<float> v;
  std::vector<unsigned int> idx;

  // 中心 [0]
  v.push_back(0.0f); v.push_back(0.0f); v.push_back(0.0f);

  // リング [1, segments]
  for (int i = 0; i < segments; i++) {
    float theta = 2.0f * kPi * i / float(segments);
    v.push_back(radius * std::cos(theta));
    v.push_back(0.0f);
    v.push_back(radius * std::sin(theta));
  }

  // ファン三角形（CCW from +Y）
  for (int i = 0; i < segments; i++) {
    unsigned int curr = 1 + i;
    unsigned int next = 1 + (i + 1) % segments;
    idx.push_back(0);
    idx.push_back(curr);
    idx.push_back(next);
  }

  return std::make_unique<Mesh>(v, idx);
}

// -----------------------------------------------------------------------
// Screen Quad  (NDC座標, ポストエフェクト用フルスクリーンクワッド)
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::screenQuad() {
  // z=0 の NDC フルスクリーン。MVP不要、頂点シェーダーでそのまま使う。
  // TexCoord は頂点シェーダー側で  pos.xy * 0.5 + 0.5  で計算する。
  std::vector<float> v = {
      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,
  };
  std::vector<unsigned int> idx = {0, 1, 2, 2, 3, 0};

  return std::make_unique<Mesh>(v, idx);
}
