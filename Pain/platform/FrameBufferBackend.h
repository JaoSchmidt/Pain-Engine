/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


// FrameBufferBackend.h
#pragma once

#include "CoreRender/FrameBuffer.h"
namespace pain::backend
{

void bindFrameBuffer(uint32_t rendererId);
void unbindFrameBuffer();
uint32_t createFrameBuffer(FrameBufferCreationInfo &spec);
void deleteFrameBuffer(uint32_t rendererId);
void resizeFrameBuffer(const FrameBufferCreationInfo &spec);

} // namespace pain::backend
