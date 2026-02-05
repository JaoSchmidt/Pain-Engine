/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// VertexArrayBackend.h
#pragma once

#include "CoreRender/Buffers.h"
#include <cstdint>

namespace pain::backend
{

uint32_t createVertexArray();
void destroyVertexArray(uint32_t id);
void bindVertexArray(uint32_t id);
void unbindVertexArray();
void addVertexBuffer(const VertexBuffer &vertexBuffer, uint32_t rendererId,
                     uint32_t &index);
void setIndexBuffer(const IndexBuffer &indexBuffer, uint32_t rendererId);
} // namespace pain::backend
