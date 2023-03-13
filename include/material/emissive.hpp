#pragma once

#include <optional>

#include "material.hpp"
#include "tensor.hpp"

namespace coex::material {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
struct Emissive {
    constexpr Emissive() = default;
    constexpr Emissive(const Vector<Scalar, 3> &emission) : m_emission(emission) {}
    constexpr Emissive(Vector<Scalar, 3> &&emission) : m_emission(std::move(emission)) {}

    constexpr auto &emission() & { return m_emission; }
    constexpr const auto &emission() const & { return m_emission; }
    constexpr auto &&emission() && { return std::move(m_emission); }
    constexpr const auto &&emission() const && { return std::move(m_emission); }

    constexpr auto operator()(const auto &ray, const auto &normal, auto &generator) const {
        return [&, &out_position = ray.position(), out_direction = -ray.direction()]() constexpr {
            /****************************************************************
             * Rendering Equation
             * Lo := E(wi ~ CDF(wi))[BRDF(x, wi, wo) * Li * (wi · n) / PDF(wi)]
             ****************************************************************/
            auto emitter = [&](const auto &out_position, const auto &out_direction) constexpr {
                return m_emission * coex::tensor::dot(out_direction, normal);
            };
            auto emittance = emitter(out_position, out_direction);
            return std::make_tuple(emittance * ray.weight(), std::optional<std::decay_t<decltype(ray)>>{});
        }();
    }

private:

    Vector<Scalar, 3> m_emission;
};

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto make_emissive(auto &&...args) {
    return Material<Scalar, Vector>(Emissive<Scalar, Vector>(std::forward<decltype(args)>(args)...));
}

}  // namespace coex::material
