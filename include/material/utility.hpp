#pragma once

#include <numbers>

#include "math.hpp"
#include "tensor.hpp"

namespace coex::material {

constexpr auto reflect(const auto &incident, const auto &normal) { return 2.0 * coex::tensor::dot(incident, normal) * normal - incident; }

constexpr auto refract(const auto &incident, const auto &normal, auto refractive_index) {
    auto parallel = (coex::tensor::dot(incident, normal) * normal - incident) / refractive_index;
    auto perpendicular = -coex::math::sqrt(1.0 - coex::tensor::dot(parallel, parallel)) * normal;
    return parallel + perpendicular;
}

constexpr auto schlick_approx(auto specular_reflectance, auto cos_theta) {
    return specular_reflectance + (1.0 - specular_reflectance) * coex::math::pow(1.0 - cos_theta, 5);
}

namespace numbers {
inline constexpr auto epsilon = 1e-6;
}

}  // namespace coex::material
