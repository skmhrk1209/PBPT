#pragma once

#include <optional>

namespace coex {

template <typename, template <typename...> typename, typename...>
struct is_detected_impl : std::false_type {};

template <template <typename...> typename Op, typename... Ts>
struct is_detected_impl<std::void_t<Op<Ts...>>, Op, Ts...> : std::true_type {};

template <template <typename...> typename Op, typename... Ts>
using is_detected = is_detected_impl<std::void_t<>, Op, Ts...>;

template <template <typename...> typename Op, typename... Ts>
constexpr auto is_detected_v = is_detected<Op, Ts...>::value;

template <typename>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template <typename T>
constexpr auto is_optional_v = is_optional<T>::value;

template <typename>
struct is_string : std::false_type {};

template <typename Char, typename Traits, typename Allocator>
struct is_string<std::basic_string<Char, Traits, Allocator>> : std::true_type {};

template <typename T>
constexpr auto is_string_v = is_string<T>::value;

template <typename T>
concept Iterable = requires(T x) { std::begin(x), std::end(x); };

template <typename T>
concept Indexable = requires(T x) { x[std::declval<std::size_t>()]; };

}  // namespace coex
