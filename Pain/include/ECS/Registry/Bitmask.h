#pragma once
#include "ECS/Registry/Entity.h"
#include <cstddef>
#include <type_traits>
namespace reg
{

template <typename... Components> class CompileTimeBitMask
{

public:
  // Get the index of a component type (compile-time)
  template <typename Target> static constexpr Bitmask singleComponentBitmask()
  {
    static_assert(isRegistered<Target>(),
                  "You are asking for a component but haven't registered it "
                  "yet inside the register");
    using CleanTarget = std::remove_pointer_t<
        std::remove_cv_t<std::remove_reference_t<Target>>>;
    return exp(getComponentIndex<CleanTarget, Components...>());
  }

  // Get combined bit mask for multiple components
  template <typename... Targets>
  static constexpr Bitmask multiComponentBitmask()
  {
    if constexpr (sizeof...(Targets) > 0)
      return getComponentsBitmask<Targets...>();
    else
      return Bitmask{0};
  }

  // Get total number of registered components
  static constexpr std::size_t getNumberOfRegisteredComponents()
  {
    return m_comp_num;
  }

  // Check if a component type is registered
  template <typename Target> static constexpr bool isRegistered()
  {
    using CleanTarget = std::remove_pointer_t<
        std::remove_cv_t<std::remove_reference_t<Target>>>;
    return typenameContainsType<CleanTarget, Components...>();
  }

  template <typename... Ts> static constexpr bool allRegistered()
  {
    return (isRegistered<Ts>() && ...);
  }

private:
  static constexpr std::size_t m_comp_num = sizeof...(Components);
  // Recursive helper to check if a type exists in the parameter pack
  template <typename Target, typename First, typename... Rest>
  static constexpr bool typenameContainsType()
  {
    if constexpr (std::is_same_v<First, Target>) {
      return true;
    } else if constexpr (sizeof...(Rest) > 0) {
      return typenameContainsType<Target, Rest...>();
    } else {
      return false;
    }
  }

  template <typename Target, typename First, typename... Rest>
  static constexpr std::size_t getComponentIndex(size_t n = 0)
  {
    if constexpr (std::is_same_v<First, Target>) {
      return n;
    } else if constexpr (sizeof...(Rest) > 0) {
      return getComponentIndex<Target, Rest...>(n + 1);
    }
  }

  template <typename FirstDirty, typename... Targets>
  static constexpr Bitmask getComponentsBitmask()
  {
    using First = std::remove_pointer_t<
        std::remove_cv_t<std::remove_reference_t<FirstDirty>>>;
    static_assert(isRegistered<First>(),
                  "One of the components you are trying to access isn't "
                  "properly registered inside the CompileTimeBitmask");
    if constexpr (sizeof...(Targets) > 0) {
      return exp(getComponentIndex<First, Components...>()) |
             getComponentsBitmask<Targets...>();
    } else {
      return exp(getComponentIndex<First, Components...>());
    }
  }

  static constexpr Bitmask exp(std::size_t n) { return Bitmask{1 << n}; }
};

// ---------------------------------------------------- //
// Assert the right class inside registries
// ---------------------------------------------------- //
template <typename T> struct is_compile_time_bitmask : std::false_type {
};

template <typename... Components>
struct is_compile_time_bitmask<CompileTimeBitMask<Components...>>
    : std::true_type {
};

template <typename T>
inline constexpr bool is_compile_time_bitmask_v =
    is_compile_time_bitmask<T>::value;

template <typename T>
concept CompileTimeBitMaskType = is_compile_time_bitmask_v<T>;

} // namespace reg
