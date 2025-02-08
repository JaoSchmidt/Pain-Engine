#include <tuple>

namespace pain
{
/* A simple wrapper around std::tuple that has an attached get function
 *
 * For example, it allow you to use your_tuple.get<Component>() instead of
 * std::get<Component>(your_tuple)
 * */
template <typename... Types> class Tuple
{
public:
  std::tuple<Types...> t; // Encapsulated tuple

  template <typename... Args>
  Tuple(Args &&...args) : t(std::forward<Args>(args)...)
  {
  }
  template <typename... Args>
  explicit Tuple(std::tuple<Args...> t) : t(std::move(t))
  {
  }

  // Type-based access (since C++14)
  template <typename T> inline T &get() { return std::get<T>(t); }
  template <typename T> inline const T &get() const { return std::get<T>(t); }
  // Index-based access
  template <std::size_t N> inline auto &get() { return std::get<N>(t); }
  template <std::size_t N> inline const auto &get() const
  {
    return std::get<N>(t);
  }

  // Exposed tuple member functions
  void swap(Tuple &other) noexcept { t.swap(other.t); }

  constexpr std::size_t size() const noexcept
  {
    return std::tuple_size_v<std::tuple<Types...>>;
  }

  inline bool operator==(const Tuple &other) const { return t == other.t; }
  inline bool operator!=(const Tuple &other) const { return t != other.t; }
  inline bool operator>(const Tuple &other) const { return t > other.t; }
  inline bool operator<(const Tuple &other) const { return t < other.t; }
  inline bool operator>=(const Tuple &other) const { return t >= other.t; }
  inline bool operator<=(const Tuple &other) const { return t <= other.t; }
  inline bool operator<=>(const Tuple &other) const { return t <=> other.t; }
};

} // namespace pain

namespace std
{
// this will enable structure binding
template <typename... Types>
struct tuple_size<pain::Tuple<Types...>>
    : std::tuple_size<std::tuple<Types...>> {
};

// this will enable structure binding
template <std::size_t N, typename... Types>
struct tuple_element<N, pain::Tuple<Types...>>
    : std::tuple_element<N, std::tuple<Types...>> {
};

} // namespace std
