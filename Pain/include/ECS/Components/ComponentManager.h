#include "ECS/Registry/Bitmask.h"

#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Components/Particle.h"
#include "ECS/Components/Rotation.h"
#include "ECS/Components/Sprite.h"

namespace pain
{
using ComponentManager = CompileTimeBitMask<
    OrthoCameraComponent, TransformComponent, MovementComponent,
    NativeScriptComponent, ParticleSprayComponent, RotationComponent,
    SpriteComponent, SpritelessComponent, TrianguleComponent>;

}
