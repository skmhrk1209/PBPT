#pragma once

#include <cstdint>

namespace pbpt::random {

template <typename T = std::int_fast32_t>
constexpr auto now() {
  return static_cast<T>(__TIME__[0] - '0') * 10 + static_cast<T>(__TIME__[1] - '0') * 60 * 60 +
         static_cast<T>(__TIME__[3] - '0') * 10 + static_cast<T>(__TIME__[4] - '0') * 60 +
         static_cast<T>(__TIME__[6] - '0') * 10 + static_cast<T>(__TIME__[7] - '0');
}

}  // namespace pbpt::random
