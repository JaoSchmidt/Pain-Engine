/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "Assets/ManagerShader.h"
#include "CoreRender/RenderContext.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "CoreRender/Text/ManagerFont.h"
#include "platform/ContextBackend.h"
namespace pain
{

struct RenderApi {
  static RenderApi create();
  ShaderManager m_shaderManager;
  MaterialManager m_materialManager;
  FontManager m_fontManager;
  Renderer2d m_renderer2d;
  Renderer3d m_renderer3d;
  RenderContext m_renderContext;

  static void setViewPort(int x, int y, int width, int height)
  {
    backend::setViewPort(x, y, width, height);
  }

private:
  RenderApi();
};

} // namespace pain
