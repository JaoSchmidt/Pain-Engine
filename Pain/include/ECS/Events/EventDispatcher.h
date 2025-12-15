#pragma once

#include "Core.h"

class EventDispatcher
{
public:
  template <typename Event> void subscribe(fn) {}

  template <typename Event> void trigger();

  template <typename Event> void enqueue();

  void update();
};
