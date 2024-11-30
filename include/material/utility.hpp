#pragma once

#include <numbers>

#include "math.hpp"
#include "tensor.hpp"

namespace pbpt::material {

constexpr auto reflect(const auto &incident, const auto &normal) { return 2.0 * pbpt::tensor::dot(incident, normal) * normal - incident; }

constexpr auto refract(const auto &incident, const auto &normal, auto refractive_index) {
    auto parallel = (pbpt::tensor::dot(incident, normal) * normal - incident) / refractive_index;
    auto perpendicular = -pbpt::math::sqrt(1.0 - pbpt::tensor::dot(parallel, parallel)) * normal;
    return parallel + perpendicular;
}

constexpr auto schlick_approx(auto specular_reflectance, auto cos_theta) {
    return specular_reflectance + (1.0 - specular_reflectance) * pbpt::math::pow(1.0 - cos_theta, 5);
}

namespace numbers {
inline constexpr auto epsilon = 1e-6;
}

}  // namespace pbpt::material
