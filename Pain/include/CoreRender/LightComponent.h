#pragma once

#include "CoreRender/Renderer/Colors.h"
#include "ECS/Components/ComponentManager.h"

namespace pain
{

/**
 * @brief Supported light types.
 */
enum class LightType : uint8_t { Directional = 0, Point, Spot };

/**
 * @brief Configuration used when creating a LightComponent.
 */
struct LightCreationInfo {
  LightType type = LightType::Directional;
  Color color = {255, 255, 255, 255};
  float intensity = 1.0f;

  // Point / Spot
  float range = 100.0f;

  // Spot only
  float innerCutoff = 12.5f; // degrees
  float outerCutoff = 17.5f; // degrees
};

/**
 * @brief ECS component representing a light source.
 *
 * Pure data container. No rendering logic.
 * Used by Renderer3D to gather lighting information.
 */
struct LightComponent {
  using tag = tag::Light;

  // ------------------------------------------------------------
  // Core Light Data
  // ------------------------------------------------------------

  LightType m_type = LightType::Directional;
  Color m_color = {255, 255, 255, 255};
  float m_intensity = 1.0f;

  // ------------------------------------------------------------
  // Point / Spot parameters
  // ------------------------------------------------------------

  float m_range = 100.0f;

  // Spot specific
  float m_innerCutoff = 12.5f;
  float m_outerCutoff = 17.5f;

  // ------------------------------------------------------------
  // Factory
  // ------------------------------------------------------------

  static LightComponent create(const LightCreationInfo &info = {})
  {
    return LightComponent{.m_type = info.type,
                          .m_color = info.color,
                          .m_intensity = info.intensity,
                          .m_range = info.range,
                          .m_innerCutoff = info.innerCutoff,
                          .m_outerCutoff = info.outerCutoff};
  }
};

} // namespace pain
