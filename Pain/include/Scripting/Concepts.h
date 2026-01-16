/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "Assets/DeltaTime.h"
// TODO: Change this to use render context
namespace pain
{
struct Renderer2d;
}

// --------------------------------------------------------------- //
// concepts for methods to use as scripts. Use this to check if a class is
// asking for a specific script function
// --------------------------------------------------------------- //

template <typename T>
concept has_onCreate_method = requires(T &&t) {
  { t.onCreate() };
};
template <typename T>
concept has_onRender_method =
    requires(T &&t, pain::Renderer2d &r, bool m, pain::DeltaTime d) {
      { t.onRender(r, m, d) };
    };
template <typename T>
concept has_onUpdate_method = requires(T &&t, pain::DeltaTime d) {
  { t.onUpdate(d) };
};
template <typename T>
concept has_onDestroy_method = requires(T &&t) {
  { t.onDestroy() };
};
template <typename T>
concept has_onEvent_method = requires(T &&t, const SDL_Event &e) {
  { t.onEvent(e) };
};

// --------------------------------------------------------------- //
// This was suggested by claude to check for privates. I.e. sometimes I forgot
// the public keyword
// &T::onFunction will be true even if the function is private
// --------------------------------------------------------------- //

template <typename T>
concept has_private_onCreate =
    requires { sizeof(&T::onCreate); } && !has_onCreate_method<T>;

template <typename T>
concept has_private_onUpdate =
    requires { sizeof(&T::onUpdate); } && !has_onUpdate_method<T>;

template <typename T>
concept has_private_onRender =
    requires { sizeof(&T::onRender); } && !has_onRender_method<T>;

template <typename T>
concept has_private_onEvent =
    requires { sizeof(&T::onEvent); } && !has_onEvent_method<T>;

template <typename T>
concept has_private_onDestroy =
    requires { sizeof(&T::onDestroy); } && !has_onDestroy_method<T>;

// --------------------------------------------------------------- //
// concepts for static asserts. Use this to check if you or other developers are
// defining incorrect functions
// --------------------------------------------------------------- //
template <typename T>
concept has_any_callable_onRender = requires(T t) { &T::onRender; };

template <typename T>
concept has_any_callable_onUpdate = requires(T t) { &T::onUpdate; };

template <typename T>
concept has_any_callable_onEvent = requires(T t) { &T::onEvent; };

template <typename T>
concept has_any_callable_onCreate = requires(T t) { &T::onCreate; };

template <typename T>
concept has_any_callable_onDestroy = requires(T t) { &T::onDestroy; };

template <typename T> void check_script_methods()
{
  // Check for wrong signatures
  if constexpr (has_any_callable_onRender<T> && !has_onRender_method<T>) {
    static_assert(false, "Error: onRender() has wrong signature! Should be "
                         "onRender(Renderer2d&, bool, DeltaTime).");
  }
  if constexpr (has_any_callable_onUpdate<T> && !has_onUpdate_method<T>) {
    static_assert(false, "Error: onUpdate() has wrong signature! Should be "
                         "onUpdate(DeltaTime).");
  }
  if constexpr (has_any_callable_onEvent<T> && !has_onEvent_method<T>) {
    static_assert(false, "Error: onEvent() has wrong signature! Should be "
                         "onEvent(const SDL_Event&).");
  }
  if constexpr (has_any_callable_onCreate<T> && !has_onCreate_method<T>) {
    static_assert(false, "Error: onCreate() has wrong signature! Should be "
                         "onCreate() with no parameters.");
  }
  if constexpr (has_any_callable_onDestroy<T> && !has_onDestroy_method<T>) {
    static_assert(false, "Error: onDestroy() has wrong signature! Should be "
                         "onDestroy() with no parameters.");
  }
  // Check for private methods
  if constexpr (has_private_onCreate<T>) {
    static_assert(false, "Error: onCreate() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (has_private_onUpdate<T>) {
    static_assert(false, "Error: onUpdate() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (has_private_onRender<T>) {
    static_assert(false, "Error: onRender() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (has_private_onEvent<T>) {
    static_assert(false, "Error: onEvent() exists but is not public! Make sure "
                         "it's in the public section.");
  }
  if constexpr (has_private_onDestroy<T>) {
    static_assert(false, "Error: onDestroy() exists but is not public! Make "
                         "sure it's in the public section.");
  }
}
