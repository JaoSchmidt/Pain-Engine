/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Scripting/Lua/WorldSceneBind.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/MaterialComponent.h"
#include "CoreRender/SpriteComponent.h"
#include "ECS/WorldScene.h"
#include "Physics/Collision/Collider.h"
#include "Physics/Collision/SweepAndPruneSys.h"
#include "Physics/RotationComponent.h"
#include "Scripting/Lua/LuaScriptComponent.h"

namespace pain
{
struct LuaComponentDesc {
  reg::Bitmask bit;
  std::function<void(reg::Entity, reg::Bitmask)> emplace;
  std::function<void(reg::Entity)> onEmplace = nullptr;
};

template <typename Component>
void onComponentAdded(Scene &scene, reg::Entity entity)
{
  if constexpr (std::is_same_v<Component, SAPCollider>) {
    Systems::SweepAndPruneSys *s =
        scene.template getSys<Systems::SweepAndPruneSys>();
    if (s) {
      s->insertCollider(entity);
    } else {
      PLOG_W(
          "You are trying to create Sweep and Prune component without adding "
          "a proper Sweep and Prune system");
    }
  }
}

inline std::variant<QuadShape, RectShape, TriangleShape>
parseSpriteShape(sol::object obj)
{
  if (!obj.valid() || obj.get_type() != sol::type::table)
    return QuadShape{}; // default

  sol::table t = obj.as<sol::table>();

  // type field (required for table-based shapes)
  const std::string type = t["type"].get_or(std::string("quad"));

  if (type == "quad") {
    float side = t["side"].get_or(0.125f);
    return QuadShape{side};
  } else if (type == "rect") {
    glm::vec2 size{0.125f, 0.25f};
    if (auto v = t["size"]; v.valid())
      size = v.get<glm::vec2>();
    return RectShape{size};
  } else if (type == "triangle") {
    float base = t["base"].get_or(0.125f);
    float height = t["height"].get_or(0.125f);
    return TriangleShape{base, height};
  }

  // fallback
  return QuadShape{};
}

void luabinder::bindWorldComponents(Scene &scene, sol::state &lua,
                                    MaterialManager &mm,
                                    const AppInit &initConfig)
{
  sol::table worldTbl = lua.create_table();
  // ------------------------------------------------------------
  //  Material Component bind
  // ------------------------------------------------------------
  if constexpr (WorldComponents::isRegistered<MaterialComponent>()) {
    worldTbl.set_function(
        "Material",
        sol::overload(

            // ----------------------------------------
            // Default material
            // ----------------------------------------
            [&](void) {
              return LuaComponentDesc{
                  scene.getSingleBitmask<MaterialComponent>(),
                  [&scene, &mm](reg::Entity e, reg::Bitmask b) {
                    scene.manualPush(e, b, MaterialComponent::create(mm));
                  }};
            },

            // ----------------------------------------
            // From Material object
            // ----------------------------------------
            [&](Material &mat) {
              return LuaComponentDesc{
                  scene.getSingleBitmask<MaterialComponent>(),
                  [=, &mat, &scene](reg::Entity e, reg::Bitmask b) {
                    scene.manualPush(e, b, MaterialComponent::create(mat));
                  }};
            }));
  }

  // ------------------------------------------------------------
  //  Sprite Component bind
  // ------------------------------------------------------------
  if constexpr (WorldComponents::isRegistered<SpriteComponent>())
    worldTbl["Sprite"] = sol::overload(
        [&](sol::optional<SpriteCreationInfo> oInfo) {
          SpriteCreationInfo info = oInfo.value_or(SpriteCreationInfo{});
          return LuaComponentDesc{
              scene.getSingleBitmask<SpriteComponent>(),
              [info = std::move(info), &scene](reg::Entity e, reg::Bitmask b) {
                scene.manualPush(e, b, SpriteComponent::create(info));
              }};
        },
        // allow table-style construction
        [&](sol::optional<sol::table> oInfoTable) {
          pain::SpriteCreationInfo info{};
          if (oInfoTable) {
            sol::table t = *oInfoTable;
            if (auto layer = t["layer"]; layer.valid())
              info.layer = layer.get<pain::RenderLayer>();
            if (auto shape = t["shape"]; shape.valid())
              info.shape = parseSpriteShape(shape);
          }
          return LuaComponentDesc{
              scene.getSingleBitmask<pain::SpriteComponent>(),
              [info = std::move(info), &scene](reg::Entity e, reg::Bitmask b) {
                scene.manualPush(e, b, pain::SpriteComponent::create(info));
              }};
        });

  // ------------------------------------------------------------
  //  Movement2d Component bind
  // ------------------------------------------------------------
  if constexpr (WorldComponents::isRegistered<Movement2dComponent>())
    worldTbl["Movement2d"] = [&](sol::optional<glm::vec2> oVel,
                                 sol::optional<float> oRotationSpeed) {
      Movement2dCreationInfo comp{};
      if (oVel)
        comp.velocity = *oVel;
      if (oRotationSpeed)
        comp.rotationSpeed = *oRotationSpeed;

      return LuaComponentDesc{
          scene.getSingleBitmask<Movement2dComponent>(),
          [comp = std::move(comp), &scene](reg::Entity e, reg::Bitmask b) {
            scene.manualPush(e, b, Movement2dComponent::create(comp));
          }};
    };
  // ------------------------------------------------------------
  //  Rotation Component bind
  // ------------------------------------------------------------
  if constexpr (WorldComponents::isRegistered<RotationComponent>())
    worldTbl["Rotation"] = sol::overload(
        [&](sol::optional<float> oInitialAngle,
            sol::optional<glm::vec3> oRotation) { //
          RotationCreationInfo info;
          if (oRotation)
            info.rotation = *oRotation;
          if (oInitialAngle)
            info.rotationAngle = *oInitialAngle;
          return LuaComponentDesc{
              scene.getSingleBitmask<RotationComponent>(),
              [info = std::move(info), &scene](reg::Entity e, reg::Bitmask b) {
                scene.manualPush(e, b, RotationComponent::create(info));
              } //
          };
        },
        [&](sol::optional<sol::table> oInfoTable) {
          RotationCreationInfo info;
          if (oInfoTable) {
            sol::table t = *oInfoTable;
            if (auto angle = t["angle"]; angle.valid())
              info.rotationAngle = angle.get<float>();
            if (auto rotation = t["rotation"]; rotation.valid())
              info.rotation = rotation.get<glm::vec3>();
          }
          return LuaComponentDesc{
              scene.getSingleBitmask<RotationComponent>(),
              [info = std::move(info), &scene](reg::Entity e, reg::Bitmask b) {
                scene.manualPush(e, b, RotationComponent::create(info));
              } //
          };
        });

  // ------------------------------------------------------------
  //  Transform2d Component bind
  // ------------------------------------------------------------
  if constexpr (WorldComponents::isRegistered<Transform2dComponent>())
    worldTbl["Transform2d"] = [&](sol::optional<glm::vec2> oPos) {
      Transform2dCreationInfo info{};
      if (oPos)
        info.position = *oPos;
      return LuaComponentDesc{
          scene.getSingleBitmask<Transform2dComponent>(),
          [info = std::move(info), &scene](reg::Entity e, reg::Bitmask b) {
            scene.manualPush(e, b, Transform2dComponent::create(info));
          }};
    };
  // ------------------------------------------------------------
  //  Sweep and Prune Component bind
  // ------------------------------------------------------------
  if constexpr (WorldComponents::isRegistered<SAPCollider>())
    worldTbl.set_function(
        "SAPCollider",
        sol::overload(
            [&](glm::vec2 size, sol::optional<bool> oTrigger,
                sol::optional<glm::vec2> oOffset) {
              bool isTrigger = oTrigger.value_or(false);
              glm::vec2 offset = oOffset.value_or(glm::vec2{0.f, 0.f});

              return LuaComponentDesc{
                  scene.getSingleBitmask<SAPCollider>(),
                  [=, &scene](reg::Entity e, reg::Bitmask b) {
                    scene.manualPush(
                        e, b, SAPCollider::createAABB(size, isTrigger, offset));
                  },
                  [&scene](reg::Entity e) {
                    onComponentAdded<SAPCollider>(*&scene, e);
                  }};
            },
            [&](float radius, sol::optional<bool> oTrigger,
                sol::optional<glm::vec2> oOffset) {
              bool isTrigger = oTrigger.value_or(false);
              glm::vec2 offset = oOffset.value_or(glm::vec2{0.f, 0.f});

              return LuaComponentDesc{
                  scene.getSingleBitmask<SAPCollider>(),
                  [=, &scene](reg::Entity e, reg::Bitmask b) {
                    scene.manualPush(
                        e, b,
                        SAPCollider::createCircle(radius, isTrigger, offset));
                  },
                  [&scene](reg::Entity e) {
                    onComponentAdded<SAPCollider>(*&scene, e);
                  }};
            }));
  if constexpr (WorldComponents::isRegistered<::cmp::OrthoCamera>())
    worldTbl["OrthoCamera"] = sol::overload(
        [&](sol::optional<bool> oActive, sol::optional<int> oWidth,
            sol::optional<int> oHeight, sol::optional<float> oZoom) {
          bool active = oActive.value_or(true);
          int width = oWidth.value_or(initConfig.defaultWidth);
          int height = oHeight.value_or(initConfig.defaultHeight);
          float zoom = oZoom.value_or(initConfig.defaultZoom2d);

          return LuaComponentDesc{
              scene.getSingleBitmask<::cmp::OrthoCamera>(),
              [=, &scene](reg::Entity e, reg::Bitmask b) {
                scene.manualPush(
                    e, b,
                    ::cmp::OrthoCamera::create(active, width, height, zoom, e));
              }};
        },
        [&](sol::optional<sol::table> oTbl) {
          bool active = true;
          int width = initConfig.defaultWidth;
          int height = initConfig.defaultHeight;
          float zoom = initConfig.defaultZoom2d;

          if (oTbl) {
            sol::table t = *oTbl;

            if (auto v = t["active"]; v.valid())
              active = v.get<bool>();

            if (auto v = t["width"]; v.valid())
              width = v.get<int>();

            if (auto v = t["height"]; v.valid())
              height = v.get<int>();

            if (auto v = t["zoom"]; v.valid())
              zoom = v.get<float>();
          }

          return LuaComponentDesc{
              scene.getSingleBitmask<::cmp::OrthoCamera>(),
              [=, &scene](reg::Entity e, reg::Bitmask b) {
                scene.manualPush(
                    e, b,
                    ::cmp::OrthoCamera::create(active, width, height, zoom, e));
              }};
        } //
    );

  // ------------------------------------------------------------
  //  Transform2d Component bind
  // ------------------------------------------------------------
  if constexpr (WorldComponents::isRegistered<LuaScriptComponent>()) {
    worldTbl["LuaScript"] = [&]() {
      return LuaComponentDesc{
          scene.getSingleBitmask<LuaScriptComponent>(),
          [=, &scene](reg::Entity e, reg::Bitmask b) {
            scene.manualPush(e, b, LuaScriptComponent{e});
          } //
      };
    };
    worldTbl["emplace_lua_script"] = sol::overload(
        [&scene](reg::Entity entity, const std::string &scriptPath,
                 const sol::table &initArgs) {
          Scene::emplaceLuaScript(entity, scene, scriptPath.c_str(), initArgs);
        },
        [&scene](reg::Entity entity, const std::string &scriptPath) {
          Scene::emplaceLuaScript(entity, scene, scriptPath.c_str());
        });
  }

  worldTbl["create_entity"] = [&](sol::table components) {
    reg::Bitmask archetype{};

    for (const auto &kv : components) {
      const auto &d = kv.second.as<LuaComponentDesc>();
      archetype |= d.bit;
    }

    reg::Entity e = scene.manualEntityCreation(archetype);

    for (auto &kv : components) {
      auto &d = kv.second.as<LuaComponentDesc>();
      d.emplace(e, archetype);
    }
    for (auto &kv : components) {
      auto &d = kv.second.as<LuaComponentDesc>();
      if (d.onEmplace)
        d.onEmplace(e);
    }
    return e;
  };

  worldTbl["get_lua_script"] = [&](reg::Entity e) -> sol::table {
    if (scene.hasAnyComponents<LuaScriptComponent>(e))
      return scene.getComponent<LuaScriptComponent>(e).m_scriptTable;
    return sol::nil;
  };

  // =========== Self reference =================================
  worldTbl["get_2d_position"] = sol::overload(
      [&](reg::Entity e) -> sol::object {
        if (scene.hasAnyComponents<pain::Transform2dComponent>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<pain::Transform2dComponent>(e)));
        return sol::nil;
      },
      [&](sol::table self) -> sol::object {
        reg::Entity e = self["entity"];
        if (scene.hasAnyComponents<pain::Transform2dComponent>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<pain::Transform2dComponent>(e)));
        return sol::nil;
      });
  worldTbl["get_sprite"] = sol::overload(
      [&](reg::Entity e) -> sol::object {
        if (scene.hasAnyComponents<SpriteComponent>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<SpriteComponent>(e)));
        return sol::nil;
      },
      [&](sol::table self) -> sol::object {
        reg::Entity e = self["entity"];
        if (scene.hasAnyComponents<SpriteComponent>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<SpriteComponent>(e)));
        return sol::nil;
      });
  worldTbl["get_2d_movement"] = sol::overload(
      [&](reg::Entity e) -> sol::object {
        if (scene.hasAnyComponents<Movement2dComponent>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<Movement2dComponent>(e)));
        return sol::nil;
      },
      [&](sol::table self) -> sol::object {
        reg::Entity e = self["entity"];
        if (scene.hasAnyComponents<Movement2dComponent>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<Movement2dComponent>(e)));
        return sol::nil;
      });
  worldTbl["get_rotation"] = sol::overload(
      [&](reg::Entity e) -> sol::object {
        if (scene.hasAnyComponents<RotationComponent>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<RotationComponent>(e)));
        return sol::nil;
      },
      [&](sol::table self) -> sol::object {
        reg::Entity e = self["entity"];
        if (scene.hasAnyComponents<RotationComponent>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<RotationComponent>(e)));
        return sol::nil;
      });
  worldTbl["get_ortho_camera"] = sol::overload(
      [&](reg::Entity e) -> sol::object {
        if (scene.hasAnyComponents<cmp::OrthoCamera>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<cmp::OrthoCamera>(e)));
        return sol::nil;
      },
      [&](sol::table self) -> sol::object {
        reg::Entity e = self["entity"];
        if (scene.hasAnyComponents<cmp::OrthoCamera>(e))
          return sol::make_reference(
              lua, std::ref(scene.getComponent<cmp::OrthoCamera>(e)));
        return sol::nil;
      });
  lua["World"] = worldTbl;
}

void luabinder::printState(sol::state &lua)
{
  lua_State *L = lua.lua_state();
  printf("Lua state ptr: %p\n", (void *)L);
}

} // namespace pain
