#include "CoreFiles/ResourceManagerSing.h"

#include <SDL2/SDL_surface.h>

#include <SDL_image.h>
namespace pain
{

namespace resources
{
// NOTE: remember folks, surfaceMap is in the static/global memory but its
// content are in the heap
static std::unordered_map<const char *, SDL_Surface *> m_surfacesMap = {};

SDL_Surface *getSurface(const char *filepath)
{
  auto search = m_surfacesMap.find(filepath);
  if (search != m_surfacesMap.end()) {
    return search->second;
  }
  SDL_Surface *surface = IMG_Load(filepath);
  if (surface) {
    m_surfacesMap[filepath] = surface;
  }
  return surface;
}

} // namespace resources

} // namespace pain
