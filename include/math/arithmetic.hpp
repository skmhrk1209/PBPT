#pragma once

#include <complex>
#include <concepts>
#include <type_traits>

namespace coex::math {

constexpr auto sqrt(auto x) { return std::sqrt(x); }

constexpr auto cbrt(auto x) { return std::cbrt(x); }

constexpr auto pow(auto x, auto n) { return std::pow(x, n); }

constexpr auto square(auto x) { return std::pow(x, 2); }

constexpr auto cube(auto x) { return std::pow(x, 3); }

constexpr auto lerp(const auto &in_val, const auto &in_min, const auto &in_max, const auto &out_min, const auto &out_max) {
    return out_min + (out_max - out_min) * (in_val - in_min) / (in_max - in_min);
}

}  // namespace coex::math
