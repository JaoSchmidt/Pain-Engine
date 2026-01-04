#pragma once
#include "ECS/Registry/Bitmask.h"

namespace cmp
{
struct OrthoCamera;
struct LuaScheduleTask;
} // namespace cmp

namespace pain
{
template <typename Alias, typename Target> struct AliasComponent {
  using type = Target;
};
struct LuaScriptComponent;
struct NativeScriptComponent;
struct Transform2dComponent;
struct Movement2dComponent;
struct ParticleSprayComponent;
struct RotationComponent;
struct SpriteComponent;
struct SpritelessComponent;
struct TrianguleComponent;
struct onUpdateLuaFunction;
struct ImGuiComponent;

// mutually exclusive collision components
struct ColliderComponent;
struct SAPCollider;

// basic World Scene Manager
using ComponentManager = reg::CompileTimeBitMask<
    cmp::OrthoCamera, Transform2dComponent, Movement2dComponent,
    NativeScriptComponent, ParticleSprayComponent, RotationComponent,
    SpriteComponent, SpritelessComponent, TrianguleComponent,
    LuaScriptComponent, onUpdateLuaFunction, SAPCollider, cmp::LuaScheduleTask>;

// using NaiveCollisionSys instead of SAPCollider
using CMNaiveCollision = reg::CompileTimeBitMask<
    cmp::OrthoCamera, Transform2dComponent, Movement2dComponent,
    NativeScriptComponent, ParticleSprayComponent, RotationComponent,
    SpriteComponent, SpritelessComponent, TrianguleComponent,
    LuaScriptComponent, onUpdateLuaFunction, ColliderComponent,
    cmp::LuaScheduleTask>;

// using only relevant to UI
using UIManager = reg::CompileTimeBitMask<ImGuiComponent, cmp::OrthoCamera,
                                          Transform2dComponent>;

static_assert(UIManager::isRegistered<ImGuiComponent>(), "banana");

// HACK: Stupidity finder
#define XOR(A, B) (A && !B) || (!A && B)
static_assert(XOR(ComponentManager::isRegistered<SAPCollider>(),
                  ComponentManager::isRegistered<ColliderComponent>()),
              "Cannot mix collider types togheter");
} // namespace pain
