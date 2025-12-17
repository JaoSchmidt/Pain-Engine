#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"

#include <functional>
#include <map>
#include <memory>
#include <typeindex>

namespace reg
{

class EventDispatcher
{
public:
  // type erasure stuff
  using Deleter = void (*)(void *);
  using ErasedVector = std::unique_ptr<void, Deleter>;

  // useful for subsribing to specific events happening
  template <typename Event> using Listener = std::function<void(const Event &)>;
  // useful to hold before triggering

private:
  struct PendingQueue {
    ErasedVector data;
    std::function<void()> dispatch;
  };

public:
  template <typename Event> void subscribe(Listener<Event> listener)
  {
    std::vector<Listener<Event>> &list = getEventObservers<Event>();
    list.emplace_back(listener);
  }

  template <typename Event> void trigger(const Event &event)
  {
    std::vector<Listener<Event>> &list = getEventObservers<Event>();
    for (Listener<Event> &handler : list) {
      handler(event);
    }
  }

  template <typename Event> void enqueue(const Event &event)
  {
    if (hasEventHandler<Event>()) {
      PendingQueue &pending = getEvents<Event>();
      std::vector<Event> *vec =
          static_cast<std::vector<Event> *>(pending.data.get());
      vec->emplace_back(std::move(event));
    } else {
      PLOG_W("Warning, there is no handler for the Event \"{}\"",
             typeid(Event).name());
    }
  };

  void update()
  {
    for (auto &[type, pendingQueue] : m_pending)
      pendingQueue.dispatch();
  }

private:
  std::map<std::type_index, ErasedVector> m_eventObservers;
  std::map<std::type_index, PendingQueue> m_pending;
  template <typename Event> bool hasEventHandler()
  {
    return m_eventObservers.find(std::type_index(typeid(Event))) !=
           m_eventObservers.end();
  }
  template <typename Event> PendingQueue &getEvents()
  {
    auto it = m_pending.find(std::type_index(typeid(Event)));
    if (it != m_pending.end()) {
      return it->second;
    } else {

      Deleter deleter = [](void *vector) {
        delete static_cast<std::vector<Event> *>(vector);
      };
      std::vector<Event> *vec = new std::vector<Event>();
      PendingQueue pendingQueue = //
          {
              .data = {vec, deleter},
              .dispatch =
                  [this, vec]() {
                    for (const Event &event : *vec) {
                      trigger(event);
                    }
                    vec->clear();
                  } //
          };
      auto [newIt, isInserted] = m_pending.emplace(
          std::type_index(typeid(Event)), std::move(pendingQueue));

      P_ASSERT(isInserted, "Could not create new component vector");
      PLOG_I("New Event added {}", typeid(Event).name());

      // store the deleter to use inside the destructor
      return newIt->second;
    }
  }
  template <typename Event> std::vector<Listener<Event>> &getEventObservers()
  {
    auto it = m_eventObservers.find(std::type_index(typeid(Event)));
    if (it != m_eventObservers.end()) {
      return *static_cast<std::vector<Listener<Event>> *>(it->second.get());
    } else {

      Deleter deleter = [](void *vector) {
        delete static_cast<std::vector<Listener<Event>> *>(vector);
      };
      auto [newIt, isInserted] = m_eventObservers.emplace(
          std::type_index(typeid(Event)),
          ErasedVector{new std::vector<Listener<Event>>(), deleter});
      P_ASSERT(isInserted, "Could not create new component vector");
      PLOG_I("New subscription added {}", typeid(Event).name());

      // store the deleter to use inside the destructor
      return *static_cast<std::vector<Listener<Event>> *>(newIt->second.get());
    }
  }
};

} // namespace reg
