#pragma once

// For use specifically for games build with Pain

#include "CoreFiles/Application.h"
#include "CoreFiles/ImGuiController.h"
#include "CoreFiles/Layer.h"
#include "CoreFiles/LayerStack.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreFiles/RandNumberGenerator.h"
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
#include "ECS/GameObject.h"
#include "ECS/Scene.h"
#include "ECS/SceneManager.h"
#include "ECS/Scriptable.h"
// Misc
#include "Misc/BasicOrthoCamera.h"
#include "Misc/BasicShape.h"
#include "Misc/PerspCameraController.h"

#include <SDL2/SDL_events.h>

#include "imgui.h"

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
  static void initiate();
  static void runApplication(Application *app);
};
} // namespace pain

#define LOG_T(...) ::pain::LogWrapper::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_I(...) ::pain::LogWrapper::GetClientLogger()->info(__VA_ARGS__)
#define LOG_W(...) ::pain::LogWrapper::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_E(...) ::pain::LogWrapper::GetClientLogger()->error(__VA_ARGS__)
#define LOG_F(...) ::pain::LogWrapper::GetClientLogger()->critical(__VA_ARGS__)
