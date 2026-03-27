#include "mesh_factory.h"
#include <cmath>
#include <vector>

static constexpr float kPi = 3.14159265358979323846f;

// Vertex format: x, y, z, nx, ny, nz (6 floats per vertex)

// -----------------------------------------------------------------------
// Cube  (各面に正確なフラット法線を持つ24頂点)
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::cube(float size) {
  float h = size * 0.5f;

  std::vector<float> v;
  v.reserve(24 * 6);

  auto push = [&](float x, float y, float z, float nx, float ny, float nz) {
    v.insert(v.end(), {x, y, z, nx, ny, nz});
  };

  // Front (+z) — normal (0,0,1)
  push(-h, -h,  h,  0,0,1);
  push( h, -h,  h,  0,0,1);
  push( h,  h,  h,  0,0,1);
  push(-h,  h,  h,  0,0,1);

  // Back (-z) — normal (0,0,-1)
  push( h, -h, -h,  0,0,-1);
  push(-h, -h, -h,  0,0,-1);
  push(-h,  h, -h,  0,0,-1);
  push( h,  h, -h,  0,0,-1);

  // Left (-x) — normal (-1,0,0)
  push(-h, -h, -h, -1,0,0);
  push(-h, -h,  h, -1,0,0);
  push(-h,  h,  h, -1,0,0);
  push(-h,  h, -h, -1,0,0);

  // Right (+x) — normal (1,0,0)
  push( h, -h,  h,  1,0,0);
  push( h, -h, -h,  1,0,0);
  push( h,  h, -h,  1,0,0);
  push( h,  h,  h,  1,0,0);

  // Bottom (-y) — normal (0,-1,0)
  push(-h, -h, -h,  0,-1,0);
  push( h, -h, -h,  0,-1,0);
  push( h, -h,  h,  0,-1,0);
  push(-h, -h,  h,  0,-1,0);

  // Top (+y) — normal (0,1,0)
  push(-h,  h,  h,  0,1,0);
  push( h,  h,  h,  0,1,0);
  push( h,  h, -h,  0,1,0);
  push(-h,  h, -h,  0,1,0);

  std::vector<unsigned int> idx;
  idx.reserve(36);
  for (unsigned int face = 0; face < 6; face++) {
    unsigned int b = face * 4;
    idx.insert(idx.end(), {b, b+1, b+2,  b, b+2, b+3});
  }

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
      v.insert(v.end(), {x, 0.0f, z,  0.0f, 1.0f, 0.0f});
    }
  }

  for (int j = 0; j < subdivD; j++) {
    for (int i = 0; i < subdivW; i++) {
      unsigned int tl = j * (subdivW + 1) + i;
      unsigned int tr = tl + 1;
      unsigned int bl = tl + (subdivW + 1);
      unsigned int br = bl + 1;
      idx.push_back(tl); idx.push_back(bl); idx.push_back(br);
      idx.push_back(tl); idx.push_back(br); idx.push_back(tr);
    }
  }

  return std::make_unique<Mesh>(v, idx);
}

// -----------------------------------------------------------------------
// Sphere  (UV球, 法線 = normalize(position))
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::sphere(float radius, int stacks, int slices) {
  std::vector<float> v;
  std::vector<unsigned int> idx;

  for (int i = 0; i <= stacks; i++) {
    float phi = kPi * i / float(stacks);
    for (int j = 0; j <= slices; j++) {
      float theta = 2.0f * kPi * j / float(slices);
      float nx = std::sin(phi) * std::cos(theta);
      float ny = std::cos(phi);
      float nz = std::sin(phi) * std::sin(theta);
      v.insert(v.end(), {radius * nx, radius * ny, radius * nz,  nx, ny, nz});
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
// Cylinder  (側面 + 上下キャップ、各部分に正確な法線)
// -----------------------------------------------------------------------
std::unique_ptr<Mesh> MeshFactory::cylinder(float radius, float height,
                                             int slices) {
  std::vector<float> v;
  std::vector<unsigned int> idx;
  float halfH = height * 0.5f;

  // -- 側面 --
  // 底リング (側面用)
  unsigned int sideBottomBase = 0;
  for (int i = 0; i < slices; i++) {
    float theta = 2.0f * kPi * i / float(slices);
    float cx = std::cos(theta), cz = std::sin(theta);
    v.insert(v.end(), {radius * cx, -halfH, radius * cz,  cx, 0.0f, cz});
  }
  // 上リング (側面用)
  unsigned int sideTopBase = slices;
  for (int i = 0; i < slices; i++) {
    float theta = 2.0f * kPi * i / float(slices);
    float cx = std::cos(theta), cz = std::sin(theta);
    v.insert(v.end(), {radius * cx,  halfH, radius * cz,  cx, 0.0f, cz});
  }
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    unsigned int b0 = sideBottomBase + i,    b1 = sideBottomBase + next;
    unsigned int t0 = sideTopBase   + i,    t1 = sideTopBase   + next;
    idx.push_back(b0); idx.push_back(b1); idx.push_back(t0);
    idx.push_back(t0); idx.push_back(b1); idx.push_back(t1);
  }

  // -- 底キャップ (法線 -Y) --
  unsigned int bottomRingBase = 2 * slices;
  for (int i = 0; i < slices; i++) {
    float theta = 2.0f * kPi * i / float(slices);
    v.insert(v.end(), {radius * std::cos(theta), -halfH, radius * std::sin(theta),
                       0.0f, -1.0f, 0.0f});
  }
  unsigned int bottomCenter = static_cast<unsigned int>(v.size() / 6);
  v.insert(v.end(), {0.0f, -halfH, 0.0f,  0.0f, -1.0f, 0.0f});
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    idx.push_back(bottomCenter);
    idx.push_back(bottomRingBase + next);
    idx.push_back(bottomRingBase + i);
  }

  // -- 上キャップ (法線 +Y) --
  unsigned int topRingBase = static_cast<unsigned int>(v.size() / 6);
  for (int i = 0; i < slices; i++) {
    float theta = 2.0f * kPi * i / float(slices);
    v.insert(v.end(), {radius * std::cos(theta),  halfH, radius * std::sin(theta),
                       0.0f, 1.0f, 0.0f});
  }
  unsigned int topCenter = static_cast<unsigned int>(v.size() / 6);
  v.insert(v.end(), {0.0f,  halfH, 0.0f,  0.0f, 1.0f, 0.0f});
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    idx.push_back(topCenter);
    idx.push_back(topRingBase + i);
    idx.push_back(topRingBase + next);
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

  // 側面法線: normalize(height * (cos θ, 0, sin θ) + (0, radius, 0))
  float L = std::sqrt(height * height + radius * radius);
  float ny_side = radius / L;
  float nxz_side = height / L;

  // -- 底リング (側面用) --
  for (int i = 0; i < slices; i++) {
    float theta = 2.0f * kPi * i / float(slices);
    float cx = std::cos(theta), cz = std::sin(theta);
    v.insert(v.end(), {radius * cx, -halfH, radius * cz,
                       nxz_side * cx, ny_side, nxz_side * cz});
  }

  // -- 頂点 (slices個; 各スライスに個別の法線) --
  unsigned int apexBase = slices;
  for (int i = 0; i < slices; i++) {
    // 隣接する2スライスの中間方向に頂点法線を置く (スムーズ近似)
    float theta = 2.0f * kPi * (i + 0.5f) / float(slices);
    v.insert(v.end(), {0.0f, halfH, 0.0f,
                       nxz_side * std::cos(theta), ny_side, nxz_side * std::sin(theta)});
  }

  // 側面
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    idx.push_back(static_cast<unsigned int>(i));
    idx.push_back(static_cast<unsigned int>(next));
    idx.push_back(apexBase + i);
  }

  // -- 底キャップ (法線 -Y) --
  unsigned int bottomRingBase = static_cast<unsigned int>(v.size() / 6);
  for (int i = 0; i < slices; i++) {
    float theta = 2.0f * kPi * i / float(slices);
    v.insert(v.end(), {radius * std::cos(theta), -halfH, radius * std::sin(theta),
                       0.0f, -1.0f, 0.0f});
  }
  unsigned int bottomCenter = static_cast<unsigned int>(v.size() / 6);
  v.insert(v.end(), {0.0f, -halfH, 0.0f,  0.0f, -1.0f, 0.0f});
  for (int i = 0; i < slices; i++) {
    int next = (i + 1) % slices;
    idx.push_back(bottomCenter);
    idx.push_back(bottomRingBase + next);
    idx.push_back(bottomRingBase + i);
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
  v.insert(v.end(), {0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f});

  // リング [1, segments]
  for (int i = 0; i < segments; i++) {
    float theta = 2.0f * kPi * i / float(segments);
    v.insert(v.end(), {radius * std::cos(theta), 0.0f, radius * std::sin(theta),
                       0.0f, 1.0f, 0.0f});
  }

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
  std::vector<float> v = {
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
       1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
       1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
      -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
  };
  std::vector<unsigned int> idx = {0, 1, 2, 2, 3, 0};

  return std::make_unique<Mesh>(v, idx);
}
