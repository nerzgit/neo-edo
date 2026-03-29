#include "obj_parser.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

ObjData parseObj(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("ObjParser: cannot open file: " + path);

  ObjData data;
  std::string line;

  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream ss(line);
    std::string token;
    ss >> token;
    if (token == "v") {
      parseVertex(ss, data);
    } else if (token == "vn") {
      parseNormal(ss, data);
    } else if (token == "f") {
      parseFace(ss, data);
    }
  }

  return data;
}

static void parseVertex(std::istringstream &ss, ObjData &data) {
  glm::vec3 p;
  ss >> p.x >> p.y >> p.z;
  data.positions.push_back(p);
}

static void parseNormal(std::istringstream &ss, ObjData &data) {
  glm::vec3 n;
  ss >> n.x >> n.y >> n.z;
  data.normals.push_back(n);
}

static void parseFace(std::istringstream &ss, ObjData &data) {
  std::vector<std::pair<int, int>> poly;
  std::string vtx;

  while (ss >> vtx) {
    int posIdx = 0, normIdx = 0;
    std::istringstream vs(vtx);
    std::string part;

    std::getline(vs, part, '/');
    posIdx = std::stoi(part);

    if (std::getline(vs, part, '/')) {
      if (std::getline(vs, part, '/') && !part.empty())
        normIdx = std::stoi(part);
    }

    poly.push_back({posIdx, normIdx});
  }

  for (size_t i = 1; i + 1 < poly.size(); i++) {
    data.faceVerts.push_back(poly[0]);
    data.faceVerts.push_back(poly[i]);
    data.faceVerts.push_back(poly[i + 1]);
  }
}