#pragma once
#include "pch.h"

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

  void add(const std::string &name, const std::shared_ptr<Shader> &shader);
  void add(const std::shared_ptr<Shader> &shader);
  std::shared_ptr<Shader> load(const char *filepath);
  std::shared_ptr<Shader> load(const std::string &name, const char *filepath);

  std::shared_ptr<Shader> get(const std::string &name);

  bool exists(const std::string &name) const;

private:
  ShaderLibrary() {}
  static ShaderLibrary *m_instance;

  std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
};
} // namespace pain
