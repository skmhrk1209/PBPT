#pragma once

#include <algorithm>
#include <optional>
#include <queue>
#include <utility>

#include "../occupation.hpp"
#include "material.hpp"
#include "math.hpp"
#include "tensor.hpp"

namespace pbpt::geometry::primitive {

template <
    typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector,
    template <typename, template <typename, auto> typename> typename Material = pbpt::material::Material>
struct Cylinder {
  constexpr Cylinder() = default;
  constexpr Cylinder(Scalar height, const Vector<Scalar, 2> &radii, const Material<Scalar, Vector> &material)
      : m_height(height), m_radii(radii), m_material(material) {}
  constexpr Cylinder(Scalar height, Vector<Scalar, 2> &&radii, Material<Scalar, Vector> &&material)
      : m_height(height), m_radii(std::move(radii)), m_material(std::move(material)) {}

  constexpr auto &height() & { return m_height; }
  constexpr const auto &height() const & { return m_height; }
  constexpr auto &&height() && { return std::move(m_height); }
  constexpr const auto &&height() const && { return std::move(m_height); }

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

    auto circle_position = [&](auto height) constexpr -> std::optional<Scalar> {
      if (ray_direction_y) return (height - ray_position_y) / ray_direction_y;
      return {};
    };
    auto circle_normal = [this](const auto &position) constexpr -> Vector<Scalar, 3> {
      auto [position_x, position_y, position_z] = position;
      return {0.0, position_y > 0.0 ? 1.0 : -1.0, 0.0};
    };

    auto cylinder_position = [&]() constexpr -> std::optional<std::pair<Scalar, Scalar>> {
      auto norm_ray_position = Vector<Scalar, 2>{ray_position_z, ray_position_x} / m_radii;
      auto norm_ray_direction = Vector<Scalar, 2>{ray_direction_z, ray_direction_x} / m_radii;

      auto A = pbpt::tensor::dot(norm_ray_direction, norm_ray_direction);
      auto B = pbpt::tensor::dot(norm_ray_direction, norm_ray_position);
      auto C = pbpt::tensor::dot(norm_ray_position, norm_ray_position);
      auto D = B * B - A * C + A;

      if (D >= 0) {
        auto min_distance = (-B - pbpt::math::sqrt(D)) / A;
        auto max_distance = (-B + pbpt::math::sqrt(D)) / A;
        return std::make_pair(min_distance, max_distance);
      }
      return {};
    };
    auto cylinder_normal = [this](const auto &position) constexpr -> Vector<Scalar, 3> {
      auto [position_x, position_y, position_z] = position;
      auto [normal_z, normal_x] = pbpt::tensor::normalized(
          Vector<Scalar, 2>{position_z, position_x} / pbpt::tensor::elemwise(pbpt::math::square<Scalar>, m_radii)
      );
      return Vector<Scalar, 3>{normal_x, 0.0, normal_z};
    };

    OccupationQueue occupations;
    if (auto intersection = cylinder_position()) {
      auto [min_distance, max_distance] = intersection.value();
      auto [min_intersection_x, min_intersection_y, min_intersection_z] = ray.at(min_distance);
      auto [max_intersection_x, max_intersection_y, max_intersection_z] = ray.at(max_distance);
      if (-m_height <= min_intersection_y && min_intersection_y <= m_height) {
        if (-m_height <= max_intersection_y && max_intersection_y <= m_height) {
          if (max_distance > 0.0) {
            Surface<NormalEvaluator, MaterialReference> surface(cylinder_normal, std::cref(material()));
            Intersection<Scalar, NormalEvaluator, MaterialReference> min_intersection(min_distance, surface);
            Intersection<Scalar, NormalEvaluator, MaterialReference> max_intersection(max_distance, surface);
            occupations.emplace(std::move(min_intersection), std::move(max_intersection));
          }
        } else if (auto intersection = circle_position(ray_direction_y > 0 ? m_height : -m_height)) {
          auto max_distance = intersection.value();
          if (max_distance > 0.0) {
            auto [max_intersection_x, max_intersection_y, max_intersection_z] = ray.at(max_distance);
            auto max_intersection = Vector<Scalar, 2>{max_intersection_z, max_intersection_x};
            auto norm_max_intersection = max_intersection / m_radii;
            if (pbpt::tensor::dot(norm_max_intersection, norm_max_intersection) <= 1.0) {
              Surface<NormalEvaluator, MaterialReference> min_surface(cylinder_normal, std::cref(material()));
              Surface<NormalEvaluator, MaterialReference> max_surface(circle_normal, std::cref(material()));
              Intersection<Scalar, NormalEvaluator, MaterialReference> min_intersection(min_distance, min_surface);
              Intersection<Scalar, NormalEvaluator, MaterialReference> max_intersection(max_distance, max_surface);
              occupations.emplace(std::move(min_intersection), std::move(max_intersection));
            }
          }
        }
      } else if (-m_height <= max_intersection_y && max_intersection_y <= m_height) {
        if (max_distance > 0.0) {
          if (auto intersection = circle_position(ray_direction_y > 0 ? -m_height : m_height)) {
            auto min_distance = intersection.value();
            auto [min_intersection_x, min_intersection_y, min_intersection_z] = ray.at(min_distance);
            auto min_intersection = Vector<Scalar, 2>{min_intersection_z, min_intersection_x};
            auto norm_min_intersection = min_intersection / m_radii;
            if (pbpt::tensor::dot(norm_min_intersection, norm_min_intersection) <= 1.0) {
              Surface<NormalEvaluator, MaterialReference> min_surface(circle_normal, std::cref(material()));
              Surface<NormalEvaluator, MaterialReference> max_surface(cylinder_normal, std::cref(material()));
              Intersection<Scalar, NormalEvaluator, MaterialReference> min_intersection(min_distance, min_surface);
              Intersection<Scalar, NormalEvaluator, MaterialReference> max_intersection(max_distance, max_surface);
              occupations.emplace(std::move(min_intersection), std::move(max_intersection));
            }
          }
        }
      }
    } else {
      if (auto intersection = circle_position(ray_direction_y > 0 ? m_height : -m_height)) {
        auto max_distance = intersection.value();
        if (max_distance > 0.0) {
          auto [max_intersection_x, max_intersection_y, max_intersection_z] = ray.at(max_distance);
          auto max_intersection = Vector<Scalar, 2>{max_intersection_z, max_intersection_x};
          auto norm_max_intersection = max_intersection / m_radii;
          if (pbpt::tensor::dot(norm_max_intersection, norm_max_intersection) <= 1.0) {
            if (auto intersection = circle_position(ray_direction_y > 0 ? -m_height : m_height)) {
              auto min_distance = intersection.value();
              auto [min_intersection_x, min_intersection_y, min_intersection_z] = ray.at(min_distance);
              auto min_intersection = Vector<Scalar, 2>{min_intersection_z, min_intersection_x};
              auto norm_min_intersection = min_intersection / m_radii;
              if (pbpt::tensor::dot(norm_min_intersection, norm_min_intersection) <= 1.0) {
                Surface<NormalEvaluator, MaterialReference> surface(circle_normal, std::cref(material()));
                Intersection<Scalar, NormalEvaluator, MaterialReference> min_intersection(min_distance, surface);
                Intersection<Scalar, NormalEvaluator, MaterialReference> max_intersection(max_distance, surface);
                occupations.emplace(std::move(min_intersection), std::move(max_intersection));
              }
            }
          }
        }
      }
    }
    return occupations;
  }

 private:
  Scalar m_height;
  Vector<Scalar, 2> m_radii;
  Material<Scalar, Vector> m_material;
};

template <
    typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector,
    template <typename, template <typename, auto> typename> typename Material = pbpt::material::Material>
constexpr auto make_cylinder(auto &&...args) {
  return Cylinder<Scalar, Vector, Material>(std::forward<decltype(args)>(args)...);
}

}  // namespace pbpt::geometry::primitive
