#pragma once

#include <complex>
#include <concepts>
#include <type_traits>

namespace coex::math {

constexpr auto root_impl(std::floating_point auto x, std::floating_point auto y, std::floating_point auto z, std::integral auto n) -> decltype(x) {
    return z - y < std::numeric_limits<decltype(x)>::epsilon() ? y : root_impl(x, y - (pow(y, n) - x) / (n * pow(y, n - 1)), y, n);
}

constexpr auto root_impl(std::floating_point auto x, std::floating_point auto y, std::integral auto n) {
    return root_impl(x, y - (pow(y, n) - x) / (n * pow(y, n - 1)), y, n);
}

constexpr auto root(std::floating_point auto x, std::integral auto n) { return root_impl(x, std::max<decltype(x)>(x, 1), n); }

constexpr auto sqrt(std::floating_point auto x) { return root(x, 2); }

constexpr auto cbrt(std::floating_point auto x) { return root(x, 3); }

constexpr auto pow(auto x, std::integral auto n) -> decltype(x) { return n == 1 ? x : x * pow(x, n - 1); }

constexpr auto square(auto x) { return pow(x, 2); }

constexpr auto cube(auto x) { return pow(x, 3); }

constexpr auto lerp(const auto &in_val, const auto &in_min, const auto &in_max, const auto &out_min, const auto &out_max) {
    return out_min + (out_max - out_min) * (in_val - in_min) / (in_max - in_min);
}

template <typename T>
constexpr auto abs(const std::complex<T> &x) {
    return sqrt(std::real(x * std::conj(x)));
}

}  // namespace coex::math
