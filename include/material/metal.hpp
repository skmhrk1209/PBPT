#pragma once

#include <complex>
#include <optional>

#include "math.hpp"
#include "tensor.hpp"
#include "utility.hpp"

namespace coex::material {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
class Metal {
public:

    constexpr Metal() = default;
    constexpr Metal(const Vector<std::complex<Scalar>, 3> &reflactive_index) : m_reflactive_index(reflactive_index) {}
    constexpr Metal(Vector<std::complex<Scalar>, 3> &&reflactive_index) : m_reflactive_index(std::move(reflactive_index)) {}

    constexpr auto &reflactive_index() & { return m_reflactive_index; }
    constexpr const auto &reflactive_index() const & { return m_reflactive_index; }
    constexpr auto &&reflactive_index() && { return std::move(m_reflactive_index); }
    constexpr const auto &&reflactive_index() const && { return std::move(m_reflactive_index); }

    constexpr auto operator()(const auto &ray, const auto &normal, auto &generator) const {
        return [this, &out_position = ray.position(), out_direction = -ray.direction(), &ray, &normal]() {
            /****************************************************************
             * Importance Sampling for Monte Carlo
             ****************************************************************/
            auto sampler = [&normal](const auto &out_position, const auto &out_direction) { return reflect(out_direction, normal); };
            /****************************************************************
             * Rendering Equation
             * Lo := E(wi ~ CDF(wi))[BRDF(x, wi, wo) * Li * (wi · n) / PDF(wi)]
             ****************************************************************/
            auto shader = [&reflactive_index = m_reflactive_index, &normal](const auto &out_position, const auto &out_direction,
                                                                            const auto &in_direction) -> Vector<Scalar, 3> {
                auto complex_reflectance = coex::math::square((1.0 - reflactive_index) / (1.0 + reflactive_index));
                auto specular_reflectance = [&]<auto... Is>(std::index_sequence<Is...>)->Vector<Scalar, 3> {
                    return {coex::math::abs(coex::tensor::get<Is>(complex_reflectance))...};
                }
                (std::make_index_sequence<coex::tensor::dimension_v<decltype(complex_reflectance), 0>>{});
                auto fresnel_reflectance = schlick_approx(specular_reflectance, coex::tensor::dot(out_direction, normal));
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

    Vector<std::complex<Scalar>, 3> m_reflactive_index;
};

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto make_metal(auto &&...args) {
    return Material<Scalar, Vector>(Metal<Scalar, Vector>(std::forward<decltype(args)>(args)...));
}

}  // namespace coex::material
