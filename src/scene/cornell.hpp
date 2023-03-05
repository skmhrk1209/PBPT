
#include <numbers>

#include "geometry.hpp"
#include "material.hpp"
#include "math.hpp"
#include "optics.hpp"
#include "random.hpp"
#include "tensor.hpp"

using Scalar = double;

namespace coex::scene::cornell {

inline constexpr auto object = []() constexpr {
    using namespace std::literals::complex_literals;
    return coex::geometry::make_unionizer(
        // light
        coex::geometry::make_translator(
            coex::geometry::make_sphere<>(1e3, coex::material::make_emissive<>(coex::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0})),
            coex::tensor::Vector<Scalar, 3>{0.0, -1e3 - 1.0 + 1e-4, 0.0}),
        coex::geometry::make_unionizer(
            // front wall
            coex::geometry::make_translator(
                coex::geometry::make_sphere<>(1e6, coex::material::make_lambertian<>(coex::tensor::Vector<Scalar, 3>{0.75, 0.75, 0.75})),
                coex::tensor::Vector<Scalar, 3>{0.0, 0.0, 1e6 + 1.0}),
            coex::geometry::make_unionizer(
                // top wall
                coex::geometry::make_translator(
                    coex::geometry::make_sphere<>(1e6, coex::material::make_lambertian<>(coex::tensor::Vector<Scalar, 3>{0.75, 0.75, 0.75})),
                    coex::tensor::Vector<Scalar, 3>{0.0, -1e6 - 1.0, 0.0}),
                coex::geometry::make_unionizer(
                    // bottom wall
                    coex::geometry::make_translator(
                        coex::geometry::make_sphere<>(1e6, coex::material::make_lambertian<>(coex::tensor::Vector<Scalar, 3>{0.75, 0.75, 0.75})),
                        coex::tensor::Vector<Scalar, 3>{0.0, 1e6 + 1.0, 0.0}),
                    coex::geometry::make_unionizer(
                        // left wall
                        coex::geometry::make_translator(
                            coex::geometry::make_sphere<>(1e6, coex::material::make_lambertian<>(coex::tensor::Vector<Scalar, 3>{0.75, 0.25, 0.25})),
                            coex::tensor::Vector<Scalar, 3>{-1e6 - 1.0, 0.0, 0.0}),
                        // right wall
                        coex::geometry::make_translator(
                            coex::geometry::make_sphere<>(1e6, coex::material::make_lambertian<>(coex::tensor::Vector<Scalar, 3>{0.25, 0.25, 0.75})),
                            coex::tensor::Vector<Scalar, 3>{1e6 + 1.0, 0.0, 0.0}))))));
}();

inline constexpr auto camera = []() constexpr {
    auto vertical_fov = 60.0 / 180.0 * std::numbers::pi;
    auto aspect_ratio = 1.0;
    auto focal_distance = 1.0;
    auto aperture_radius = 0.0;
    auto response_function = [](const auto &in_direction, const auto &lens_normal) {
        auto cos_theta = coex::tensor::dot(in_direction, lens_normal);
        return 1.0 / coex::math::pow(cos_theta, 4);
    };

    coex::tensor::Vector<Scalar, 3> position{0.0, 0.0, -3.0};
    coex::tensor::Vector<Scalar, 3> target{0.0, 0.0, 0.0};
    coex::tensor::Vector<Scalar, 3> down{0.0, 1.0, 0.0};

    auto w = coex::tensor::normalized(target - position);
    auto u = coex::tensor::normalized(coex::tensor::cross(down, w));
    auto v = coex::tensor::cross(w, u);
    auto rotation = coex::tensor::transposed(coex::tensor::Matrix<Scalar, 3, 3>{u, v, w});

    return coex::optics::Camera<Scalar, coex::tensor::Vector, coex::tensor::Matrix, decltype(response_function)>(
        vertical_fov, aspect_ratio, focal_distance, aperture_radius, response_function, position, rotation);
}();

inline constexpr auto background = [](const auto &ray) constexpr -> coex::tensor::Vector<Scalar, 3> { return {}; };

}  // namespace coex::scene::cornell
