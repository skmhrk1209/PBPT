#pragma once

#include <numbers>

#include "distributions.hpp"
#include "math.hpp"
#include "tensor.hpp"

namespace coex::random {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto uniform_on_unit_circle(auto &generator) -> Vector<Scalar, 2> {
    auto theta = coex::random::uniform(generator, -std::numbers::pi, std::numbers::pi);
    return {std::cos(theta), std::sin(theta)};
}

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto uniform_in_unit_circle(auto &generator) {
    auto radius = coex::math::sqrt(coex::random::uniform(generator, 0.0, 1.0));
    return uniform_on_unit_circle<Scalar, Vector>(generator) * radius;
}

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto uniform_on_unit_sphere(auto &generator) -> Vector<Scalar, 3> {
    auto cos_theta = coex::random::uniform(generator, -1.0, 1.0);
    auto sin_theta = coex::math::sqrt(1.0 - coex::math::square(cos_theta));
    auto phi = coex::random::uniform(generator, -std::numbers::pi, std::numbers::pi);
    return {sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
}

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto uniform_in_unit_sphere(auto &generator) {
    auto radius = coex::math::cbrt(coex::random::uniform(generator, 0.0, 1.0));
    return uniform_on_unit_sphere<Scalar, Vector>(generator) * radius;
}

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto uniform_on_unit_semisphere(auto &generator) -> Vector<Scalar, 3> {
    auto cos_theta = coex::random::uniform(generator, 0.0, 1.0);
    auto sin_theta = coex::math::sqrt(1.0 - coex::math::square(cos_theta));
    auto phi = coex::random::uniform(generator, -std::numbers::pi, std::numbers::pi);
    return {sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
}

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto cosine_on_unit_semisphere(auto &generator) -> Vector<Scalar, 3> {
    auto cos_theta = coex::math::sqrt(coex::random::uniform(generator, 0.0, 1.0));
    auto sin_theta = coex::math::sqrt(1.0 - coex::math::square(cos_theta));
    auto phi = coex::random::uniform(generator, -std::numbers::pi, std::numbers::pi);
    return {sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta};
}

}  // namespace coex::random
