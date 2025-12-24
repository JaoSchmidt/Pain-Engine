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

} // namespace pain::backend
