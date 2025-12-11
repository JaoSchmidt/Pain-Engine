template <typename... ExcludeComponents> struct exclude_t {
};

template <typename... ExcludeComponents>
inline constexpr exclude_t<ExcludeComponents...> exclude{};
