#pragma once

#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/Movement.h"
#include "ECS/Entity.h"
#include "ECS/Registry/Tuple.h"
#include <tuple>
#include <type_traits>

/*
 * The Archetype desgin can be better understood by searching online or by
 * reading a bit of explanation on the Unity Documentation:
 * https://docs.unity3d.com/Packages/com.unity.entities@0.17/manual/index.html
 *
 * WARN: WAIT! Before diving in, noticed that the Archetype uses a lot of
 * meta-programming, I recommend that you take a look at those tutorials first:
 * https://www.youtube.com/watch?v=VBI6TSo8Zog&list=PLWxziGKTUvQFIsbbFcTZz7jOT4TMGnZBh&index=1
 * https://stackoverflow.com/questions/64794649/is-there-a-way-to-consistently-sort-type-template-parameters
 */

namespace pain
{

// ---------------------------------------------------- //
// type "name" retriever function
// ---------------------------------------------------- //
template <typename T> constexpr std::string_view type_name() noexcept
{
  std::string_view name = "Error: unsupported compiler", prefix, suffix;
#ifdef __clang__
  name = __PRETTY_FUNCTION__;
  prefix = "auto type_name() [T = ";
  suffix = "]";
#elif defined(__GNUC__)
  name = __PRETTY_FUNCTION__;
  prefix = "constexpr auto type_name() [with T = ";
  suffix = "]";
#elif defined(_MSC_VER)
  name = __FUNCSIG__;
  prefix = "auto __cdecl type_name<";
  suffix = ">(void) noexcept";
#else
  static_assert(false, "Unsupported compiler!");
#endif
  name.remove_prefix(prefix.size());
  name.remove_suffix(suffix.size());
  return name;
}

template <class Left, class Right>
static constexpr bool less_than = type_name<Left>() < type_name<Right>();
// ---------------------------------------------------- //
// List definition
// ---------------------------------------------------- //

template <typename... Ts> struct temp_list;

template <template <typename...> class Ins, typename...> struct instantiate;

template <template <typename...> class Ins, typename... Ts>
struct instantiate<Ins, temp_list<Ts...>> {
  using type = Ins<Ts...>;
};

template <template <typename...> class Ins, typename... Ts>
using instantiate_t = typename instantiate<Ins, Ts...>::type;

// ---------------------------------------------------- //
// Concat definition
// ---------------------------------------------------- //

template <typename...> struct concat;

template <typename... Ts, typename... Us>
struct concat<temp_list<Ts...>, temp_list<Us...>> {
  using type = temp_list<Ts..., Us...>;
};

template <typename... Ts> using concat_t = typename concat<Ts...>::type;

// ---------------------------------------------------- //
// Take<Count> definition
// ---------------------------------------------------- //

template <int N, typename... Ts> struct take;

template <int Count, typename... Ts>
using take_t = typename take<Count, Ts...>::type;

// Base 0
template <typename... Ts> struct take<0, temp_list<Ts...>> {
  using type = temp_list<>;
  using rest = temp_list<Ts...>;
};

// Base 1
template <typename A, typename... Ts_minus1>
struct take<1, temp_list<A, Ts_minus1...>> {
  using type = temp_list<A>;
  using rest = temp_list<Ts_minus1...>;
};

// Recursion
template <int N, class A, class... Ts_minusN>
struct take<N, temp_list<A, Ts_minusN...>> {
  using type = concat_t<temp_list<A>, take_t<N - 1, temp_list<Ts_minusN...>>>;
  using rest = typename take<N - 1, temp_list<Ts_minusN...>>::rest;
};

// ---------------------------------------------------- //
// merge_sort definition
// ---------------------------------------------------- //

template <class...> struct merge_sort;

template <class... Ts> using merge_sort_t = typename merge_sort<Ts...>::type;

// Base 1
template <class... Rs> struct merge_sort<temp_list<>, temp_list<Rs...>> {
  using type = temp_list<Rs...>;
};

// Base 2
template <class... Ls> struct merge_sort<temp_list<Ls...>, temp_list<>> {
  using type = temp_list<Ls...>;
};

// recursion
template <class LHead, class... Ls, class RHead, class... Rs>
struct merge_sort<temp_list<LHead, Ls...>, temp_list<RHead, Rs...>> {
  // clang-format off
  using type =
    // IF
    std::conditional_t<less_than<LHead, RHead>,
      concat_t<
        temp_list<LHead>,merge_sort_t<temp_list<Ls...>, temp_list<RHead, Rs...>>
      >,
    // ELSE
      concat_t<
        temp_list<RHead>,merge_sort_t<temp_list<LHead, Ls...>, temp_list<Rs...>>
      >
    >;
};
// clang-format on

// ---------------------------------------------------- //
// Sort List definition
// ---------------------------------------------------- //

template <class... Ts> struct sort_list;

template <class... Ts> using sort_list_t = typename sort_list<Ts...>::type;

template <class T> struct sort_list<temp_list<T>> {
  using type = temp_list<T>;
};

template <class L, class R> struct sort_list<L, R> {
  using type =
      std::conditional_t<less_than<L, R>, temp_list<L, R>, temp_list<R, L>>;
};

template <class... Types> struct sort_list<temp_list<Types...>> {
  static constexpr auto divide = sizeof...(Types) / 2;
  using split = take<divide, temp_list<Types...>>;
  using type = merge_sort_t<sort_list_t<typename split::type>,
                            sort_list_t<typename split::rest>>;
};

// ---------------------------------------------------- //
// Container definition
// ---------------------------------------------------- //

template <typename... Components> struct UnsortedArchetype {
  std::vector<Entity> m_entities;
  // "SoA", more like map of vectors
  std::tuple<std::vector<Components>...> m_components;

  bool has(Entity entity) const
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it == m_entities.end())
      return false;
    return true;
  }

  void addEntity(Entity entity) { m_entities.push_back(entity); }

  // TODO: Allow use of reserve() function and multiple emplaces
  template <typename C, typename... Args> void pushComponent(Args &&...args)
  {
    std::vector<C> &componentArray = std::get<std::vector<C>>(m_components);
    componentArray.emplace(C(std::forward<Args>(args)...));
  }

  // Remove an entity from the archetype
  bool remove(Entity entity)
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it == m_entities.end())
      return false;

    // remove entity
    int index = it - m_entities.begin();
    m_entities.erase(m_entities.begin() + index);

    // remove components
    std::apply(
        [](auto &&...vector) { ((vector.erase(vector.begin() + index)), ...); },
        m_components);
    return true;
  }

  inline std::vector<Entity>::const_iterator begin() const
  {
    return m_entities.begin();
  }
  inline std::vector<Entity>::const_iterator end() const
  {
    return m_entities.end();
  }

  // Extract a column of an archetype. I.e. the components of an entity
  Tuple<const Components &...> extractColumn(Entity entity) const
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return std::make_tuple(fetchComponent<Components>(index)...);
  };
  Tuple<Components &...> extractColumn(Entity entity)
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return std::make_tuple(fetchComponent<Components>(index)...);
  };

  // Same as extractColumn but with extract a single component of the entity
  template <typename T> T &extractComponent(Entity entity)
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return fetchComponent<T>(index);
  }
  template <typename T> const T &extractComponent(Entity entity) const
  {
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    int index = it - m_entities.begin();
    return fetchComponent<T>(index);
  }

  template <typename T> const T &fetchComponent(int entityIndex) const
  {
    std::vector<T> componentVector = std::get<T>(m_components);
    P_ASSERT((unsigned)entityIndex >= componentVector.size(),
             "Entity index {} out of range", entityIndex);

    return componentVector[entityIndex];
  }

  template <typename T> T &fetchComponent(int entityIndex)
  {
    std::vector<T> componentVector = std::get<T>(m_components);
    P_ASSERT((unsigned)entityIndex >= componentVector.size(),
             "Entity index {} out of range", entityIndex);

    return componentVector[entityIndex];
  }
  template <typename C> std::vector<C> &getComponent()
  {
    return std::get<C>(m_components);
  }
  template <typename C> const std::vector<C> &getComponent() const
  {
    return std::get<C>(m_components);
  }
};

// ---------------------------------------------------- //
// Type definition
// ---------------------------------------------------- //

template <class... Ts>
using Archetype =
    instantiate_t<UnsortedArchetype, sort_list_t<temp_list<Ts...>>>;

static_assert(std::is_same_v<Archetype<MovementComponent, TransformComponent>,
                             Archetype<TransformComponent, MovementComponent>>);

} // namespace pain
