/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ECS/UIScene.h"

namespace pain
{

UIScene UIScene::create(reg::EventDispatcher &eventDispatcher,
                        sol::state &solState, ThreadPool &threadPool)
{
  return UIScene(eventDispatcher, solState, threadPool);
}

} // namespace pain
