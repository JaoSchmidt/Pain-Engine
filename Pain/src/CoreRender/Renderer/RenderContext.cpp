/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/RenderContext.h"
#include "CoreRender/Renderer/Renderer3d.h"

namespace pain
{
Renderers Renderers::create() { return Renderers(); }
Renderers::Renderers()
    : m_materialManager(MaterialManager::create()),
      renderer2d(Renderer2d::createRenderer2d(m_materialManager)),
      renderer3d(Renderer3d::createRenderer3d(m_materialManager)) {};

} // namespace pain
