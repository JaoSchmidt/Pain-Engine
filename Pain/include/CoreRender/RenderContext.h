#pragma once
#include "Assets/ManagerMaterial.h"
#include "CoreRender/RenderCommand.h"

namespace pain
{

class RenderContext
{
public:
  static RenderContext create(MaterialManager &materialManager);
  void submit(pain::RenderCommand cmd)
  {
    m_commands.emplace_back(std::move(cmd));
  }
  void clear() { m_commands.clear(); }
  const std::vector<RenderCommand> &getCommands() const { return m_commands; };

  // void execute()
  // {
  //   for (auto &cmd : commands)
  //     cmd->execute(*this);
  //
  //   commands.clear();
  // }

  // Actual rendering functions
  void submitCube(const glm::vec3 &position, float size,
                  const Material &material, const glm::vec3 &rotation);
  void submitCube(const glm::vec3 &position, float size,
                  const Material &material);
  void submitUVSphere(const glm::vec3 &position, float size, SphereDivision div,
                      const Material &material);
  void submitUVSphere(const glm::vec3 &position, float size, SphereDivision div,
                      const Material &material, const glm::vec3 &rotation);
  void submitLight(const glm::vec3 &pos, const Color &color);

  glm::mat4 getUniformScaleTransform(const glm::vec3 &position, float size);
  glm::mat4 getUniformScaleTransform(const glm::vec3 &position, float size,
                                     const glm::vec3 &rotation);

  MaterialManager &m_materialManager;

private:
  RenderContext(MaterialManager &materialManager);
  std::vector<RenderCommand> m_commands = {};
};

} // namespace pain
