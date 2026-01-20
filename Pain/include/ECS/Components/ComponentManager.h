/**
 * @file ComponentManager.h
 * @brief Defines compile-time component sets used to build ECS scenes.
 *
 * This file declares the component tag types and the compile-time component
 * layouts used by the engine to construct different scenes (e.g. World, UI).
 *
 * Each scene defines its own component list using reg::CompileTimeBitMask.
 * The order of types inside the bitmask determines the generated archetype
 * identifiers at compile time.
 *
 * This allows high-level ECS APIs such as:
 *  - getComponents<Cs...>()
 *  - createComponents<Cs...>()
 *  - hasComponents<Cs...>()
 *  - query<Cs...>()
 *
 * to implicitly use the correct bitmask without exposing raw bit values or
 * requiring users to manually manage masks.
 *
 * The ECS registry implementation is intentionally not documented here, as it
 * will eventually live in a separate library.
 */

#pragma once
#include "ECS/Registry/Bitmask.h"

namespace pain
{

/**
 * @namespace pain::tag
 * @brief Component tag types used for compile-time registration.
 *
 * These empty structs act as unique type identifiers for ECS components.
 * They are used exclusively as template parameters for CompileTimeBitMask
 * and related ECS queries.
 */
namespace tag
{
struct OrthoCamera;
struct LuaScheduleTask;
struct LuaScript;
struct Transform2d;
struct Movement2d;
struct ParticleSpray;
struct Rotation;
struct Sprite;
struct Spriteless;
struct Triangule;
struct NativeScript;
struct ImGuiScript;
struct Collider;
struct SAPCollider;
} // namespace tag

/**
 * @typedef WorldComponents
 * @brief Component layout for the main world scene.
 *
 * Defines the full set of components available in the gameplay / world scene.
 * The order of the listed tags determines the compile-time bit positions used
 * internally by the ECS.
 *
 * Changing the order or contents of this list affects archetype identifiers
 * and should be done carefully.
 */
using WorldComponents = reg::CompileTimeBitMask< //
    tag::OrthoCamera,                            // 1
    tag::Transform2d,                            // 2
    tag::Movement2d,                             // 4
    tag::NativeScript,                           // 8
    tag::ParticleSpray,                          // 16
    tag::Rotation,                               // 32
    tag::Sprite,                                 // 64
    tag::Spriteless,                             // 128
    tag::Triangule,                              // 256
    tag::LuaScript,                              // 512
    tag::SAPCollider,                            // 1024
    tag::LuaScheduleTask                         // 2048
    >;

/**
 * @typedef UIComponents
 * @brief Component layout for UI-only scenes.
 *
 * Contains only the components relevant for UI rendering and interaction.
 * This allows the engine to create isolated scenes with smaller, specialized
 * component sets.
 */
using UIComponents = reg::CompileTimeBitMask<tag::ImGuiScript, tag::OrthoCamera,
                                             tag::Transform2d>;

static_assert(UIComponents::allRegistered<tag::ImGuiScript, tag::Transform2d>(),
              "stop being stupid");

// NOTE: keep this comment as requested.
// HACK: Stupidity finder
#define XOR(A, B) (A && !B) || (!A && B)

/**
 * @brief Ensures exactly one collider type is registered in WorldComponents.
 *
 * Prevents accidental configuration errors where both collider implementations
 * are enabled simultaneously.
 */
static_assert(XOR(WorldComponents::isRegistered<tag::SAPCollider>(),
                  WorldComponents::isRegistered<tag::Collider>()),
              "Cannot mix collider types togheter");

} // namespace pain
