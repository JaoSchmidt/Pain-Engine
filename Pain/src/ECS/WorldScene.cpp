/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */



#include "ECS/WorldScene.h"

namespace pain
{

Scene Scene::create(reg::EventDispatcher &eventDispatcher, sol::state &solState,
                    ThreadPool &threadPool)
{
  return Scene(eventDispatcher, solState, threadPool);
}

} // namespace pain
