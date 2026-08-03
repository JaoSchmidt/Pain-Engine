/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <Misc/TextureSheet.h>
#include <imgui.h>
#include <pain.h>

namespace painless
{

// ============================================================ //
// Component name lookup by bit index (matches WorldComponents order)
// ============================================================ //

inline const char *getWorldComponentName(int bitIndex)
{
  static constexpr const char *names[] = {
      "OrthoCamera",   // 0
      "PerspCamera",   // 1
      "Transform2d",   // 2
      "Movement2d",    // 3
      "Transform3d",   // 4
      "Movement3d",    // 5
      "NativeScript",  // 6
      "ParticleSpray", // 7
      "Rotation",      // 8
      "Sprite",        // 9
      "LuaScript",     // 10
      "SAPCollider",   // 11
      "LuaScheduler",  // 12
      "Mesh",          // 13
      "Material",      // 14
      "Light",         // 15
      "ParticleTrail", // 16
      "TextComponent", // 17
      "ColorIndex"     // 18
  };
  if (bitIndex >= 0 && bitIndex < 19)
    return names[bitIndex];
  return "Unknown";
}

inline const char *getMeshShapeName(pain::MeshShape shape)
{
  switch (shape) {
  case pain::Shpere_8x8:
    return "Sphere 8x8";
  case pain::Shpere_16x16:
    return "Sphere 16x16";
  case pain::Shpere_32x32:
    return "Sphere 32x32";
  case pain::Cube:
    return "Cube";
  default:
    return "Unknown";
  }
}

inline const char *getLightTypeName(pain::LightType type)
{
  switch (type) {
  case pain::LightType::Directional:
    return "Directional";
  case pain::LightType::Point:
    return "Point";
  case pain::LightType::Spot:
    return "Spot";
  default:
    return "Unknown";
  }
}

inline const char *getRenderLayerName(pain::RenderLayer layer)
{
  switch (layer) {
  case pain::RenderLayer::A:
    return "A (Very distant)";
  case pain::RenderLayer::B:
    return "B (Distant)";
  case pain::RenderLayer::C:
    return "C (Background)";
  case pain::RenderLayer::D:
    return "D (Default)";
  case pain::RenderLayer::E:
    return "E (Foreground)";
  case pain::RenderLayer::F:
    return "F (Close)";
  case pain::RenderLayer::G:
    return "G (Very close)";
  default:
    return "Unknown";
  }
}

// ============================================================ //
// Icon rendering from a TextureSheet
// ============================================================ //

inline void renderComponentIcon(pain::TextureSheet &sheet, int bitIndex,
                                float size = 16.f)
{
  const auto &uvs = sheet[static_cast<unsigned short>(bitIndex)];
  // TextureSheet UVs: [0]=bottom-left, [1]=bottom-right,
  //                   [2]=top-right,    [3]=top-left
  // ImGui::Image wants: uv0=top-left, uv1=bottom-right
  ImVec2 uv0(uvs[3].x, uvs[3].y);
  ImVec2 uv1(uvs[1].x, uvs[1].y);
  uint32_t texID = sheet.getTexture().getRendererId();
  ImGui::Image((ImTextureID)(uintptr_t)texID, ImVec2(size, size), uv0, uv1);
}

// ============================================================ //
// Per-component ImGui preview renderers
// ============================================================ //

template <typename Component> struct ComponentPreview {
  static void render(const Component &) { ImGui::TextDisabled("(no preview)"); }
};

template <> struct ComponentPreview<pain::Transform2dComponent> {
  static void render(const pain::Transform2dComponent &c)
  {
    ImGui::Text("Position: (%.2f, %.2f)", c.m_position.x, c.m_position.y);
  }
};

template <> struct ComponentPreview<pain::Movement2dComponent> {
  static void render(const pain::Movement2dComponent &c)
  {
    ImGui::Text("Velocity: (%.2f, %.2f)", c.m_velocity.x, c.m_velocity.y);
    ImGui::Text("Rot Speed: %.2f", c.m_rotationSpeed);
  }
};

template <> struct ComponentPreview<pain::Transform3dComponent> {
  static void render(const pain::Transform3dComponent &c)
  {
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", c.m_position.x, c.m_position.y,
                c.m_position.z);
  }
};

template <> struct ComponentPreview<pain::Movement3dComponent> {
  static void render(const pain::Movement3dComponent &c)
  {
    ImGui::Text("Velocity: (%.2f, %.2f, %.2f)", c.m_velocity.x, c.m_velocity.y,
                c.m_velocity.z);
    ImGui::Text("Rot Speed: %.2f", c.m_rotationSpeed);
  }
};

template <> struct ComponentPreview<pain::OrthoCameraComponent> {
  static void render(const pain::OrthoCameraComponent &c)
  {
    ImGui::Text("Active: %s", c.m_active ? "yes" : "no");
    ImGui::Text("Zoom: %.2f", c.m_zoomLevel);
    ImGui::Text("Resolution: %dx%d", c.m_resolution.x, c.m_resolution.y);
  }
};

template <> struct ComponentPreview<pain::PerspCameraComponent> {
  static void render(const pain::PerspCameraComponent &c)
  {
    ImGui::Text("Active: %s", c.m_active ? "yes" : "no");
    ImGui::Text("FOV: %.1f deg", c.m_fieldOfViewDegrees);
    ImGui::Text("Resolution: %dx%d", c.m_resolution.x, c.m_resolution.y);
  }
};

template <> struct ComponentPreview<pain::SpriteComponent> {
  static void render(const pain::SpriteComponent &c)
  {
    ImGui::Text("Layer: %s", getRenderLayerName(c.layer));
    if (std::holds_alternative<pain::QuadShape>(c.m_shape)) {
      ImGui::Text("Shape: Quad (side=%.3f)",
                  std::get<pain::QuadShape>(c.m_shape).side);
    } else if (std::holds_alternative<pain::RectShape>(c.m_shape)) {
      auto &r = std::get<pain::RectShape>(c.m_shape);
      ImGui::Text("Shape: Rect (%.3f x %.3f)", r.size.x, r.size.y);
    } else if (std::holds_alternative<pain::TriangleShape>(c.m_shape)) {
      auto &t = std::get<pain::TriangleShape>(c.m_shape);
      ImGui::Text("Shape: Triangle (base=%.3f, h=%.3f)", t.base, t.height);
    } else if (std::holds_alternative<pain::LineShape>(c.m_shape)) {
      auto &l = std::get<pain::LineShape>(c.m_shape);
      ImGui::Text("Shape: Line (thick=%.1f)", l.thickness);
    }
  }
};

template <> struct ComponentPreview<pain::MeshComponent> {
  static void render(const pain::MeshComponent &c)
  {
    ImGui::Text("Shape: %s", getMeshShapeName(c.shape));
    ImGui::Text("Size: %.3f", c.size);
  }
};

template <> struct ComponentPreview<pain::MaterialComponent> {
  static void render(const pain::MaterialComponent &c)
  {
    if (c.m_material) {
      ImGui::Text("Material: %p", (void *)c.m_material);
      ImGui::Text("Name: %s", c.m_material->m_name.c_str());
      ImGui::Text("Shader: %p", (void *)c.m_material->m_shader);
      glm::vec4 rgba = c.m_material->m_color.getVector();
      ImGui::ColorEdit4("Color", &rgba.x,
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoLabel);
    } else {
      ImGui::TextDisabled("Material: null");
    }
  }
};

template <> struct ComponentPreview<pain::LightComponent> {
  static void render(const pain::LightComponent &c)
  {
    ImGui::Text("Type: %s", getLightTypeName(c.m_type));
    glm::vec4 rgba = c.m_color.getVector();
    ImGui::ColorEdit4("Color", &rgba.x,
                      ImGuiColorEditFlags_NoInputs |
                          ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();
    ImGui::Text("Color");
    ImGui::Text("Intensity: %.2f", c.m_intensity);
    ImGui::Text("Range: %.2f", c.m_range);
  }
};

template <> struct ComponentPreview<pain::RotationComponent> {
  static void render(const pain::RotationComponent &c)
  {
    ImGui::Text("Rotation: %.2f rad", c.m_rotationRadians);
    ImGui::Text("Axis: (%.2f, %.2f, %.2f)", c.m_rotation.x, c.m_rotation.y,
                c.m_rotation.z);
  }
};

template <> struct ComponentPreview<pain::NativeScriptComponent> {
  static void render(const pain::NativeScriptComponent &c)
  {
    c.instance ? ImGui::Text("Instance: bound")
               : ImGui::TextDisabled("Instance: empty");
    c.onCreateFunction ? ImGui::Text("onCreate: bound")
                       : ImGui::TextDisabled("onCreate: empty");
    c.onUpdateFunction ? ImGui::Text("onUpdate: bound")
                       : ImGui::TextDisabled("onUpdate: empty");
    c.onRenderFunction ? ImGui::Text("onRender: bound")
                       : ImGui::TextDisabled("onRender: empty");
    c.onEventFunction ? ImGui::Text("onEvent: bound")
                      : ImGui::TextDisabled("onEvent: empty");
    c.onDestroyFunction ? ImGui::Text("onDestroy: bound")
                        : ImGui::TextDisabled("onDestroy: empty");
  }
};

template <> struct ComponentPreview<pain::LuaScriptComponent> {
  static void render(const pain::LuaScriptComponent &c)
  {
    ImGui::Text("Script: %s", c.m_scriptPath.c_str());
    ImGui::Text("Entity: %d", static_cast<int>(c.m_entity));
  }
};

template <> struct ComponentPreview<pain::SAPCollider> {
  static void render(const pain::SAPCollider &c)
  {
    ImGui::Text("Offset: (%.2f, %.2f)", c.m_offset.x, c.m_offset.y);
    ImGui::Text("Trigger: %s", c.m_isTrigger ? "yes" : "no");
    if (std::holds_alternative<pain::CircleShape>(c.m_shape)) {
      ImGui::Text("Shape: Circle (r=%.2f)",
                  std::get<pain::CircleShape>(c.m_shape).radius);
    } else if (std::holds_alternative<pain::AABBShape>(c.m_shape)) {
      auto &aabb = std::get<pain::AABBShape>(c.m_shape);
      ImGui::Text("Shape: AABB (%.2f x %.2f)", aabb.halfSize.x,
                  aabb.halfSize.y);
    } else if (std::holds_alternative<pain::CapsuleShape>(c.m_shape)) {
      ImGui::Text("Shape: Capsule");
    }
  }
};

template <> struct ComponentPreview<pain::LuaSchedulerComponent> {
  static void render(const pain::LuaSchedulerComponent &c)
  {
    ImGui::Text("Interval: %.2f", c.interval);
    ImGui::Text("Elapsed: %.2f", c.elapsed);
  }
};

template <> struct ComponentPreview<pain::ParticleSprayComponent> {
  static void render(const pain::ParticleSprayComponent &c)
  {
    ImGui::Text("Velocity: %.2f", c.velocity);
    ImGui::Text("Auto Emit: %s", c.autoEmit ? "yes" : "no");
    ImGui::Text("Max Particles: %u", c.maxNumberOfParticles);
    ImGui::Text(
        "Alive: %zu",
        std::count_if(c.particles.begin(), c.particles.end(),
                      [](const pain::SprayParticle &p) { return p.alive; }));
  }
};

template <> struct ComponentPreview<pain::TrailComponent> {
  static void render(const pain::TrailComponent &c)
  {
    ImGui::Text("Capacity: %u", c.capacity);
    ImGui::Text("Points: %zu", c.coordinates.size());
    ImGui::Text("Auto Emit: %s", c.autoEmit ? "yes" : "no");
  }
};

template <> struct ComponentPreview<pain::TextComponent> {
  static void render(const pain::TextComponent &c)
  {
    ImGui::Text("Text: %s", c.text.c_str());
    ImGui::Text("Scale: %.1f", c.scale);
    glm::vec4 rgba = c.color.getVector();
    ImGui::ColorEdit4("Color", &rgba.x,
                      ImGuiColorEditFlags_NoInputs |
                          ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();
    ImGui::Text("Color");
  }
};

template <> struct ComponentPreview<pain::ColorIndexComponent> {
  static void render(const pain::ColorIndexComponent &c)
  {
    glm::vec4 rgba = c.color.getVector();
    ImGui::ColorEdit4("Color", &rgba.x,
                      ImGuiColorEditFlags_NoInputs |
                          ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();
    ImGui::Text("Color");
  }
};

// ============================================================ //
// EntityInspector
// ============================================================ //

struct EntityInspector {
  template <reg::CompileTimeBitMaskType Manager>
  static void render(pain::AbstractScene<Manager> &scene,
                     pain::TextureSheet *icons = nullptr)
  {
#ifndef NDEBUG
    const std::vector<reg::Record> &records = scene.getRegistry().getRecords();
    const std::unordered_map<reg::Entity, std::string> &names =
        scene.getEntityNames();

    // -- group alive entities by name --------------------------------
    struct NamedGroup {
      std::string name;
      std::vector<int> entities;
    };
    std::vector<NamedGroup> namedGroups;
    std::vector<int> unnamedEntities;

    for (int i = 0; i < static_cast<int>(records.size()); ++i) {
      if (records[i].bitmask == reg::Bitmask{-1})
        continue;

      reg::Entity entity{i};
      auto nameIt = names.find(entity);
      if (nameIt != names.end()) {
        // find existing group with same name
        auto g = std::find_if(
            namedGroups.begin(), namedGroups.end(),
            [&](const NamedGroup &ng) { return ng.name == nameIt->second; });
        if (g != namedGroups.end()) {
          g->entities.push_back(i);
        } else {
          namedGroups.push_back({nameIt->second, {i}});
        }
      } else {
        unnamedEntities.push_back(i);
      }
    }

    // count alive
    int aliveCount = 0;
    for (auto &ng : namedGroups)
      aliveCount += static_cast<int>(ng.entities.size());
    aliveCount += static_cast<int>(unnamedEntities.size());

    ImGui::Text("Alive Entities: %d / %d", aliveCount, (int)records.size());
    ImGui::Separator();

    // -- render named groups -----------------------------------------
    for (int g = 0; g < static_cast<int>(namedGroups.size()); ++g) {
      auto &ng = namedGroups[g];
      ImGui::PushID(g);

      bool multi = ng.entities.size() > 1;
      if (multi)
        snprintf(m_labelBuf, sizeof(m_labelBuf), "%s (%zu)##group",
                 ng.name.c_str(), ng.entities.size());
      else
        snprintf(m_labelBuf, sizeof(m_labelBuf), "%s [id = %d]##single",
                 ng.name.c_str(), ng.entities[0]);

      if (ImGui::TreeNode(m_labelBuf)) {
        // if multi, each entity is a sub-node; if single, show directly
        const std::vector<int> &toRender =
            multi ? ng.entities : std::vector<int>{ng.entities[0]};
        for (int idx = 0; idx < static_cast<int>(toRender.size()); ++idx) {
          int ei = toRender[idx];
          if (multi) {
            ImGui::PushID(ei);
            snprintf(m_labelBuf, sizeof(m_labelBuf), "Entity %d##sub", ei);
            if (ImGui::TreeNode(m_labelBuf)) {
              renderEntityNode<Manager>(scene, ei, names, icons);
              ImGui::TreePop();
            }
            ImGui::PopID();
          } else {
            renderEntityNode<Manager>(scene, ei, names, icons);
          }
        }
        ImGui::TreePop();
      }

      ImGui::PopID();
    }

    // -- render unnamed entities -------------------------------------
    for (int idx = 0; idx < static_cast<int>(unnamedEntities.size()); ++idx) {
      int ei = unnamedEntities[idx];
      ImGui::PushID(ei + 100000);
      renderEntityNode<Manager>(scene, ei, names, icons);
      ImGui::PopID();
    }
#endif
  }

private:
  inline static char m_labelBuf[128];

  template <reg::CompileTimeBitMaskType Manager>
  static void
  renderEntityNode(pain::AbstractScene<Manager> &scene, int i,
                   const std::unordered_map<reg::Entity, std::string> &names,
                   pain::TextureSheet *icons)
  {
    const auto &records = scene.getRegistry().getRecords();
    auto record = records[i];
    reg::Entity entity{i};

    auto nameIt = names.find(entity);
    bool hasName = (nameIt != names.end());

    if (hasName)
      ImGui::Text("Name: %s", nameIt->second.c_str());
    ImGui::Text("ID: %d", i);
    ImGui::Text("Bitmask: %d", record.bitmask.value);

    ImGui::Separator();

    constexpr int totalBits =
        static_cast<int>(Manager::getNumberOfRegisteredComponents());

    for (int bit = 0; bit < totalBits; ++bit) {
      if (!(record.bitmask.value & (1 << bit)))
        continue;

      const char *compName = getWorldComponentName(bit);
      ImGui::PushID(bit);

      if (icons) {
        renderComponentIcon(*icons, bit);
        ImGui::SameLine();
      }

      if (ImGui::TreeNode(compName)) {
        renderComponentPreview<Manager>(scene, entity, compName, bit);
        ImGui::TreePop();
      }

      ImGui::PopID();
    }
  }

  template <reg::CompileTimeBitMaskType Manager>
  static void renderComponentPreview(pain::AbstractScene<Manager> &scene,
                                     reg::Entity entity,
                                     const char * /*compName*/, int bit)
  {
    auto check = [&]<typename C>() {
      if constexpr (Manager::template isRegistered<C>()) {
        constexpr int myBit =
            Manager::template singleComponentBitmask<C>().value;
        if (myBit == (1 << bit)) {
          const auto &comp = scene.template getComponent<C>(entity);
          ComponentPreview<std::remove_cvref_t<C>>::render(comp);
        }
      }
    };

    check.template operator()<pain::Transform2dComponent>();
    check.template operator()<pain::Movement2dComponent>();
    check.template operator()<pain::Transform3dComponent>();
    check.template operator()<pain::Movement3dComponent>();
    check.template operator()<pain::OrthoCameraComponent>();
    check.template operator()<pain::PerspCameraComponent>();
    check.template operator()<pain::NativeScriptComponent>();
    check.template operator()<pain::ParticleSprayComponent>();
    check.template operator()<pain::RotationComponent>();
    check.template operator()<pain::SpriteComponent>();
    check.template operator()<pain::LuaScriptComponent>();
    check.template operator()<pain::SAPCollider>();
    check.template operator()<pain::LuaSchedulerComponent>();
    check.template operator()<pain::MeshComponent>();
    check.template operator()<pain::MaterialComponent>();
    check.template operator()<pain::LightComponent>();
    check.template operator()<pain::TrailComponent>();
    check.template operator()<pain::TextComponent>();
    check.template operator()<pain::ColorIndexComponent>();
  }
};

} // namespace painless
