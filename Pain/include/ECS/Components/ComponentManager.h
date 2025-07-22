#include "ECS/Registry/Bitmask.h"

#include "ECS/Components/Camera.h"
#include "ECS/Components/LuaScript.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Rotation.h"
#include "ECS/Components/Sprite.h"

namespace pain
{
struct LuaScriptComponent;
using ComponentManager =
    CompileTimeBitMask<OrthoCameraComponent, TransformComponent,
                       MovementComponent, NativeScriptComponent,
                       ParticleSprayComponent, RotationComponent,
                       SpriteComponent, SpritelessComponent, TrianguleComponent,
                       LuaScriptComponent, onUpdateLuaFunction>;

} // namespace pain
