#pragma once

#include "CoreRender/CameraComponent.h"
#include "CoreRender/ColorIndexComponent.h"
#include "CoreRender/LightComponent.h"
#include "CoreRender/MaterialComponent.h"
#include "CoreRender/MeshComponent.h"
#include "CoreRender/SpriteComponent.h"
#include "CoreRender/Text/TextComponent.h"
#include "ECS/Components/NativeScript.h"
#include "Misc/Schedule/SchedulerComponent.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/MovementComponent.h"
#include "Physics/Particles/SprayCmp.h"
#include "Physics/RotationComponent.h"
#include "Scripting/Lua/LuaScriptComponent.h"

namespace cmp
{
using Pos2d = pain::Transform2dComponent;
using Mov2d = pain::Movement2dComponent;
using Cam2d = pain::OrthoCameraComponent;
using Cam3d = pain::PerspCameraComponent;
using Sprite = pain::SpriteComponent;
using Material = pain::MaterialComponent;
using Mesh = pain::MeshComponent;
using Light = pain::LightComponent;
using Text = pain::TextComponent;
using ColorIdx = pain::ColorIndexComponent;
using Script = pain::NativeScriptComponent;
using Lua = pain::LuaScriptComponent;
using Pos3d = pain::Transform3dComponent;
using Mov3d = pain::Movement3dComponent;
using Rot = pain::RotationComponent;
using ParticleSpray = pain::ParticleSprayComponent;
using Schedule = pain::LuaSchedulerComponent;
} // namespace cmp
