/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "CoreRender/Buffers/Shader.h"
#include "CoreRender/Renderer/Colors.h"
#include "ECS/Components/ComponentManager.h"

namespace pain
{

/** Use this component mainly for debug purposes or very specific object
 * rendering. It will overwrite a material color. */
struct ColorIndexComponent {
  using tag = tag::ColorIndex;
  Color color = Colors::StrongPink;
};

} // namespace pain
