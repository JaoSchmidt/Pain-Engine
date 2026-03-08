/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/Renderers.h"
#include "CoreRender/Renderer/Renderer3d.h"

namespace pain
{
Renderers Renderers::create() { return Renderers(); }
Renderers::Renderers()
    : m_materialManager(MaterialManager::create()),
      m_renderer2d(Renderer2d::createRenderer2d(m_materialManager)),
      m_renderer3d(Renderer3d::createRenderer3d(m_materialManager)),
      m_renderContext(RenderContext::create(m_materialManager)) {};

} // namespace pain
