#pragma once

// For use specifically for games build with Pain

#include "CoreFiles/Application.h"
#include "CoreFiles/Layer.h"
#include "CoreFiles/LayerStack.h"
#include "CoreFiles/LogWrapper.h"
// Renderer
#include "CoreRender/AllBuffers.h"
#include "CoreRender/BufferLayout.h"
#include "CoreRender/Camera.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
// ECS
#include "ECS/Components/Movement.h"
#include "ECS/Entity.h"
#include "ECS/GameObjWrapper.h"
#include "ECS/Scene.h"
#include "ECS/SceneManager.h"
// Misc
#include "Misc/BasicOrthoCamera.h"
#include "Misc/PerspCameraController.h"
#include "Misc/SimpleShapeClasses.h"

#include <SDL2/SDL_events.h>

#ifndef NDEBUG
#define ASSERT(x, s, ...)                                                      \
  {                                                                            \
    if (!(x)) {                                                                \
      LOG_E(std::string("Assertion Failed: ").append(s), __VA_ARGS__);         \
      assert(x);                                                               \
    }                                                                          \
  }
#else
#define ASSERT(x, ...)
#endif

#define LOG_T(...) ::pain::LogWrapper::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_I(...) ::pain::LogWrapper::GetClientLogger()->info(__VA_ARGS__)
#define LOG_W(...) ::pain::LogWrapper::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_E(...) ::pain::LogWrapper::GetClientLogger()->error(__VA_ARGS__)
#define LOG_F(...) ::pain::LogWrapper::GetClientLogger()->critical(__VA_ARGS__)

namespace pain
{
struct Pain {
  static void initiate();
  static void runApplication(Application *app);
};
} // namespace pain
