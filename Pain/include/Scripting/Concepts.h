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
struct Renderers;
}

/** Concepts used during NativeScriptComponent::bind() to validate script
 * callback signatures. These concepts detect whether a user script exposes
 * correctly named and typed lifecycle functions. Invalid signatures or
 * inaccessible functions are rejected at compile time via static assertions. */

// --------------------------------------------------------------- //
// concepts for methods to use as scripts. Use this to check if a class is
// asking for a specific script function
// --------------------------------------------------------------- //

/** Detects whether a type exposes a public onCreate() method with no
 * parameters. */
template <typename T>
concept has_onCreate_method = requires(T &&t) {
  { t.onCreate() };
};

/** Detects whether a type exposes a public onRender(Renderers&, bool,
 * DeltaTime) method. */
template <typename T>
concept has_onRender_method =
    requires(T &&t, pain::Renderers &r, bool m, pain::DeltaTime d) {
      { t.onRender(r, m, d) };
    };

/** Detects whether a type exposes a public onUpdate(DeltaTime) method. */
template <typename T>
concept has_onUpdate_method = requires(T &&t, pain::DeltaTime d) {
  { t.onUpdate(d) };
};

/** Detects whether a type exposes a public onDestroy() method with no
 * parameters. */
template <typename T>
concept has_onDestroy_method = requires(T &&t) {
  { t.onDestroy() };
};

/** Detects whether a type exposes a public onEvent(const SDL_Event&) method. */
template <typename T>
concept has_onEvent_method = requires(T &&t, const SDL_Event &e) {
  { t.onEvent(e) };
};

// --------------------------------------------------------------- //
// This was suggested by claude to check for privates. I.e. sometimes I forgot
// the public keyword
// &T::onFunction will be true even if the function is private
// --------------------------------------------------------------- //

/** Detects a private onCreate() method that is not publicly callable. */
template <typename T>
concept has_private_onCreate =
    requires { sizeof(&T::onCreate); } && !has_onCreate_method<T>;

/** Detects a private onUpdate() method that is not publicly callable. */
template <typename T>
concept has_private_onUpdate =
    requires { sizeof(&T::onUpdate); } && !has_onUpdate_method<T>;

/** Detects a private onRender() method that is not publicly callable. */
template <typename T>
concept has_private_onRender =
    requires { sizeof(&T::onRender); } && !has_onRender_method<T>;

/** Detects a private onEvent() method that is not publicly callable. */
template <typename T>
concept has_private_onEvent =
    requires { sizeof(&T::onEvent); } && !has_onEvent_method<T>;

/** Detects a private onDestroy() method that is not publicly callable. */
template <typename T>
concept has_private_onDestroy =
    requires { sizeof(&T::onDestroy); } && !has_onDestroy_method<T>;

// --------------------------------------------------------------- //
// concepts for static asserts. Use this to check if you or other developers are
// defining incorrect functions
// --------------------------------------------------------------- //

/** Detects whether a type declares any callable onRender member regardless of
 * signature. */
template <typename T>
concept has_any_callable_onRender = requires(T t) { &T::onRender; };

/** Detects whether a type declares any callable onUpdate member regardless of
 * signature. */
template <typename T>
concept has_any_callable_onUpdate = requires(T t) { &T::onUpdate; };

/** Detects whether a type declares any callable onEvent member regardless of
 * signature. */
template <typename T>
concept has_any_callable_onEvent = requires(T t) { &T::onEvent; };

/** Detects whether a type declares any callable onCreate member regardless of
 * signature. */
template <typename T>
concept has_any_callable_onCreate = requires(T t) { &T::onCreate; };

/** Detects whether a type declares any callable onDestroy member regardless of
 * signature. */
template <typename T>
concept has_any_callable_onDestroy = requires(T t) { &T::onDestroy; };

/** Performs compile-time validation of script callback signatures and
 * visibility. */
template <typename T> void check_script_methods()
{
  // Check for wrong signatures
  if constexpr (has_any_callable_onRender<T> && !has_onRender_method<T>) {
    static_assert(false, "Error: onRender() has wrong signature! Should be "
                         "onRender(Renderers&, bool, DeltaTime).");
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
