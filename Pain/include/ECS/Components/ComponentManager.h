/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once
#include "ECS/Registry/Bitmask.h"

namespace pain
{
namespace tag
{
struct OrthoCamera;
struct LuaScheduleTask;
struct LuaScript;
struct Transform2d;
struct Movement2d;
struct ParticleSpray;
struct Rotation;
struct Sprite;
struct Spriteless;
struct Triangule;
struct NativeScript;
struct ImGuiScript;
struct Collider;
struct SAPCollider;
} // namespace tag

// basic World Scene Manager
using WorldComponents = reg::CompileTimeBitMask< //
    tag::OrthoCamera,                            // 1
    tag::Transform2d,                            // 2
    tag::Movement2d,                             // 4
    tag::NativeScript,                           // 8
    tag::ParticleSpray,                          // 16
    tag::Rotation,                               // 32
    tag::Sprite,                                 // 64
    tag::Spriteless,                             // 128
    tag::Triangule,                              // 256
    tag::LuaScript,                              // 512
    tag::SAPCollider,                            // 1024
    tag::LuaScheduleTask>;                       // 2048

// if using NaiveCollisionSys instead of SAPCollider:
// using WorldComponents = reg::CompileTimeBitMask<
//     tag::OrthoCamera, tag::Transform2d, tag::Movement2d, tag::NativeScript,
//     tag::ParticleSpray, tag::Rotation, tag::Sprite, tag::Spriteless,
//     tag::Triangule, tag::LuaScript, tag::Collider, tag::LuaScheduleTask>;

// using only relevant to UI
using UIComponents = reg::CompileTimeBitMask<tag::ImGuiScript, tag::OrthoCamera,
                                             tag::Transform2d>;

static_assert(UIComponents::allRegistered<tag::ImGuiScript, tag::Transform2d>(),
              "banana");

// HACK: Stupidity finder
#define XOR(A, B) (A && !B) || (!A && B)
static_assert(XOR(WorldComponents::isRegistered<tag::SAPCollider>(),
                  WorldComponents::isRegistered<tag::Collider>()),
              "Cannot mix collider types togheter");
} // namespace pain
