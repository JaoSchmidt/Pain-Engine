/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
/// @file pain.h
/// @brief Public umbrella header for the Pain engine API.

/// For use specifically for games built with Pain.
#include "Assets/ManagerIni.h"
#include "Assets/ManagerMaterial.h"
#include "Assets/ManagerTexture.h"
#include "Assets/RandNumberGenerator.h"
#include "CoreFiles/Application.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "GUI/ImGuiSys.h"
// Misc
#include "GUI/Launcher.h"
#include "Misc/BasicOrthoCamera.h"
#include "Misc/BasicPerspCamera.h"
#include "Misc/BasicShape.h"
#include "Misc/Events.h"
// Renderer
#include "CoreRender/Buffers/BufferLayout.h"
#include "CoreRender/Buffers/Buffers.h"
#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Buffers/Texture.h"
#include "CoreRender/Buffers/VertexArray.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/LightComponent.h"
#include "CoreRender/LightSys.h"
#include "CoreRender/MaterialComponent.h"
#include "CoreRender/MeshComponent.h"
#include "CoreRender/Render2dSys.h"
#include "CoreRender/Render3dSys.h"
#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "CoreRender/SpriteComponent.h"
#include "CoreRender/Text/Font.h"
// ECS
#include "ECS/Components/NativeScript.h"
#include "ECS/Scene.h"
#include "ECS/Scriptable.h"
// Events
#include "Events/EventDispatcher.h"
// Scripts
#include "Misc/Schedule/SchedulerComponent.h"
#include "Misc/Schedule/SchedulerSys.h"
#include "Scripting/Lua/LuaScriptComponent.h"
#include "Scripting/Lua/LuaScriptSys.h"
#include "Scripting/Lua/WorldSceneBind.h"
#include "Scripting/Native/NativeScriptSys.h"

#include "Physics/Collision/Collider.h"
#include "Physics/Collision/SweepAndPruneSys.h"
#include "Physics/KinematicsSys.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/MovementComponent.h"
#include "Physics/Particles/ParticleSys.h"
#include "Physics/RotationComponent.h"

#include <SDL2/SDL_events.h>

#include "imgui.h"

/// @name Logging Macros
/// @{
#define LOG_T(...) ::pain::logWrapper::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_I(...) ::pain::logWrapper::GetClientLogger()->info(__VA_ARGS__)
#define LOG_W(...) ::pain::logWrapper::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_E(...) ::pain::logWrapper::GetClientLogger()->error(__VA_ARGS__)
#define LOG_F(...) ::pain::logWrapper::GetClientLogger()->critical(__VA_ARGS__)
/// @}

/// @name Assertion Macro
/// @{
#ifndef NDEBUG
#define ASSERT(x, s, ...)                                                      \
  {                                                                            \
    if (!(x)) {                                                                \
      LOG_E("Assertion Failed: " s, ##__VA_ARGS__);                            \
      assert(x);                                                               \
    }                                                                          \
  }
#else
#define ASSERT(x, ...)
#endif
/// @}

namespace pain
{

/**
 * @brief Engine bootstrap utilities.
 *
 * This struct provides static helpers used by applications to initialize
 * configuration and run the main Application lifetime.
 */
struct Pain {
  /// @brief Initializes logging and checks whether the settings GUI is
  /// required.
  /// @return True if the settings GUI should be shown.
  static bool initiateIni();

  /// @brief Runs an application, deletes it, and returns its end flags.
  static EndGameFlags runAndDeleteApplication(Application *app);
};
} // namespace pain
