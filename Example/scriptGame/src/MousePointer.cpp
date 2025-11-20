#include "MousePointer.h"
#include "ECS/Scriptable.h"
#include "Misc/BasicOrthoCamera.h"
#include "Physics/Collision/GridManager.h"
#include <pain.h>

MousePointer::MousePointer(pain::Scene &scene) : NormalEntity(scene)
{
  createComponents(scene, pain::TransformComponent{},                       //
                   pain::SpriteComponent{"resources/textures/pointer.png"}, //
                   pain::NativeScriptComponent{});
}

MousePointerScript::MousePointerScript(reg::Entity entity, pain::Scene &scene,
                                       reg::Entity cameraEntity)

    : pain::ExtendedEntity(entity, scene), m_cameraEntity(cameraEntity) {};

void MousePointerScript::onEvent(const SDL_Event &event) {}

void MousePointerScript::onCreate()
{
  getComponent<pain::TransformComponent>().m_position = {0.f, 0.0f, 0.f};
}

glm::vec2 MousePointerScript::screenToWorld(int mouseX, int mouseY)
{
  // 1. Get the active OrthoCamera
  pain::OrthoCameraComponent &camCC =
      getScene().getComponent<pain::OrthoCameraComponent>(m_cameraEntity);
  pain::TransformComponent &camTC =
      getScene().getComponent<pain::TransformComponent>(m_cameraEntity);
  pain::RotationComponent &camRC =
      getScene().getComponent<pain::RotationComponent>(m_cameraEntity);

  // 2. Convert screen -> NDC space
  float ndcX = (2.0f * mouseX) / camCC.m_matrices->getResolution().x - 1.0f;
  float ndcY = 1.0f - (2.0f * mouseY) / camCC.m_matrices->getResolution().y;

  // 3. Convert NDC -> camera local coordinates
  float camLocalX = ndcX * (camCC.m_aspectRatio * camCC.m_zoomLevel);
  float camLocalY = ndcY * camCC.m_zoomLevel;

  // 4. Convert camera local -> world coordinates using rotation
  float angle = camRC.m_rotationAngle;
  glm::vec2 forward = glm::vec2(std::cos(angle), std::sin(angle));
  glm::vec2 right = glm::vec2(-std::sin(angle), std::cos(angle));

  return glm::vec2(camTC.m_position.x, camTC.m_position.y) + right * camLocalY +
         forward * camLocalX;
}
void MousePointerScript::onUpdate(double deltaTimeSec)
{
  int mx, my;
  SDL_GetMouseState(&mx, &my);

  auto &tc = getComponent<pain::TransformComponent>();
  glm::vec2 world = screenToWorld(mx, my);
  tc.m_position = glm::vec3(world, 0.f);
}
