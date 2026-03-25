#pragma once

#include "../renderer/shader.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class ShaderManager {
public:
  // ソース文字列から直接ロード
  void load(const std::string &name, const char *vertSrc,
            const char *fragSrc) {
    shaders_[name] = std::make_unique<Shader>(vertSrc, fragSrc);
  }

  // ファイルパスからロード
  void loadFromFile(const std::string &name, const std::string &vertPath,
                    const std::string &fragPath) {
    load(name, readFile(vertPath).c_str(), readFile(fragPath).c_str());
  }

  Shader &get(const std::string &name) {
    auto it = shaders_.find(name);
    if (it == shaders_.end())
      throw std::runtime_error("Shader not found: " + name);
    return *it->second;
  }

private:
  std::unordered_map<std::string, std::unique_ptr<Shader>> shaders_;

  static std::string readFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open())
      throw std::runtime_error("Failed to open shader file: " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
  }
};
