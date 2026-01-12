#pragma once

// For use specifically for games build with Pain

#include "Assets/DefaultTexture.h"
#include "Assets/RandNumberGenerator.h"
#include "CoreFiles/Application.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "GUI/ImGuiSys.h"
// Misc
#include "GUI/Launcher.h"
#include "Misc/BasicOrthoCamera.h"
#include "Misc/BasicShape.h"
#include "Misc/Events.h"
#include "Misc/PerspCameraController.h"
// Renderer
#include "CoreRender/BufferLayout.h"
#include "CoreRender/Buffers.h"
#include "CoreRender/RenderSys.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Text/Font.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
// ECS
#include "ECS/Components/NativeScript.h"
#include "ECS/Components/Sprite.h"
#include "ECS/EventDispatcher.h"
#include "ECS/Scene.h"
#include "ECS/Scriptable.h"
#include "Physics/MovementComponent.h"
// Scripts
#include "Scripting/Component.h"
#include "Scripting/LuaScriptSys.h"
#include "Scripting/NativeScriptSys.h"
#include "Scripting/SchedulerComponent.h"
#include "Scripting/SchedulerSys.h"

#include "Physics/Collision/Collider.h"
#include "Physics/Collision/SweepAndPruneSys.h"
#include "Physics/KinematicsSys.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"

#include <SDL2/SDL_events.h>

#include "imgui.h"

#define LOG_T(...) ::pain::logWrapper::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_I(...) ::pain::logWrapper::GetClientLogger()->info(__VA_ARGS__)
#define LOG_W(...) ::pain::logWrapper::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_E(...) ::pain::logWrapper::GetClientLogger()->error(__VA_ARGS__)
#define LOG_F(...) ::pain::logWrapper::GetClientLogger()->critical(__VA_ARGS__)

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

namespace pain
{
struct Pain {
  static bool initiate();
  static EndGameFlags runAndDeleteApplication(Application *app);
};
} // namespace pain
