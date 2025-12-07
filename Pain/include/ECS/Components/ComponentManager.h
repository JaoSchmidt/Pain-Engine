#pragma once
#include "ECS/Registry/Bitmask.h"

namespace pain
{
template <typename Alias, typename Target> struct AliasComponent {
  using type = Target;
};
struct LuaScriptComponent;
struct NativeScriptComponent;
struct CollisionCallbackComponent;
struct OrthoCameraComponent;
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

using ComponentManager = reg::CompileTimeBitMask<
    OrthoCameraComponent, Transform2dComponent, Movement2dComponent,
    NativeScriptComponent, ParticleSprayComponent, RotationComponent,
    SpriteComponent, SpritelessComponent, TrianguleComponent,
    LuaScriptComponent, onUpdateLuaFunction, ImGuiComponent,
    CollisionCallbackComponent, SAPCollider>;

using CMNaiveCollision = reg::CompileTimeBitMask<
    OrthoCameraComponent, Transform2dComponent, Movement2dComponent,
    NativeScriptComponent, ParticleSprayComponent, RotationComponent,
    SpriteComponent, SpritelessComponent, TrianguleComponent,
    LuaScriptComponent, onUpdateLuaFunction, ImGuiComponent,
    CollisionCallbackComponent, ColliderComponent>;

// HACK: Stupidity finder
#define XOR(A, B) (A && !B) || (!A && B)
static_assert(XOR(ComponentManager::isRegistered<SAPCollider>(),
                  ComponentManager::isRegistered<ColliderComponent>()),
              "Cannot mix collider types togheter");
} // namespace pain
