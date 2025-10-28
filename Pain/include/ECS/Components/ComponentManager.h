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
struct ColliderComponent;
struct OrthoCameraComponent;
struct TransformComponent;
struct MovementComponent;
struct ParticleSprayComponent;
struct RotationComponent;
struct SpriteComponent;
struct SpritelessComponent;
struct TrianguleComponent;
struct onUpdateLuaFunction;
struct CollisionComponent;
struct ImGuiComponent;

using ComponentManager = reg::CompileTimeBitMask<
    OrthoCameraComponent, TransformComponent, MovementComponent,
    NativeScriptComponent, ParticleSprayComponent, RotationComponent,
    SpriteComponent, SpritelessComponent, TrianguleComponent,
    LuaScriptComponent, onUpdateLuaFunction, ImGuiComponent,
    CollisionCallbackComponent, ColliderComponent, CollisionComponent>;

} // namespace pain
