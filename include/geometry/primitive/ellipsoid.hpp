#pragma once

#include <algorithm>
#include <optional>
#include <queue>
#include <utility>

#include "../occupation.hpp"
#include "material.hpp"
#include "math.hpp"
#include "tensor.hpp"

namespace coex::geometry::primitive {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, template <typename, auto> typename> typename Material = coex::material::Material>
struct Ellipsoid {
    constexpr Ellipsoid() = default;
    constexpr Ellipsoid(const Vector<Scalar, 3> &radii, const Material<Scalar, Vector> &material) : m_radii(radii), m_material(material) {}
    constexpr Ellipsoid(Vector<Scalar, 3> &&radii, Material<Scalar, Vector> &&material)
        : m_radii(std::move(radii)),
          m_material(std::move(material)) {}

    constexpr auto &radii() & { return m_radii; }
    constexpr const auto &radii() const & { return m_radii; }
    constexpr auto &&radii() && { return std::move(m_radii); }
    constexpr const auto &&radii() const && { return std::move(m_radii); }

    constexpr auto &material() & { return m_material; }
    constexpr const auto &material() const & { return m_material; }
    constexpr auto &&material() && { return std::move(m_material); }
    constexpr const auto &&material() const && { return std::move(m_material); }

    constexpr auto intersect(const auto &ray) const {
        using NormalEvaluator = std::function<Vector<Scalar, 3>(const Vector<Scalar, 3> &)>;
        using MaterialReference = std::reference_wrapper<const Material<Scalar, Vector>>;
        using OccupationType = Occupation<Scalar, NormalEvaluator, MaterialReference>;
        using OccupationQueue = std::priority_queue<OccupationType, std::vector<OccupationType>, OccupationComparator>;

        auto ellipsoid_position = [&]() constexpr -> std::optional<std::pair<Scalar, Scalar>> {
            auto norm_ray_position = ray.position() / m_radii;
            auto norm_ray_direction = ray.direction() / m_radii;

            auto A = coex::tensor::dot(norm_ray_direction, norm_ray_direction);
            auto B = coex::tensor::dot(norm_ray_direction, norm_ray_position);
            auto C = coex::tensor::dot(norm_ray_position, norm_ray_position);
            auto D = B * B - A * C + A;

            if (D >= 0) {
                auto min_distance = (-B - coex::math::sqrt(D)) / A;
                auto max_distance = (-B + coex::math::sqrt(D)) / A;
                return std::make_pair(min_distance, max_distance);
            }
            return {};
        };
        auto ellipsoid_normal = [this](const auto &position) constexpr -> Vector<Scalar, 3> {
            return coex::tensor::normalized(position / coex::tensor::elemwise(coex::math::square<Scalar>, m_radii));
        };

        OccupationQueue occupations;
        if (auto intersection = ellipsoid_position()) {
            auto [min_distance, max_distance] = intersection.value();
            if (max_distance > 0.0) {
                Surface<NormalEvaluator, MaterialReference> surface(ellipsoid_normal, std::cref(material()));
                Intersection<Scalar, NormalEvaluator, MaterialReference> min_intersection(min_distance, surface);
                Intersection<Scalar, NormalEvaluator, MaterialReference> max_intersection(max_distance, surface);
                occupations.emplace(std::move(min_intersection), std::move(max_intersection));
            }
        }
        return occupations;
    }

private:

    Vector<Scalar, 3> m_radii;
    Material<Scalar, Vector> m_material;
};

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, template <typename, auto> typename> typename Material = coex::material::Material>
constexpr auto make_ellipsoid(auto &&...args) {
    return Ellipsoid<Scalar, Vector, Material>(std::forward<decltype(args)>(args)...);
}

}  // namespace coex::geometry::primitive
