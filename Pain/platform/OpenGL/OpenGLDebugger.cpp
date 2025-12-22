#include "OpenGLDebugger.h"
#include "CoreFiles/LogWrapper.h"

namespace pain
{

namespace Debug
{
// this servers to be used as OpenGL callback. Automatic. Only works with
// OpenGL 4.3+
void glErrorHandler(unsigned int source, unsigned int type, unsigned int id,
                    unsigned int severity, int lenght, const char *message,
                    const void *userParam)
{
  UNUSED(lenght)
  UNUSED(userParam)
  // uncomment this if your gpu being too educated with warnings
  // notification warnings are usually just optimizations anyway
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

  if (GL_DEBUG_SEVERITY_HIGH == type || GL_DEBUG_TYPE_ERROR == type) {
    PLOG_E("-----------------------------------");
    PLOG_E("Debug message ({}): {}", id, message);
  } else {
    PLOG_W("-----------------------------------");
    PLOG_W("Debug message ({}): {}", id, message);
  }
  // clang-format off
  switch (source)
  {
      case GL_DEBUG_SOURCE_API:             PLOG_W(  "Source: API"); break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   PLOG_W(  "Source: Window System"); break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER: PLOG_W(  "Source: Shader Compiler"); break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:     PLOG_W(  "Source: Third Party"); break;
      case GL_DEBUG_SOURCE_APPLICATION:     PLOG_W(  "Source: Application"); break;
      case GL_DEBUG_SOURCE_OTHER:           PLOG_W(  "Source: Other"); break;
  }
  switch (type)
  {
      case GL_DEBUG_TYPE_ERROR:               PLOG_E(  "Type: Error"); break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: PLOG_W(  "Type: Deprecated Behaviour"); break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  PLOG_W(  "Type: Undefined Behaviour"); break;
      case GL_DEBUG_TYPE_PORTABILITY:         PLOG_W(  "Type: Portability"); break;
      case GL_DEBUG_TYPE_PERFORMANCE:         PLOG_W(  "Type: Performance"); break;
      case GL_DEBUG_TYPE_MARKER:              PLOG_W(  "Type: Marker"); break;
      case GL_DEBUG_TYPE_PUSH_GROUP:          PLOG_W(  "Type: Push Group"); break;
      case GL_DEBUG_TYPE_POP_GROUP:           PLOG_W(  "Type: Pop Group"); break;
      case GL_DEBUG_TYPE_OTHER:               PLOG_W(  "Type: Other"); break;
  }
  switch (severity)
  {
      case GL_DEBUG_SEVERITY_HIGH:         PLOG_E(  "Severity: high"); break;
      case GL_DEBUG_SEVERITY_MEDIUM:       PLOG_W(  "Severity: medium"); break;
      case GL_DEBUG_SEVERITY_LOW:          PLOG_W(  "Severity: low"); break;
      case GL_DEBUG_SEVERITY_NOTIFICATION: PLOG_W(  "Severity: notification"); break;
  }
	// if (GL_DEBUG_SEVERITY_HIGH == type || GL_DEBUG_TYPE_ERROR == type)
	// 	P_ASSERT(false,"OpenGL critical error");
  // clang-format on
}

// This is usefull to be put after every openGL operation to detect errors
GLenum glCheckError_(const char *file, int line, const char *func)
{
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    switch (errorCode) {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_STACK_OVERFLOW:
      error = "STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      error = "STACK_UNDERFLOW";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION";
      break;
    }
    PLOG_E("OpenGL error code {}: {}", errorCode, error);
    PLOG_E("{} | {} ({})", func, file, line);
  }
  return errorCode;
}
} // namespace Debug
} // namespace pain
