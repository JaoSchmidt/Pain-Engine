
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Player3d.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "Misc/Events.h"
#include "Physics/Collision/Collider.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/Particles/SprayCmp.h"
#include "Scripting/LuaScriptComponent.h"
#include <pain.h>

reg::Entity Player3d::create(pain::Scene &scene, pain::Texture &tex,
                             glm::vec3 initialPos, glm::vec3 size,
                             int resolutionHeight, int resolutionWeigh,
                             float fieldOfViewDegrees)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(                             //
      entity, pain::Transform3dComponent{initialPos}, //
      pain::RotationComponent{},                      //
      pain::Movement3dComponent{},                    //
      Component::PerspCamera::create(resolutionWeigh, resolutionHeight,
                                     fieldOfViewDegrees,
                                     entity), //
      pain::NativeScriptComponent{}           //
  );
  pain::Scene::emplaceScript<pain::PerspCameraScript>(entity, scene);
  return entity;
}

void Player3d::Script::onRender(pain::Renderers &renderer, bool isMinimized,
                                pain::DeltaTime currentTime)
{
  auto &camCC = getComponent<Component::PerspCamera>();
  IMGUI_PLOG(
      [=]() { ImGui::Text("Camera fov: %.3f", camCC.m_fieldOfViewDegrees); });
}
//
// void Player3d::Script::onCreate()
// {
//
//   getEventDispatcher().subscribe<pain::CollisionEvent>(
//       [&](const pain::CollisionEvent &e) {
//         if (e.a == getEntity() || e.b == getEntity()) {
//           PLOG_I("Detected collision with entity {}",
//                  e.a == getEntity() ? e.b : e.a);
//         }
//       });
// }
