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

  // ================================================================= //
  // Draw 3d
  // ================================================================= //

  /// @bried Submit a cube polyhedron.
  void submitCube(const glm::vec3 &position, float size,
                  const Material &material, const glm::vec3 &rotation);
  /// @bried Submit a rotated cube polyhedron. Rotation dimensions are in
  /// radians
  void submitCube(const glm::vec3 &position, float size,
                  const Material &material);
  /// @bried Submit a UV sphere polyhedron.
  void submitUVSphere(const glm::vec3 &position, float size, SphereDivision div,
                      const Material &material);
  /// @bried Submit a rotating UV sphere polyhedron. Rotation dimensions are in
  /// radians
  void submitUVSphere(const glm::vec3 &position, float size, SphereDivision div,
                      const Material &material, const glm::vec3 &rotation);
  /// @brief a light position to be used by other shaders
  void submitLight(const glm::vec3 &pos, const Color &color);
  // ================================================================= //
  // Draw 2d
  // ================================================================= //

  /// @bried Submit an axis-aligned textured quad.
  void submitRect(const glm::vec2 &position, const glm::vec2 &size,
                  RenderLayer layer, const Material &material);
  /** @bried Submit a rotated textured quad.
   * @param rotationRadians Rotation angle in radians. */
  void submitRect(const glm::vec2 &position, const glm::vec2 &size,
                  const float rotationRadians, RenderLayer layer,
                  const Material &material);
  /// @bried Submit an axis-aligned textured quad.
  void submitQuad(const glm::vec2 &position, float size, RenderLayer layer,
                  const Material &material);
  /** @bried Submit a rotated textured quad.
   * @param rotationRadians Rotation angle in radians. */
  void submitQuad(const glm::vec2 &position, float size,
                  const float rotationRadians, RenderLayer layer,
                  const Material &material);
  /// @bried Submit a colored triangle primitive.
  void submitTri(const glm::vec2 &position, float size, RenderLayer layer,
                 const Material &material);
  /// @bried Submit a rotated triangle primitive.
  void submitTri(const glm::vec2 &position, float size,
                 const float rotationRadians, RenderLayer layer,
                 const Material &material);

  MaterialManager &m_materialManager;

private:
  glm::mat4 getUniformScaleTransform(const glm::vec2 &position, float size);
  glm::mat4 getUniformScaleTransform(const glm::vec2 &position, float size,
                                     float rotation);
  glm::mat4 getUniformScaleTransform(const glm::vec3 &position, float size);
  glm::mat4 getUniformScaleTransform(const glm::vec3 &position, float size,
                                     const glm::vec3 &rotation);
  glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size,
                         float rotationAngleRadians);
  glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size);
  RenderContext(MaterialManager &materialManager);
  std::vector<RenderCommand> m_commands = {};
};

} // namespace pain
