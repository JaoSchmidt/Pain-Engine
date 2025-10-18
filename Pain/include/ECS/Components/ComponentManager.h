#pragma once
#include "ECS/Registry/Bitmask.h"

#include "ECS/Components/Camera.h"
#include "ECS/Components/LuaScript.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Rotation.h"
#include "ECS/Components/Sprite.h"

// TODO: Test without any include, that is, try declare and not define anything

namespace pain
{
template <typename Alias, typename Target> struct AliasComponent {
  using type = Target;
};
struct LuaScriptComponent;
struct NativeScriptComponent;
struct CollisionCallbackComponent;
struct ColliderComponent;
struct CollisionComponent;
struct ImGuiComponent;
using ComponentManager = CompileTimeBitMask<
    OrthoCameraComponent, TransformComponent, MovementComponent,
    NativeScriptComponent, ParticleSprayComponent, RotationComponent,
    SpriteComponent, SpritelessComponent, TrianguleComponent,
    LuaScriptComponent, onUpdateLuaFunction, ImGuiComponent,
    CollisionCallbackComponent, ColliderComponent, CollisionComponent>;

} // namespace pain
