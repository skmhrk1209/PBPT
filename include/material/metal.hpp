#pragma once

#include <complex>
#include <optional>

#include "math.hpp"
#include "tensor.hpp"
#include "utility.hpp"

namespace pbpt::material {

template <typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector>
struct Metal {
    constexpr Metal() = default;
    constexpr Metal(const Vector<std::complex<Scalar>, 3> &refractive_index) : m_refractive_index(refractive_index) {}
    constexpr Metal(Vector<std::complex<Scalar>, 3> &&refractive_index) : m_refractive_index(std::move(refractive_index)) {}

    constexpr auto &refractive_index() & { return m_refractive_index; }
    constexpr const auto &refractive_index() const & { return m_refractive_index; }
    constexpr auto &&refractive_index() && { return std::move(m_refractive_index); }
    constexpr const auto &&refractive_index() const && { return std::move(m_refractive_index); }

    constexpr auto operator()(const auto &ray, const auto &normal, auto &generator) const {
        return [&, &out_position = ray.position(), out_direction = -ray.direction()]() constexpr {
            /****************************************************************
             * Importance Sampling for Monte Carlo
             ****************************************************************/
            auto sampler = [&](const auto &out_position, const auto &out_direction) constexpr { return reflect(out_direction, normal); };
            /****************************************************************
             * Rendering Equation
             * Lo := E(wi ~ CDF(wi))[BRDF(x, wi, wo) * Li * (wi · n) / PDF(wi)]
             ****************************************************************/
            auto shader = [&](const auto &out_position, const auto &out_direction, const auto &in_direction) constexpr -> Vector<Scalar, 3> {
                auto complex_reflectance
                    = pbpt::tensor::elemwise(pbpt::math::square<std::complex<Scalar>>, (1.0 - m_refractive_index) / (1.0 + m_refractive_index));
                auto specular_reflectance = [&]<auto... Is>(std::index_sequence<Is...>)->Vector<Scalar, 3> {
                    return {std::abs(pbpt::tensor::get<Is>(complex_reflectance))...};
                }
                (std::make_index_sequence<pbpt::tensor::dimension_v<decltype(complex_reflectance), 0>>{});
                auto fresnel_reflectance = schlick_approx(specular_reflectance, pbpt::tensor::dot(out_direction, normal));
                return fresnel_reflectance;
            };
            auto in_position = out_position + numbers::epsilon * normal;
            auto in_direction = sampler(out_position, out_direction);
            auto reflectance = shader(out_position, out_direction, in_direction);
            auto reflected_ray = std::decay_t<decltype(ray)>(std::move(in_position), std::move(in_direction), 1.0);
            return std::make_tuple(reflectance * ray.weight(), std::make_optional(std::move(reflected_ray)));
        }();
    }

private:

    Vector<std::complex<Scalar>, 3> m_refractive_index;
};

template <typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector>
constexpr auto make_metal(auto &&...args) {
    return Material<Scalar, Vector>(Metal<Scalar, Vector>(std::forward<decltype(args)>(args)...));
}

}  // namespace pbpt::material
