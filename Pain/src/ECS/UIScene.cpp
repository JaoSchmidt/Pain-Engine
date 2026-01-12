#include "ECS/UIScene.h"

namespace pain
{

UIScene UIScene::create(reg::EventDispatcher &eventDispatcher,
                        sol::state &solState, ThreadPool &threadPool)
{
  return UIScene(eventDispatcher, solState, threadPool);
}

} // namespace pain
