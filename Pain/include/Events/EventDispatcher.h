/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"

#include <functional>
#include <map>
#include <memory>
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <typeindex>

/**
 * @brief Concept that constrains events that can be forwarded to Lua.
 *
 * An event type must provide a function:
 *
 * @code
 * sol::table Event::toLuaTable(sol::state& s);
 * @endcode
 *
 * This allows native C++ events to be automatically converted and dispatched
 * to Lua subscribers.
 */
template <typename Event>
concept LuaConvertable = requires(Event &e, sol::state &s) {
  { e.toLuaTable(s) };
};

namespace reg
{

/**
 * @brief Type-erased event dispatcher supporting C++ and Lua listeners.
 *
 * EventDispatcher acts as a central event bus that allows:
 *
 * - Subscribing native C++ listeners to strongly-typed events.
 * - Subscribing Lua functions to strongly-typed C++ events.
 * - Dispatching events immediately (trigger).
 * - Deferring events until the end of a frame (enqueue + update).
 * - Custom Lua-to-Lua events identified by numeric IDs.
 *
 * Events are internally type-erased using hashed type information so that
 * heterogeneous event types can coexist in the same dispatcher.
 *
 * @note This class is not thread-safe. All interaction is expected to happen
 *       on the main thread.
 */
class EventDispatcher
{
  // type erasure stuff
  using Deleter = void (*)(void *);
  using ErasedVector = std::unique_ptr<void, Deleter>;

  // useful for subsribing to specific events happening
  template <LuaConvertable Event>
  using Listener = std::function<void(const Event &)>;

  // this will hold the data until the end of a frame/cycle.
  struct EventQueue {
    ErasedVector data;
    std::function<void()> dispatch; // not a subscribe function, but a wrapper
  };

public:
  /**
   * @brief Constructs a new EventDispatcher bound to a Lua state.
   *
   * @param lua Reference to the active Lua state used for event conversion and
   * dispatch.
   */
  EventDispatcher(sol::state &lua) : m_lua(lua) {};
  // --------------------------------------------------
  // C++ Events and Dispatch (not fully static tho)
  // --------------------------------------------------
  /**
   * @brief Subscribes a native C++ listener to a specific event type.
   *
   * @tparam Event Event type to subscribe to.
   * @param listener Callback invoked whenever the event is triggered.
   */
  template <LuaConvertable Event> void subscribe(Listener<Event> listener)
  {
    std::vector<Listener<Event>> &list = getSubscribers<Event>();
    list.emplace_back(listener);
  }

  /**
   * @brief Immediately triggers an event.
   *
   * The event is delivered synchronously to:
   *  - All native C++ subscribers of this event type.
   *  - All Lua subscribers after converting the event to a Lua table.
   *
   * @tparam Event Event type.
   * @param event Event instance to dispatch.
   */
  template <LuaConvertable Event> void trigger(const Event &event)
  {
    std::vector<Listener<Event>> &list = getSubscribers<Event>();
    for (Listener<Event> &handler : list) {
      handler(event);
    }
    trigger<Event>(event.toLuaTable(m_lua));
  }
  /**
   * @brief Enqueues an event to be dispatched later during update().
   *
   * If no subscriber exists for the event type, a warning is logged and the
   * event is discarded.
   *
   * @tparam Event Event type.
   * @param event Event instance to enqueue.
   */
  template <LuaConvertable Event> void enqueue(Event event)
  {
    if (hasEventHandler<Event>()) {
      EventQueue &pending = getPendingEvents<Event>();
      std::vector<Event> *vec =
          static_cast<std::vector<Event> *>(pending.data.get());
      vec->emplace_back(std::move(event));
    } else {
      PLOG_W("Warning, there is no handler for the Event \"{}\"",
             typeid(Event).name());
    }
  };
  /**
   * @brief Dispatches all queued events and Lua pending events.
   *
   * This function should typically be called once per frame.
   * It processes all pending native events first, then processes
   * pending Lua events.
   */
  void update()
  {
    for (auto &[type, pendingQueue] : m_cppPending) {
      pendingQueue.dispatch();
    }
    updateLua();
  }

  // --------------------------------------------------
  // Lua Events and Dispatch
  // --------------------------------------------------
  /**
   * @brief Subscribes a Lua function to a strongly-typed C++ event.
   *
   * @tparam Event Event type.
   * @param fn Lua callback function.
   */
  template <LuaConvertable Event> void subscribe(sol::function fn)
  {
    std::vector<sol::function> &list = getLuaSubscribersCpp<Event>();
    list.emplace_back(std::move(fn));
  }

  /**
   * @brief Immediately triggers a strongly-typed C++ event into Lua.
   *
   * @tparam Event Event type.
   * @param event Lua table representing the event.
   */
  template <LuaConvertable Event> void trigger(const sol::table &event)
  {
    std::vector<sol::function> &list = getLuaSubscribersCpp<Event>();
    for (sol::function &handler : list) {
      handler(event);
    }
  }
  /**
   * @brief Subscribes a Lua function to a custom Lua-only event.
   *
   * @param id Numeric event identifier.
   * @param fn Lua callback function.
   */
  void subscribe(size_t id, sol::function fn);
  /**
   * @brief Enqueues a Lua-only event to be dispatched during update().
   *
   * @param id Numeric event identifier.
   * @param data Lua table payload.
   */
  void enqueue(size_t id, const sol::table data);
  /**
   * @brief Immediately triggers a Lua-only event.
   *
   * @param id Numeric event identifier.
   * @param event Lua table payload.
   */
  void trigger(size_t id, const sol::table &event);

private:
  sol::state &m_lua;
  // C++ events
  std::map<std::type_index, ErasedVector> m_cppSubsribers;
  std::map<std::type_index, EventQueue> m_cppPending;

  // Lua events
  std::map<uint64_t, std::vector<sol::function>> m_luaSubscribers;
  std::map<uint64_t, std::vector<sol::table>> m_luaPending;

  // Lua subsribers for c++ events
  std::map<std::type_index, std::vector<sol::function>> m_luaSubscribersCpp;

  // --------------------------------------------------
  // C++ Events Map
  // --------------------------------------------------
  template <LuaConvertable Event> bool hasEventHandler() const
  {
    return m_cppSubsribers.find(std::type_index(typeid(Event))) !=
           m_cppSubsribers.end();
  }
  template <LuaConvertable Event> EventQueue &getPendingEvents()
  {
    auto it = m_cppPending.find(std::type_index(typeid(Event)));
    if (it != m_cppPending.end()) {
      return it->second;
    } else {

      Deleter deleter = [](void *vector) {
        delete static_cast<std::vector<Event> *>(vector);
      };
      std::vector<Event> *vec = new std::vector<Event>();
      EventQueue pendingQueue = //
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
      auto [newIt, isInserted] = m_cppPending.emplace(
          std::type_index(typeid(Event)), std::move(pendingQueue));

      P_ASSERT(isInserted, "Could not create new component vector");
      PLOG_I("New Event added {}", typeid(Event).name());

      // store the deleter to use inside the destructor
      return newIt->second;
    }
  }
  template <LuaConvertable Event> std::vector<Listener<Event>> &getSubscribers()
  {
    auto it = m_cppSubsribers.find(std::type_index(typeid(Event)));
    if (it != m_cppSubsribers.end()) {
      return *static_cast<std::vector<Listener<Event>> *>(it->second.get());
    } else {

      Deleter deleter = [](void *vector) {
        delete static_cast<std::vector<Listener<Event>> *>(vector);
      };
      auto [newIt, isInserted] = m_cppSubsribers.emplace(
          std::type_index(typeid(Event)),
          ErasedVector{new std::vector<Listener<Event>>(), deleter});
      P_ASSERT(isInserted, "Could not create new component vector");
      PLOG_I("New subscription added {}", typeid(Event).name());

      // store the deleter to use inside the destructor
      return *static_cast<std::vector<Listener<Event>> *>(newIt->second.get());
    }
  }
  // --------------------------------------------------
  // Lua Events Map
  // --------------------------------------------------
  bool hasEventHandlerLua(size_t id) const;
  void updateLua();
  template <LuaConvertable Event>
  std::vector<sol::function> &getLuaSubscribersCpp()
  {
    auto it = m_luaSubscribersCpp.find(std::type_index(typeid(Event)));
    if (it != m_luaSubscribersCpp.end()) {
      return it->second;
    } else {
      auto [newIt, isInserted] = m_luaSubscribersCpp.emplace(
          std::type_index(typeid(Event)), std::vector<sol::function>{});
      P_ASSERT(isInserted, "Could not create new component vector");
      PLOG_I("New subscription added {}", typeid(Event).name());

      // store the deleter to use inside the destructor
      return newIt->second;
    }
  }
};

} // namespace reg
