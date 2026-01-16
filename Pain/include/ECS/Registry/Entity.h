/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "spdlog/fmt/bundled/base.h"
#include <cstdint>
#include <ostream>

// NOTE: tag here is to separate equal definitions when using T
template <typename T, typename Tag> struct StrongType {
  T value;

  constexpr explicit StrongType(T v = {}) : value(v) {}

  // Allow implicit conversion to T for internal use (optional)
  constexpr operator T() const noexcept { return value; }

  // Equality operators
  constexpr auto operator<=>(const StrongType &) const = default;
  friend std::ostream &operator<<(std::ostream &os, const StrongType &v)
  {
    return os << v.value;
  };
};
namespace reg
{
struct BitmaskTag {
};
struct EntityTag {
};
struct ColumnTag {
};

using Bitmask = StrongType<std::int32_t, BitmaskTag>;
using Entity = StrongType<std::int32_t, EntityTag>;
using Column = StrongType<std::int32_t, ColumnTag>;
struct Record {
  Bitmask bitmask;
  Column column;
};
// ---------------------------------------------------- //
// Bitmask operations on "Bitmask"
// clang-format off
// ---------------------------------------------------- //

constexpr Bitmask operator|(Bitmask lhs, Bitmask rhs) noexcept { return Bitmask(lhs.value | rhs.value); }
constexpr Bitmask operator&(Bitmask lhs, Bitmask rhs) noexcept { return Bitmask(lhs.value & rhs.value); }
constexpr Bitmask operator^(Bitmask lhs, Bitmask rhs) noexcept { return Bitmask(lhs.value ^ rhs.value); }
constexpr Bitmask operator~(Bitmask v) noexcept { return Bitmask(~v.value); }
constexpr Bitmask &operator|=(Bitmask &lhs, Bitmask rhs) noexcept { lhs.value |= rhs.value; return lhs; }
constexpr Bitmask &operator&=(Bitmask &lhs, Bitmask rhs) noexcept { lhs.value &= rhs.value; return lhs; }
constexpr Bitmask &operator^=(Bitmask &lhs, Bitmask rhs) noexcept { lhs.value ^= rhs.value; return lhs; }

} // namespace reg

// clang-format on

// For spdlog
template <> struct fmt::formatter<reg::Entity> : fmt::formatter<int32_t> {
  auto format(const reg::Entity &e, fmt::format_context &ctx) const
  {
    return fmt::formatter<int32_t>::format(e.value, ctx);
  }
};

// For remove batch function
namespace std
{
template <> struct hash<reg::Bitmask> {
  size_t operator()(const reg::Bitmask &b) const noexcept
  {
    return std::hash<std::int32_t>()(b.value);
  }
};

template <> struct hash<reg::Column> {
  size_t operator()(const reg::Column &c) const noexcept
  {
    return std::hash<std::int32_t>()(c.value);
  }
};

// For collision atm
template <> struct hash<reg::Entity> {
  size_t operator()(const reg::Entity &c) const noexcept
  {
    return std::hash<std::int32_t>()(c.value);
  }
};

} // namespace std
