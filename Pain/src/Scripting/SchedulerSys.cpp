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


#include "Scripting/SchedulerSys.h"
#include "Debugging/Profiling.h"
#include "Scripting/SchedulerComponent.h"

// NOTE: I believe lua script can be tough to paralize because of the OOP nature
// of scripts
namespace pain
{
namespace Systems
{

void LuaSchedulerSys::onUpdate(DeltaTime deltaTime)
{
  PROFILE_SCOPE("onUpdate - LuaSchedulerSys");

  auto chunks = query<cmp::LuaScheduleTask>();
  float deltaTimef = deltaTime.getSecondsf();
  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      auto &lst = scripts[i];

      lst.elapsed += deltaTimef;
      if (lst.elapsed >= lst.interval) {
        lst.elapsed -= lst.interval;
        if (lst.onScheduleFunction) {
          sol::protected_function_result result = (lst.onScheduleFunction)(lst);

          if (!result.valid()) {
            PLOG_E("Lua error (m_onScheduleFunction): {}",
                   result.get<sol::error>().what());
          }
        }
      }
    }
  }
}
} // namespace Systems
} // namespace pain
