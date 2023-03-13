#pragma once

#include <algorithm>
#include <numbers>
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
struct Plane {
    constexpr Plane() = default;
    constexpr Plane(const Vector<Scalar, 2> &radii, const Material<Scalar, Vector> &material) : m_radii(radii), m_material(material) {}
    constexpr Plane(Vector<Scalar, 2> &&radii, Material<Scalar, Vector> &&material) : m_radii(std::move(radii)), m_material(std::move(material)) {}

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

        auto [ray_position_x, ray_position_y, ray_position_z] = ray.position();
        auto [ray_direction_x, ray_direction_y, ray_direction_z] = ray.direction();

        auto plane_position = [&]() constexpr -> std::optional<Scalar> {
            if (ray_direction_y) return -ray_position_y / ray_direction_y;
            return {};
        };
        auto plane_normal = [this](const auto &position) constexpr -> Vector<Scalar, 3> { return {0.0, -1.0, 0.0}; };

        OccupationQueue occupations;
        if (auto intersection = plane_position()) {
            auto distance = intersection.value();
            if (distance > 0.0) {
                auto [intersection_x, intersection_y, intersection_z] = ray.at(distance);
                auto [depth, width] = m_radii;
                if ((-depth <= intersection_z) && (intersection_z <= depth)) {
                    if ((-width <= intersection_x) && (intersection_x <= width)) {
                        Surface<NormalEvaluator, MaterialReference> surface(plane_normal, std::cref(material()));
                        Intersection<Scalar, NormalEvaluator, MaterialReference> min_intersection(distance, surface);
                        Intersection<Scalar, NormalEvaluator, MaterialReference> max_intersection(distance, surface);
                        occupations.emplace(std::move(min_intersection), std::move(max_intersection));
                    }
                }
            }
        }
        return occupations;
    }

private:

    Vector<Scalar, 2> m_radii;
    Material<Scalar, Vector> m_material;
};

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, template <typename, auto> typename> typename Material = coex::material::Material>
constexpr auto make_plane(auto &&...args) {
    return Plane<Scalar, Vector, Material>(std::forward<decltype(args)>(args)...);
}

}  // namespace coex::geometry::primitive
