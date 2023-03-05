#pragma once

#include "common.hpp"
#include "random.hpp"
#include "ray.hpp"
#include "tensor.hpp"

namespace coex::optics {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, auto, auto> typename Matrix = coex::tensor::Matrix,
          typename ResponseFunction = decltype([](const Vector<Scalar, 3> &, const Vector<Scalar, 3> &) -> Scalar {})>
class Camera {
public:

    constexpr Camera() = default;

    constexpr Camera(Scalar vertical_fov, Scalar aspect_ratio, Scalar focal_distance, Scalar aperture_radius, ResponseFunction response_function,
                     const Vector<Scalar, 3> &position, const Matrix<Scalar, 3, 3> &orientation)
        : m_vertical_fov(vertical_fov),
          m_aspect_ratio(aspect_ratio),
          m_focal_distance(focal_distance),
          m_aperture_radius(aperture_radius),
          m_response_function(response_function),
          m_position(position),
          m_orientation(orientation) {}

    constexpr Camera(Scalar vertical_fov, Scalar aspect_ratio, Scalar focal_distance, Scalar aperture_radius, ResponseFunction response_function,
                     Vector<Scalar, 3> &&position, Matrix<Scalar, 3, 3> &&orientation)
        : m_vertical_fov(vertical_fov),
          m_aspect_ratio(aspect_ratio),
          m_focal_distance(focal_distance),
          m_aperture_radius(aperture_radius),
          m_response_function(response_function),
          m_position(std::move(position)),
          m_orientation(std::move(orientation)) {}

    constexpr auto &vertical_fov() & { return m_vertical_fov; }
    constexpr const auto &vertical_fov() const & { return m_vertical_fov; }
    constexpr auto &&vertical_fov() && { return std::move(m_vertical_fov); }
    constexpr const auto &&vertical_fov() const && { return std::move(m_vertical_fov); }

    constexpr auto &aspect_ratio() & { return m_aspect_ratio; }
    constexpr const auto &aspect_ratio() const & { return m_aspect_ratio; }
    constexpr auto &&aspect_ratio() && { return std::move(m_aspect_ratio); }
    constexpr const auto &&aspect_ratio() const && { return std::move(m_aspect_ratio); }

    constexpr auto &focal_distance() & { return m_focal_distance; }
    constexpr const auto &focal_distance() const & { return m_focal_distance; }
    constexpr auto &&focal_distance() && { return std::move(m_focal_distance); }
    constexpr const auto &&focal_distance() const && { return std::move(m_focal_distance); }

    constexpr auto &aperture_radius() & { return m_aperture_radius; }
    constexpr const auto &aperture_radius() const & { return m_aperture_radius; }
    constexpr auto &&aperture_radius() && { return std::move(m_aperture_radius); }
    constexpr const auto &&aperture_radius() const && { return std::move(m_aperture_radius); }

    constexpr auto &response_function() & { return m_response_function; }
    constexpr const auto &response_function() const & { return m_response_function; }
    constexpr auto &&response_function() && { return std::move(m_response_function); }
    constexpr const auto &&response_function() const && { return std::move(m_response_function); }

    constexpr auto &position() & { return m_position; }
    constexpr const auto &position() const & { return m_position; }
    constexpr auto &&position() && { return std::move(m_position); }
    constexpr const auto &&position() const && { return std::move(m_position); }

    constexpr auto &orientation() & { return m_orientation; }
    constexpr const auto &orientation() const & { return m_orientation; }
    constexpr auto &&orientation() && { return std::move(m_orientation); }
    constexpr const auto &&orientation() const && { return std::move(m_orientation); }

    constexpr auto ray(auto coord_u, auto coord_v, auto &generator) const -> Ray<Scalar, Vector> {
        // ---------------- screen ---------------- //
        auto screen_height = 2.0 * std::tan(m_vertical_fov / 2.0);
        auto screen_width = screen_height * m_aspect_ratio;
        auto coord_x = coex::math::lerp(coord_u, 0.0, 1.0, -screen_width / 2.0, screen_width / 2.0);
        auto coord_y = coex::math::lerp(coord_v, 0.0, 1.0, -screen_height / 2.0, screen_height / 2.0);
        // ---------------- defocus ---------------- //
        auto target = m_position + m_orientation % Vector<Scalar, 3>{coord_x, coord_y, 1.0} * m_focal_distance;
        auto [offset_x, offset_y] = coex::random::uniform_in_unit_circle<Scalar, Vector>(generator) * m_aperture_radius;
        auto in_position = m_position + m_orientation % Vector<Scalar, 3>{offset_x, offset_y, 0.0};
        auto in_direction = coex::tensor::normalized(target - in_position);
        // ---------------- sampling ---------------- //
        auto lens_normal = m_orientation % Vector<Scalar, 3>{0.0, 0.0, 1.0};
        auto response = m_response_function(in_direction, lens_normal);
        auto cos_theta = coex::tensor::dot(in_direction, lens_normal);
        auto weight = response * coex::math::pow(cos_theta, 4);
        return {std::move(in_position), std::move(in_direction), weight};
    }

private:

    Scalar m_vertical_fov;
    Scalar m_aspect_ratio;
    Scalar m_focal_distance;
    Scalar m_aperture_radius;
    ResponseFunction m_response_function;
    Vector<Scalar, 3> m_position;
    Matrix<Scalar, 3, 3> m_orientation;
};

}  // namespace coex::optics
