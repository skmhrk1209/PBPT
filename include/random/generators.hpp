#pragma once
#include <cstdint>
#include <iostream>

namespace coex::random {

template <typename T = std::int_fast32_t>
constexpr auto now() {
    return static_cast<T>(__TIME__[0] - '0') * 10 + static_cast<T>(__TIME__[1] - '0') * 60 * 60 + static_cast<T>(__TIME__[3] - '0') * 10
         + static_cast<T>(__TIME__[4] - '0') * 60 + static_cast<T>(__TIME__[6] - '0') * 10 + static_cast<T>(__TIME__[7] - '0');
}

template <typename T = std::int_fast32_t, T A = 48271, T B = 0, T M = 2147483647>
class LinearCongruentialGenerator {
public:

    static constexpr T a = A;
    static constexpr T b = B;
    static constexpr T m = M;
    static constexpr T min = 0;
    static constexpr T max = M;

    constexpr LinearCongruentialGenerator() = default;
    constexpr LinearCongruentialGenerator(T seed) : x(seed){};

    constexpr auto operator()() {
        // Stephen K. Park & Keith W. Miller
        // https://c-faq.com/lib/rand.html
        x = a * (x % q) - r * (x / q);
        x = x > 0 ? x : x + m;
        return x;
    }

private:

    T x;
    static constexpr T q = M / A;
    static constexpr T r = M % A;
};

}  // namespace coex::random
