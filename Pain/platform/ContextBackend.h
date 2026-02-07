/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// ContextBackend.h
#pragma once

#include "CoreRender/VertexArray.h"
namespace pain::backend
{

void Init();
void InitRenderer();
void setViewPort(int x, int y, int width, int height);
void setClearColor(const glm::vec4 &color);
void clear();
void drawIndexed(const VertexArray &vertexArray, uint32_t indexCount = 0);
void drawInstanced(uint32_t indiceCount = 0, uint32_t instanceCount = 0);
unsigned getTMU();
int getTMUi();

} // namespace pain::backend
