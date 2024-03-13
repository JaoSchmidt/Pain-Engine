#include "CoreRender/ShaderManager.h"

#include "Core.h"
#include "CoreFiles/LogWrapper.h"

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

void ShaderLibrary::Add(const std::string &name,
                        const std::shared_ptr<Shader> &shader)
{
  P_ASSERT(!Exists(name), "Shader already exists!");
  m_Shaders[name] = shader;
}

void ShaderLibrary::Add(const std::shared_ptr<Shader> &shader)
{
  auto &name = shader->getName();
  Add(name, shader);
}

std::shared_ptr<pain::Shader> ShaderLibrary::Load(const char *filepath)
{
  auto shader = std::make_shared<Shader>(Shader(filepath));
  Add(shader);
  return shader;
}

std::shared_ptr<pain::Shader> ShaderLibrary::Load(const std::string &name,
                                                  const char *filepath)
{
  auto shader = std::make_shared<Shader>(Shader(filepath));
  Add(name, shader);
  return shader;
}

std::shared_ptr<pain::Shader> ShaderLibrary::Get(const std::string &name)
{
  P_ASSERT(Exists(name), "Shader not found!");
  return m_Shaders[name];
}

bool ShaderLibrary::Exists(const std::string &name) const
{
  return m_Shaders.find(name) != m_Shaders.end();
}

} // namespace pain
