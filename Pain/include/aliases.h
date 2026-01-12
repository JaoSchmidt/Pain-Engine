#pragma once
#include <functional>

template <typename Tp> using aRefWrap = std::reference_wrapper<Tp>;

template <typename T> using aScope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr aScope<T> createScope(Args &&...args)
{
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T> using aRef = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr aRef<T> createRef(Args &&...args)
{
  return std::make_shared<T>(std::forward<Args>(args)...);
}
