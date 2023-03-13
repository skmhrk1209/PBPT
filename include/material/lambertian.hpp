#pragma once

#include <numbers>
#include <optional>

#include "material.hpp"
#include "random.hpp"
#include "tensor.hpp"

namespace coex::material {

template <typename Scalar, template <typename, auto> typename Vector = coex::tensor::Vector>
struct Lambertian {
    constexpr Lambertian() = default;
    constexpr Lambertian(const Vector<Scalar, 3> &reflectance) : m_reflectance(reflectance) {}
    constexpr Lambertian(Vector<Scalar, 3> &&reflectance) : m_reflectance(std::move(reflectance)) {}

    constexpr auto &reflectance() & { return m_reflectance; }
    constexpr const auto &reflectance() const & { return m_reflectance; }
    constexpr auto &&reflectance() && { return std::move(m_reflectance); }
    constexpr const auto &&reflectance() const && { return std::move(m_reflectance); }

    constexpr auto operator()(const auto &ray, const auto &normal, auto &generator) const {
        return [&, &out_position = ray.position(), out_direction = -ray.direction()]() constexpr {
            /****************************************************************
             * BRDF (Bidirectional Reflectance Distribution Fucntion)
             ****************************************************************/
            auto brdf = [&](const auto &out_position, const auto &out_direction, const auto &in_direction) constexpr {
                return m_reflectance / std::numbers::pi;
            };
            /****************************************************************
             * PDF (Probability Density Function) for Importance Sampling
             ****************************************************************/
            auto pdf = [&](const auto &in_direction) constexpr { return coex::tensor::dot(in_direction, normal) / std::numbers::pi; };
            /****************************************************************
             * Importance Sampling for Monte Carlo
             ****************************************************************/
            auto sampler = [&](const auto &out_position, const auto &out_direction) constexpr {
                auto [coord_x, coord_y, coord_z] = coex::random::cosine_on_unit_semisphere<Scalar, Vector>(generator);
                auto tangent = coex::tensor::normalized(coex::tensor::cross(normal, Vector<Scalar, 3>{1.0, 0.0, 0.0}));
                auto bitangent = coex::tensor::cross(normal, tangent);
                return coord_x * tangent + coord_y * bitangent + coord_z * normal;
            };
            /****************************************************************
             * Rendering Equation
             * Lo := E(wi ~ CDF(wi))[BRDF(x, wi, wo) * Li * (wi · n) / PDF(wi)]
             ****************************************************************/
            auto shader = [&](const auto &out_position, const auto &out_direction, const auto &in_direction) constexpr {
                return brdf(out_position, out_direction, in_direction) * coex::tensor::dot(in_direction, normal);
            };
            auto in_position = out_position + numbers::epsilon * normal;
            auto in_direction = sampler(out_position, out_direction);
            auto reflectance = shader(out_position, out_direction, in_direction);
            auto weight = 1.0 / pdf(in_direction);
            auto reflected_ray = std::decay_t<decltype(ray)>(std::move(in_position), std::move(in_direction), weight);
            return std::make_tuple(reflectance * ray.weight(), std::make_optional(std::move(reflected_ray)));
        }();
    }

private:

    Vector<Scalar, 3> m_reflectance;
};

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto make_lambertian(auto &&...args) {
    return Material<Scalar, Vector>(Lambertian<Scalar, Vector>(std::forward<decltype(args)>(args)...));
}

}  // namespace coex::material
