module;
#include "pch.h"
export module pain.ResourceManager;

export namespace pain
{

namespace resources
{
SDL_Surface *getSurface(const char *filepath);
} // namespace resources

} // namespace pain
