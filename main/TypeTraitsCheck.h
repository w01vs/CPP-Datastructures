#ifndef TYPE_TRAITS_CHECK
#define TYPE_TRAITS_CHECK

#include <type_traits>
#include <ostream>

template <typename X, typename = void>
struct is_equatable : std::false_type {};

template <typename X>
struct is_equatable<X, std::void_t<decltype(std::declval<X>() == std::declval<X>()) >> : std::true_type {};

template <typename X, typename = void>
struct is_printable : std::false_type {};

template <typename X>
struct is_printable<X, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<X>()) >> : std::true_type {};

#endif // !TYPE_TRAITS_CHECK



