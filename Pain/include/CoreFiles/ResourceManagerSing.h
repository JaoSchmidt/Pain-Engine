#pragma once
#include "pch.h"

namespace pain
{

class Resources
{
public:
  static Resources *getInstance();
  Resources(Resources &other) = delete;
  void operator=(const Resources &) = delete;
  static Resources *instance();
  SDL_Surface *getSurface(const std::string filepath);

private:
  std::unordered_map<std::string, SDL_Surface *> m_surfacesMap;
  Resources() {}
  static Resources *m_instance;
};

} // namespace pain
