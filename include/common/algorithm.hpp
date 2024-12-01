#pragma once

#include <tuple>

namespace pbpt {

// Apply a function to each element of a tuple.
template <auto... Is>
constexpr auto for_each_impl(auto &&tuple, auto &&function, std::index_sequence<Is...>) {
  // Don't unpack in function parameters because it's not guaranteed each parameter is evaluated from left to right.
  // [](auto &&...){}((function(std::get<Is>(std::forward<decltype(tuple)>(tuple))), 0)...);

  // So, unpack in initializer list.
  std::initializer_list<int>{(function(std::get<Is>(std::forward<decltype(tuple)>(tuple))), 0)...};
}

// Apply a function to each element of a tuple.
constexpr auto for_each(auto &&tuple, auto &&function) {
  for_each_impl(
      std::forward<decltype(tuple)>(tuple), std::forward<decltype(function)>(function),
      std::make_index_sequence<std::tuple_size_v<decltype(tuple)>>()
  );
}

}  // namespace pbpt
