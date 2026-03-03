/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** ImageFormat.h */
#pragma once

#include <cstdint>

/** Pixel format used for texture and image resources. */
enum class ImageFormat {
  None = 0, /** No valid format. */
  R8,       /** 8-bit single channel. */
  RGB8,     /** 8-bit RGB. */
  RGBA8,    /** 8-bit RGBA. */
  RGBA32F   /** 32-bit float RGBA. */
};
