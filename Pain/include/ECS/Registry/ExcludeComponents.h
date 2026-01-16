/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


template <typename... ExcludeComponents> struct exclude_t {
};

template <typename... ExcludeComponents>
inline constexpr exclude_t<ExcludeComponents...> exclude{};
