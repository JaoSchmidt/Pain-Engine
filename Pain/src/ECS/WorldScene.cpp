
#include "ECS/WorldScene.h"

namespace pain
{

Scene Scene::create(reg::EventDispatcher &eventDispatcher, sol::state &solState,
                    ThreadPool &threadPool)
{
  return Scene(eventDispatcher, solState, threadPool);
}

} // namespace pain
