module;
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
module pain.ShaderManager;
import pain.Shader;

namespace pain
{

ShaderLibrary *ShaderLibrary::m_instance = nullptr;
ShaderLibrary *ShaderLibrary::getInstance()
{
  if (m_instance == nullptr) {
    m_instance = new ShaderLibrary();
  }
  return m_instance;
}

void ShaderLibrary::add(const std::string &name,
                        const std::shared_ptr<Shader> &shader)
{
  P_ASSERT(!exists(name), "Shader already exists!");
  m_Shaders[name] = shader;
}

void ShaderLibrary::add(const std::shared_ptr<Shader> &shader)
{
  auto &name = shader->getName();
  add(name, shader);
}

std::shared_ptr<pain::Shader> ShaderLibrary::load(const char *filepath)
{
  auto shader = std::make_shared<Shader>(Shader(filepath));
  add(shader);
  return shader;
}

std::shared_ptr<pain::Shader> ShaderLibrary::load(const std::string &name,
                                                  const char *filepath)
{
  auto shader = std::make_shared<Shader>(Shader(filepath));
  add(name, shader);
  return shader;
}

std::shared_ptr<pain::Shader> ShaderLibrary::get(const std::string &name)
{
  P_ASSERT(exists(name), "Shader not found!");
  return m_Shaders[name];
}

bool ShaderLibrary::exists(const std::string &name) const
{
  return m_Shaders.find(name) != m_Shaders.end();
}

} // namespace pain
