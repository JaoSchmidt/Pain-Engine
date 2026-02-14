/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// DumbObject.h
#pragma once
#include "Core.h"
#include <pain.h>

namespace DumbObject
{
reg::Entity create(pain::Scene &scene);
}

class DumbObjectScript : public pain::WorldObject
{
public:
  using pain::WorldObject::WorldObject;

  void onCreate();
  void onUpdate(pain::DeltaTime deltaTimeSec);

private:
  float m_moveSpeed = 5.0f;
};
