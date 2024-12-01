#pragma once
#include <cstdint>
#include <iostream>

namespace pbpt::random {

template <typename T = std::int_fast32_t, T A = 48271, T B = 0, T M = 2147483647>
struct LinearCongruentialGenerator {
  constexpr LinearCongruentialGenerator() = default;
  constexpr LinearCongruentialGenerator(T seed) : x(seed) {};

  constexpr T operator()() {
    // Stephen K. Park & Keith W. Miller
    // https://c-faq.com/lib/rand.html
    x = a * (x % q) - r * (x / q);
    x = x > 0 ? x : x + m;
    return x;
  }

  static constexpr T min() { return 0; }
  static constexpr T max() { return M; }

 private:
  static constexpr T a = A;
  static constexpr T b = B;
  static constexpr T m = M;
  static constexpr T q = M / A;
  static constexpr T r = M % A;

  T x;
};

}  // namespace pbpt::random
