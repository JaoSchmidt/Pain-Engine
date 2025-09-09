#pragma once

// For use specifically for games build with Pain

#include "Assets/RandNumberGenerator.h"
#include "CoreFiles/Application.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "GUI/ImGuiSystem.h"
// Misc
#include "GUI/Launcher.h"
#include "Misc/BasicOrthoCamera.h"
#include "Misc/BasicShape.h"
#include "Misc/PerspCameraController.h"
// Renderer
#include "CoreRender/AllBuffers.h"
#include "CoreRender/BufferLayout.h"
#include "CoreRender/Camera.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Text/Font.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
// ECS
#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Components/Rotation.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Entity.h"
#include "ECS/Scene.h"
#include "ECS/SceneManager.h"
#include "ECS/Scriptable.h"
// Scripts
#include "Scripting/Component.h"

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
