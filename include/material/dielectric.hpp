#pragma once

#include <optional>

#include "material.hpp"
#include "random.hpp"
#include "tensor.hpp"
#include "utility.hpp"

namespace coex::material {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
struct Dielectric {
    constexpr Dielectric() = default;
    constexpr Dielectric(Scalar refractive_index) : m_refractive_index(refractive_index) {}

    constexpr auto &refractive_index() & { return m_refractive_index; }
    constexpr const auto &refractive_index() const & { return m_refractive_index; }
    constexpr auto &&refractive_index() && { return std::move(m_refractive_index); }
    constexpr const auto &&refractive_index() const && { return std::move(m_refractive_index); }

    constexpr auto operator()(const auto &ray, const auto &normal, auto &generator) const {
        return [&, &out_position = ray.position(), out_direction = -ray.direction()]() constexpr {
            auto cos_theta = coex::tensor::dot(out_direction, normal);
            auto oriented_normal = cos_theta > 0 ? normal : -normal;
            auto refractive_index = cos_theta > 0 ? m_refractive_index : 1.0 / m_refractive_index;
            return [&, &normal = oriented_normal]() constexpr {
                auto specular_reflectance = coex::math::square((1.0 - refractive_index) / (1.0 + refractive_index));
                auto fresnel_reflectance = schlick_approx(specular_reflectance, std::abs(cos_theta));
                auto sin_theta = coex::math::sqrt(1.0 - coex::math::square(cos_theta));
                if (sin_theta > refractive_index || coex::random::uniform(generator, 0.0, 1.0) < fresnel_reflectance) {
                    /****************************************************************
                     * Importance Sampling for Monte Carlo
                     ****************************************************************/
                    auto sampler = [&](const auto &out_position, const auto &out_direction) constexpr { return reflect(out_direction, normal); };
                    /****************************************************************
                     * Rendering Equation
                     * Lo := E(wi ~ CDF(wi))[BRDF(x, wi, wo) * Li * (wi · n) / PDF(wi)]
                     ****************************************************************/
                    auto shader = [&](const auto &out_position, const auto &out_direction, const auto &in_direction) constexpr -> Vector<Scalar, 3> {
                        return {fresnel_reflectance, fresnel_reflectance, fresnel_reflectance};
                    };
                    auto in_position = out_position + numbers::epsilon * normal;
                    auto in_direction = sampler(out_position, out_direction);
                    auto reflectance = shader(out_position, out_direction, in_direction);
                    auto weight = 1.0 / fresnel_reflectance;
                    auto reflected_ray = std::decay_t<decltype(ray)>(std::move(in_position), std::move(in_direction), weight);
                    return std::make_tuple(reflectance * ray.weight(), std::make_optional(std::move(reflected_ray)));
                } else {
                    /****************************************************************
                     * Importance Sampling for Monte Carlo
                     ****************************************************************/
                    auto sampler = [&](const auto &out_position, const auto &out_direction) constexpr {
                        return refract(out_direction, normal, refractive_index);
                    };
                    /****************************************************************
                     * Rendering Equation
                     * Lo := E(wi ~ CDF(wi))[BRDF(x, wi, wo) * Li * (wi · n) / PDF(wi)]
                     ****************************************************************/
                    auto shader = [&](const auto &out_position, const auto &out_direction, const auto &in_direction) constexpr -> Vector<Scalar, 3> {
                        auto transmittance = (1.0 - fresnel_reflectance) * coex::math::square(refractive_index);
                        return {transmittance, transmittance, transmittance};
                    };
                    auto in_position = out_position - numbers::epsilon * normal;
                    auto in_direction = sampler(out_position, out_direction);
                    auto transmittance = shader(out_position, out_direction, in_direction);
                    auto weight = 1.0 / (1.0 - fresnel_reflectance);
                    auto transmitted_ray = std::decay_t<decltype(ray)>(std::move(in_position), std::move(in_direction), weight);
                    return std::make_tuple(transmittance * ray.weight(), std::make_optional(std::move(transmitted_ray)));
                }
            }();
        }();
    }

private:

    Scalar m_refractive_index;
};

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto make_dielectric(auto &&...args) {
    return Material<Scalar, Vector>(Dielectric<Scalar, Vector>(std::forward<decltype(args)>(args)...));
}

}  // namespace coex::material
