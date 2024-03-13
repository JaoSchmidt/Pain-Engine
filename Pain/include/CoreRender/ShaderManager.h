#pragma once
#include "pch.gch"

#include "Shader.h"

namespace pain
{
class ShaderLibrary
{
public:
  static ShaderLibrary *getInstance();
  ShaderLibrary(ShaderLibrary &other) = delete;
  void operator=(const ShaderLibrary &) = delete;
  static ShaderLibrary *instance();

  void Add(const std::string &name, const std::shared_ptr<Shader> &shader);
  void Add(const std::shared_ptr<Shader> &shader);
  std::shared_ptr<Shader> Load(const char *filepath);
  std::shared_ptr<Shader> Load(const std::string &name, const char *filepath);

  std::shared_ptr<Shader> Get(const std::string &name);

  bool Exists(const std::string &name) const;

private:
  ShaderLibrary() {}
  static ShaderLibrary *m_instance;

  std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
};
} // namespace pain
