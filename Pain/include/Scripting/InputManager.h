#pragma once
#include <SDL2/SDL_scancode.h>

class InputManager
{
public:
  static inline bool isKeyPressed(SDL_Scancode scancode)
  {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    return state[scancode] != 0;
  }
};
