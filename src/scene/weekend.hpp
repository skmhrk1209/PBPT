
#include <numbers>

#include "geometry.hpp"
#include "material.hpp"
#include "math.hpp"
#include "optics.hpp"
#include "random.hpp"
#include "tensor.hpp"

using Scalar = double;

namespace coex::scene::weekend {

inline constexpr auto object = []() constexpr {
    using namespace std::literals::complex_literals;
    coex::random::LinearCongruentialGenerator<> generator(__LINE__);
    return coex::geometry::csg::make_union(
        // ground sphere
        coex::geometry::transform::make_translation(
            coex::geometry::primitive::make_ellipsoid<>(coex::tensor::Vector<Scalar, 3>{1000.0, 1000.0, 1000.0}, coex::material::make_lambertian<>(coex::tensor::Vector<Scalar, 3>{0.5, 0.5, 0.5})),
            coex::tensor::Vector<Scalar, 3>{0.0, 1000.0, 0.0}),
        coex::geometry::csg::make_union(
            // left sphere (gold)
            coex::geometry::transform::make_translation(
                coex::geometry::primitive::make_ellipsoid<>(coex::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0}, coex::material::make_metal<>(coex::tensor::Vector<std::complex<Scalar>, 3>{
                                                       0.18299 + 3.42420i,
                                                       0.42108 + 2.34590i,
                                                       1.37340 + 1.77040i,
                                                   })),
                coex::tensor::Vector<Scalar, 3>{-4.0, -1.0, 0.0}),
            coex::geometry::csg::make_union(
                // center sphere (glass)
                coex::geometry::transform::make_translation(coex::geometry::primitive::make_ellipsoid<>(coex::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0}, coex::material::make_dielectric<>(1.5)),
                                                coex::tensor::Vector<Scalar, 3>{0.0, -1.0, 0.0}),
                coex::geometry::csg::make_union(
                    // right sphere (platinum)
                    coex::geometry::transform::make_translation(
                        coex::geometry::primitive::make_ellipsoid<>(coex::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0}, coex::material::make_metal<>(coex::tensor::Vector<std::complex<Scalar>, 3>{
                                                               2.37570 + 4.26550i,
                                                               2.08470 + 3.71530i,
                                                               1.84530 + 3.13650i,
                                                           })),
                        coex::tensor::Vector<Scalar, 3>{4.0, -1.0, 0.0}),
                    coex::geometry::csg::make_union(
                        // tiny sphere (scatteing only)
                        [function =
                             [&]<auto I, auto... Is>(auto self, std::index_sequence<I, Is...>) constexpr {
                                 auto [coord_x, coord_z] = coex::random::uniform_in_unit_circle<>(generator) * 10.0;
                                 auto position = coex::tensor::Vector<Scalar, 3>{coord_x, -0.2, coord_z};
                                 auto reflectance = coex::tensor::elemwise(
                                     coex::math::square<Scalar>, coex::tensor::Vector<Scalar, 3>{coex::random::uniform(generator, 0.0, 1.0),
                                                                                                 coex::random::uniform(generator, 0.0, 1.0),
                                                                                                 coex::random::uniform(generator, 0.0, 1.0)});
                                 auto sphere = coex::geometry::transform::make_translation(
                                     coex::geometry::primitive::make_ellipsoid<>(coex::tensor::Vector<Scalar, 3>{0.2, 0.2, 0.2}, coex::material::make_lambertian<>(std::move(reflectance))),
                                     std::move(position));

                                 if constexpr (sizeof...(Is))
                                     return coex::geometry::csg::make_union(std::move(sphere), self(self, std::index_sequence<Is...>{}));
                                 else
                                     return sphere;
                             }](auto &&...args) { return function(function, std::forward<decltype(args)>(args)...); }(
                            std::make_index_sequence<400>{}),
                        coex::geometry::csg::make_union(
                            // tiny sphere (transmission only)
                            [function =
                                 [&]<auto I, auto... Is>(auto self, std::index_sequence<I, Is...>) constexpr {
                                     auto [coord_x, coord_z] = coex::random::uniform_in_unit_circle<>(generator) * 10.0;
                                     auto position = coex::tensor::Vector<Scalar, 3>{coord_x, -0.2, coord_z};
                                     auto refractive_index = coex::random::uniform(generator, 1.0, 2.0);
                                     auto sphere = coex::geometry::transform::make_translation(
                                         coex::geometry::primitive::make_ellipsoid<>(coex::tensor::Vector<Scalar, 3>{0.2, 0.2, 0.2}, coex::material::make_dielectric<>(refractive_index)),
                                         std::move(position));

                                     if constexpr (sizeof...(Is))
                                         return coex::geometry::csg::make_union(std::move(sphere), self(self, std::index_sequence<Is...>{}));
                                     else
                                         return sphere;
                                 }](auto &&...args) { return function(function, std::forward<decltype(args)>(args)...); }(
                                std::make_index_sequence<200>{}),
                            // tiny sphere (reflection only)
                            [function = [&]<auto I, auto... Is>(auto self, std::index_sequence<I, Is...>) constexpr {
                                auto [coord_x, coord_z] = coex::random::uniform_in_unit_circle<>(generator) * 10.0;
                                auto position = coex::tensor::Vector<Scalar, 3>{coord_x, -0.2, coord_z};
                                coex::tensor::Vector<std::complex<Scalar>, 3> refractive_index{
                                    coex::random::uniform(generator, 0.0, 5.0) + coex::random::uniform(generator, 0.0, 5.0) * 1i,
                                    coex::random::uniform(generator, 0.0, 5.0) + coex::random::uniform(generator, 0.0, 5.0) * 1i,
                                    coex::random::uniform(generator, 0.0, 5.0) + coex::random::uniform(generator, 0.0, 5.0) * 1i,
                                };
                                auto sphere = coex::geometry::transform::make_translation(
                                    coex::geometry::primitive::make_ellipsoid<>(coex::tensor::Vector<Scalar, 3>{0.2, 0.2, 0.2}, coex::material::make_metal<>(std::move(refractive_index))),
                                    std::move(position));

                                if constexpr (sizeof...(Is))
                                    return coex::geometry::csg::make_union(std::move(sphere), self(self, std::index_sequence<Is...>{}));
                                else
                                    return sphere;
                            }](auto &&...args) {
                                return function(function, std::forward<decltype(args)>(args)...);
                            }(std::make_index_sequence<100>{})))))));
}();

inline constexpr auto camera = []() constexpr {
    auto vertical_fov = 20.0 / 180.0 * std::numbers::pi;
    auto aspect_ratio = 1.5;
    auto focal_distance = 10.0;
    auto aperture_radius = 0.1;
    auto response_function = [](const auto &in_direction, const auto &lens_normal) {
        auto cos_theta = coex::tensor::dot(in_direction, lens_normal);
        return 1.0 / coex::math::pow(cos_theta, 4);
    };

    coex::tensor::Vector<Scalar, 3> position{12.0, -2.0, -4.0};
    coex::tensor::Vector<Scalar, 3> target{0.0, 0.0, 0.0};
    coex::tensor::Vector<Scalar, 3> down{0.0, 1.0, 0.0};

    auto w = coex::tensor::normalized(target - position);
    auto u = coex::tensor::normalized(coex::tensor::cross(down, w));
    auto v = coex::tensor::cross(w, u);
    auto rotation = coex::tensor::transposed(coex::tensor::Matrix<Scalar, 3, 3>{u, v, w});

    return coex::optics::Camera<Scalar, coex::tensor::Vector, coex::tensor::Matrix, decltype(response_function)>(
        vertical_fov, aspect_ratio, focal_distance, aperture_radius, response_function, position, rotation);
}();

inline constexpr auto background = [](const auto &ray) constexpr {
    return coex::math::lerp(coex::tensor::get<1>(ray.direction()), -1.0, 1.0, coex::tensor::Vector<Scalar, 3>{0.5, 0.75, 1.0},
                            coex::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0})
         * ray.weight();
};

}  // namespace coex::scene::weekend
