/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Stats/ComponentIcons.h"
#include "Assets/ManagerTexture.h"
#include <CoreFiles/LogWrapper.h>

namespace painless
{

// FAKE texture path -- replace with your real component icons spritesheet
static constexpr const char *kPlaceholderTexturePath =
    "assets/editor/component_icons.png";

// Grid layout: 5 rows x 4 columns (19 icons + 1 unused slot)
static constexpr unsigned kGridRows = 5;
static constexpr unsigned kGridCols = 4;
static constexpr float kPadding = 0.5F;

// (row, col) for each component, ordered by WorldComponents bit index:
//   0  OrthoCamera     (0,0)
//   1  PerspCamera     (0,1)
//   2  Transform2d     (0,2)
//   3  Movement2d      (0,3)
//   4  Transform3d     (1,0)
//   5  Movement3d      (1,1)
//   6  NativeScript    (1,2)
//   7  ParticleSpray   (1,3)
//   8  Rotation        (2,0)
//   9  Sprite          (2,1)
//  10  LuaScript       (2,2)
//  11  SAPCollider     (2,3)
//  12  LuaScheduleTask (3,0)
//  13  Mesh            (3,1)
//  14  Material        (3,2)
//  15  Light           (3,3)
//  16  ParticleTrail   (4,0)
//  17  TextComponent   (4,1)
//  18  ColorIndex      (4,2)

static constexpr std::initializer_list<std::pair<int, int>> kComponentCoords = {
    {0, 0}, // OrthoCamera
    {0, 1}, // PerspCamera
    {0, 2}, // Transform2d
    {0, 3}, // Movement2d
    {1, 0}, // Transform3d
    {1, 1}, // Movement3d
    {1, 2}, // NativeScript
    {1, 3}, // ParticleSpray
    {2, 0}, // Rotation
    {2, 1}, // Sprite
    {2, 2}, // LuaScript
    {2, 3}, // SAPCollider
    {3, 0}, // LuaScheduleTask
    {3, 1}, // Mesh
    {3, 2}, // Material
    {3, 3}, // Light
    {4, 0}, // ParticleTrail
    {4, 1}, // TextComponent
    {4, 2}, // ColorIndex
};

ComponentIconSheet ComponentIconSheet::create()
{
  return {
      .sheet = &pain::TextureManager::createWithDivisions(
          "ComponentSymbols", kPlaceholderTexturePath, kGridRows, kGridCols,
          kComponentCoords, false, kPadding) //
  };
}

} // namespace painless
