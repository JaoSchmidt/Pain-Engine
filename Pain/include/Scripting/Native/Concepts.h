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
struct RenderApi;
class RenderContext;
} // namespace pain

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
concept hasOnCreateMethod = requires(T &&t) {
  { t.onCreate() };
};

/** Detects whether a type exposes a public onRender(RenderApi&, bool,
 * DeltaTime) method. */
template <typename T>
concept hasOnRenderMethod =
    requires(T &&t, pain::RenderContext &r, bool m, pain::DeltaTime d) {
      { t.onRender(r, m, d) };
    };

/** Detects whether a type exposes a public onRender(RenderApi&, bool,
 * DeltaTime) method. */
template <typename T>
concept hasOnSystemRenderMethod =
    requires(T &&t, pain::RenderApi &r, bool m, pain::DeltaTime d) {
      { t.onRender(r, m, d) };
    };

/** Detects whether a type exposes a public onUpdate(DeltaTime) method. */
template <typename T>
concept hasOnUpdateMethod = requires(T &&t, pain::DeltaTime d) {
  { t.onUpdate(d) };
};

/** Detects whether a type exposes a public onDestroy() method with no
 * parameters. */
template <typename T>
concept hasOnDestroyMethod = requires(T &&t) {
  { t.onDestroy() };
};

/** Detects whether a type exposes a public onEvent(const SDL_Event&) method. */
template <typename T>
concept hasOnEventMethod = requires(T &&t, const SDL_Event &e) {
  { t.onEvent(e) };
};

// --------------------------------------------------------------- //
// This was suggested by claude to check for privates. I.e. sometimes I forgot
// the public keyword
// &T::onFunction will be true even if the function is private
// --------------------------------------------------------------- //

/** Detects a private onCreate() method that is not publicly callable. */
template <typename T>
concept hasPrivateOnCreate =
    requires { sizeof(&T::onCreate); } && !hasOnCreateMethod<T>;

/** Detects a private onUpdate() method that is not publicly callable. */
template <typename T>
concept hasPrivateOnUpdate =
    requires { sizeof(&T::onUpdate); } && !hasOnUpdateMethod<T>;

/** Detects a private onRender() method that is not publicly callable. */
template <typename T>
concept hasPrivateOnSystemRender =
    requires { sizeof(&T::onRender); } && !hasOnSystemRenderMethod<T>;

/** Detects a private onRender() method that is not publicly callable. */
template <typename T>
concept hasPrivateOnRender =
    requires { sizeof(&T::onRender); } && !hasOnRenderMethod<T>;

/** Detects a private onEvent() method that is not publicly callable. */
template <typename T>
concept hasPrivateOnEvent =
    requires { sizeof(&T::onEvent); } && !hasOnEventMethod<T>;

/** Detects a private onDestroy() method that is not publicly callable. */
template <typename T>
concept hasPrivateOnDestroy =
    requires { sizeof(&T::onDestroy); } && !hasOnDestroyMethod<T>;

// --------------------------------------------------------------- //
// concepts for static asserts. Use this to check if you or other developers are
// defining incorrect functions
// --------------------------------------------------------------- //

/** Detects whether a type declares any callable onRender member regardless of
 * signature. */
template <typename T>
concept hasAnyCallableOnRender = requires(T t) { &T::onRender; };

/** Detects whether a type declares any callable onUpdate member regardless of
 * signature. */
template <typename T>
concept hasAnyCallableOnUpdate = requires(T t) { &T::onUpdate; };

/** Detects whether a type declares any callable onEvent member regardless of
 * signature. */
template <typename T>
concept hasAnyCallableOnEvent = requires(T t) { &T::onEvent; };

/** Detects whether a type declares any callable onCreate member regardless of
 * signature. */
template <typename T>
concept hasAnyCallableOnCreate = requires(T t) { &T::onCreate; };

/** Detects whether a type declares any callable onDestroy member regardless of
 * signature. */
template <typename T>
concept hasAnyCallableOnDestroy = requires(T t) { &T::onDestroy; };

/** Performs compile-time validation of script callback signatures and
 * visibility. */
template <typename T> void checkScriptMethods()
{
  // Check for wrong signatures
  if constexpr (hasAnyCallableOnRender<T> && !hasOnRenderMethod<T>) {
    static_assert(false, "Error: onRender() has wrong signature! Should be "
                         "onRender(RenderContext&, bool, DeltaTime).");
  }
  if constexpr (hasAnyCallableOnUpdate<T> && !hasOnUpdateMethod<T>) {
    static_assert(false, "Error: onUpdate() has wrong signature! Should be "
                         "onUpdate(DeltaTime).");
  }
  if constexpr (hasAnyCallableOnEvent<T> && !hasOnEventMethod<T>) {
    static_assert(false, "Error: onEvent() has wrong signature! Should be "
                         "onEvent(const SDL_Event&).");
  }
  if constexpr (hasAnyCallableOnCreate<T> && !hasOnCreateMethod<T>) {
    static_assert(false, "Error: onCreate() has wrong signature! Should be "
                         "onCreate() with no parameters.");
  }
  if constexpr (hasAnyCallableOnDestroy<T> && !hasOnDestroyMethod<T>) {
    static_assert(false, "Error: onDestroy() has wrong signature! Should be "
                         "onDestroy() with no parameters.");
  }

  // Check for private methods
  if constexpr (hasPrivateOnCreate<T>) {
    static_assert(false, "Error: onCreate() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (hasPrivateOnUpdate<T>) {
    static_assert(false, "Error: onUpdate() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (hasPrivateOnRender<T>) {
    static_assert(false, "Error: onRender() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (hasPrivateOnEvent<T>) {
    static_assert(false, "Error: onEvent() exists but is not public! Make sure "
                         "it's in the public section.");
  }
  if constexpr (hasPrivateOnDestroy<T>) {
    static_assert(false, "Error: onDestroy() exists but is not public! Make "
                         "sure it's in the public section.");
  }
}

/** Performs compile-time validation of script callback signatures and
 * visibility. */
template <typename T> void checkImGuiScriptMethods()
{
  // Check for wrong signatures
  if constexpr (hasAnyCallableOnRender<T> && !hasOnSystemRenderMethod<T>) {
    static_assert(false, "Error: onRender() has wrong signature! Should be "
                         "onRender(RenderApi&, bool, DeltaTime).");
  }
  if constexpr (hasAnyCallableOnEvent<T> && !hasOnEventMethod<T>) {
    static_assert(false, "Error: onEvent() has wrong signature! Should be "
                         "onEvent(const SDL_Event&).");
  }
  if constexpr (hasAnyCallableOnCreate<T> && !hasOnCreateMethod<T>) {
    static_assert(false, "Error: onCreate() has wrong signature! Should be "
                         "onCreate() with no parameters.");
  }
  if constexpr (hasAnyCallableOnDestroy<T> && !hasOnDestroyMethod<T>) {
    static_assert(false, "Error: onDestroy() has wrong signature! Should be "
                         "onDestroy() with no parameters.");
  }

  // Check for private methods
  if constexpr (hasPrivateOnCreate<T>) {
    static_assert(false, "Error: onCreate() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (hasPrivateOnSystemRender<T>) {
    static_assert(false, "Error: onRender() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (hasPrivateOnEvent<T>) {
    static_assert(false, "Error: onEvent() exists but is not public! Make sure "
                         "it's in the public section.");
  }
  if constexpr (hasPrivateOnDestroy<T>) {
    static_assert(false, "Error: onDestroy() exists but is not public! Make "
                         "sure it's in the public section.");
  }
}
