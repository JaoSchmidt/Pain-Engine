/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


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
