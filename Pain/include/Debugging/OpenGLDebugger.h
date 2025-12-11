namespace pain
{
#include "Core.h"

namespace Debug
{
void glErrorHandler(unsigned int source, unsigned int type, unsigned int id,
                    unsigned int severity, int lenght, const char *message,
                    const void *userParam);
GLenum glCheckError_(const char *file, int line, const char *func);

#define P_OPENGL_CHECK(...)                                                    \
  if (Debug::glCheckError_(__FILE__, __LINE__, PAIN_FUNC_SIG))                 \
    PLOG_E(__VA_ARGS__);

} // namespace Debug

} // namespace pain
