#pragma once

// For use specifically for games build with Pain

#include "CoreFiles/Application.h"
#include "CoreFiles/Layer.h"
#include "CoreFiles/LayerStack.h"
#include "CoreFiles/LogWrapper.h"

#include "CoreRender/AllBuffers.h"
#include "CoreRender/BufferLayout.h"
#include "CoreRender/Camera.h"
#include "CoreRender/Renderer.h"
#include "CoreRender/Shader.h"
#include "CoreRender/VertexArray.h"

#ifndef NDEBUG
#define ASSERT(x, ...)                                                         \
  {                                                                            \
    if (!(x)) {                                                                \
      LOG_E("Assertion Failed: {0}", __VA_ARGS__);                             \
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
