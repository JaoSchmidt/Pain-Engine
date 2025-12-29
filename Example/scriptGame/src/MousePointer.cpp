#include "MousePointer.h"
#include "ECS/Scriptable.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "Misc/BasicOrthoCamera.h"
#include "SDL_events.h"
#include "imgui.h"
#include <pain.h>

reg::Entity MousePointer::create(pain::Scene &scene)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform2dComponent{},                    //
      pain::SpriteComponent{"resources/textures/pointer.png"}, //
      pain::NativeScriptComponent{});
  return entity;
}

MousePointerScript::MousePointerScript(reg::Entity entity, pain::Scene &scene,
                                       reg::Entity cameraEntity)

    : pain::ExtendedEntity(entity, scene), m_cameraEntity(cameraEntity) {};

void MousePointerScript::onCreate()
{
  const pain::Component::OrthoCamera &camCC =
      getComponent<pain::Component::OrthoCamera>(m_cameraEntity);
  PLOG_I("Resolution x {}", camCC.getResolution().x);
  PLOG_I("Resolution y {}", camCC.getResolution().y);
  PLOG_I("Zoom Level {}", camCC.m_zoomLevel);
}

glm::vec2 MousePointerScript::screenToWorld(int mouseX, int mouseY)
{
  // 1. Get the active OrthoCamera
  // pain::Scene &s = std::get<std::reference_wrapper<pain::Scene>>(m_scene);
  auto [camCC, camTC, camRC] =
      getComponents<pain::Component::OrthoCamera, pain::Transform2dComponent,
                    pain::RotationComponent>(m_cameraEntity);

  // 2. Convert screen -> NDC space from -1 to 1
  float ndcX = (2.f * mouseX) / camCC.getResolution().x - 1.f;
  float ndcY = 1.f - (2.f * mouseY) / camCC.getResolution().y;

  // 3. Convert NDC -> camera local coordinates
  glm::vec2 localCoord = glm::vec2(
      ndcX * camCC.m_zoomLevel * camCC.m_aspectRatio, ndcY * camCC.m_zoomLevel);

  IMGUI_PLOG([=]() {
    ImGui::Text("Aspect Ratio (world)  %.3f", camCC.m_aspectRatio);
    ImGui::Text("Resolution (world) %dx%d ", TP_VEC2(camCC.getResolution()));
  });
  // 4. Convert camera local -> world coordinates using rotation
  float angle = glm::radians(camRC.m_rotationAngle);
  glm::mat2 rotation = glm::mat2(std::cos(angle), -std::sin(angle),
                                 std::sin(angle), std::cos(angle));
  return glm::vec2(camTC.m_position.x, camTC.m_position.y) +
         rotation * localCoord;
}
void MousePointerScript::onUpdate(pain::DeltaTime deltaTimeSec)
{
  int x, y;
  SDL_GetMouseState(&x, &y);
  glm::vec2 world = screenToWorld(x, y);
  IMGUI_PLOG_NAME("world_pos", [=]() {
    ImGui::Text("World position (%.3f, %.3f)", TP_VEC2(world));
  });

  getComponent<pain::Transform2dComponent>().m_position = glm::vec3(world, 0.f);
}

void MousePointerScript::onEvent(const SDL_Event &event)
{
  if (event.type == SDL_MOUSEMOTION) {
    glm::vec2 world = screenToWorld(event.motion.x, event.motion.y);
    IMGUI_PLOG_NAME("world_pos", [=]() {
      ImGui::Text("World position (%.3f, %.3f)", TP_VEC2(world));
    });
    getComponent<pain::Transform2dComponent>().m_position =
        glm::vec3(world, 0.f);
  } else if (event.type == SDL_MOUSEWHEEL) {
    glm::vec2 world = screenToWorld(event.wheel.mouseX, event.wheel.mouseY);
    IMGUI_PLOG_NAME("world_pos", [=]() {
      ImGui::Text("World position (%.3f, %.3f)", TP_VEC2(world));
    });
    getComponent<pain::Transform2dComponent>().m_position =
        glm::vec3(world, 0.f);
  }
}
