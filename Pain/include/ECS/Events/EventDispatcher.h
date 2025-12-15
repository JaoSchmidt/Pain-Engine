#pragma once

#include "Core.h"
#include "ECS/Events/Events.h"
#include <functional>

class EventDispatcher
{
public:
  template <typename Event> using Listener = std::function<void(const Event &)>;
  template <typename Event> void subscribe() {}

  template <typename Event> void trigger();

  template <typename Event> void enqueue();

  void update();
};
