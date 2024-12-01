
#include <numbers>

#include "geometry.hpp"
#include "material.hpp"
#include "math.hpp"
#include "optics.hpp"
#include "random.hpp"
#include "tensor.hpp"

using Scalar = double;

namespace pbpt::scene::weekend {

inline constexpr auto object = []() constexpr {
  using namespace std::literals::complex_literals;
  pbpt::random::LinearCongruentialGenerator<> generator(1);
  return pbpt::geometry::csg::make_union(
      // ground sphere
      pbpt::geometry::transform::make_translation(
          pbpt::geometry::primitive::make_ellipsoid(
              pbpt::tensor::Vector<Scalar, 3>{1000.0, 1000.0, 1000.0},
              pbpt::material::make_lambertian(pbpt::tensor::Vector<Scalar, 3>{0.5, 0.5, 0.5})
          ),
          pbpt::tensor::Vector<Scalar, 3>{0.0, 1000.0, 0.0}
      ),
      pbpt::geometry::csg::make_union(
          // left sphere (gold)
          pbpt::geometry::transform::make_translation(
              pbpt::geometry::primitive::make_ellipsoid(
                  pbpt::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0},
                  pbpt::material::make_metal(pbpt::tensor::Vector<std::complex<Scalar>, 3>{
                      0.18299 + 3.42420i,
                      0.42108 + 2.34590i,
                      1.37340 + 1.77040i,
                  })
              ),
              pbpt::tensor::Vector<Scalar, 3>{-4.0, -1.0, 0.0}
          ),
          pbpt::geometry::csg::make_union(
              // center sphere (glass)
              pbpt::geometry::transform::make_translation(
                  pbpt::geometry::primitive::make_ellipsoid(
                      pbpt::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0}, pbpt::material::make_dielectric(1.5)
                  ),
                  pbpt::tensor::Vector<Scalar, 3>{0.0, -1.0, 0.0}
              ),
              pbpt::geometry::csg::make_union(
                  // right sphere (platinum)
                  pbpt::geometry::transform::make_translation(
                      pbpt::geometry::primitive::make_ellipsoid(
                          pbpt::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0},
                          pbpt::material::make_metal(pbpt::tensor::Vector<std::complex<Scalar>, 3>{
                              2.37570 + 4.26550i,
                              2.08470 + 3.71530i,
                              1.84530 + 3.13650i,
                          })
                      ),
                      pbpt::tensor::Vector<Scalar, 3>{4.0, -1.0, 0.0}
                  ),
                  pbpt::geometry::csg::make_union(
                      // tiny sphere (scatteing only)
                      [function =
                           [&]<auto I, auto... Is>(auto self, std::index_sequence<I, Is...>) constexpr {
                             auto [coord_x, coord_z] =
                                 pbpt::random::uniform_in_unit_circle<Scalar, pbpt::tensor::Vector>(generator) * 10.0;
                             auto position = pbpt::tensor::Vector<Scalar, 3>{coord_x, -0.2, coord_z};
                             auto reflectance = pbpt::tensor::elemwise(
                                 pbpt::math::square<Scalar>,
                                 pbpt::tensor::Vector<Scalar, 3>{
                                     pbpt::random::uniform(generator, 0.0, 1.0),
                                     pbpt::random::uniform(generator, 0.0, 1.0),
                                     pbpt::random::uniform(generator, 0.0, 1.0)
                                 }
                             );
                             auto sphere = pbpt::geometry::transform::make_translation(
                                 pbpt::geometry::primitive::make_ellipsoid(
                                     pbpt::tensor::Vector<Scalar, 3>{0.2, 0.2, 0.2},
                                     pbpt::material::make_lambertian(std::move(reflectance))
                                 ),
                                 std::move(position)
                             );

                             if constexpr (sizeof...(Is))
                               return pbpt::geometry::csg::make_union(
                                   std::move(sphere), self(self, std::index_sequence<Is...>{})
                               );
                             else
                               return sphere;
                           }](auto &&...args) { return function(function, std::forward<decltype(args)>(args)...); }(
                          std::make_index_sequence<400>{}
                      ),
                      pbpt::geometry::csg::make_union(
                          // tiny sphere (transmission only)
                          [function =
                               [&]<auto I, auto... Is>(auto self, std::index_sequence<I, Is...>) constexpr {
                                 auto [coord_x, coord_z] =
                                     pbpt::random::uniform_in_unit_circle<Scalar, pbpt::tensor::Vector>(generator) *
                                     10.0;
                                 auto position = pbpt::tensor::Vector<Scalar, 3>{coord_x, -0.2, coord_z};
                                 auto refractive_index = pbpt::random::uniform(generator, 1.0, 2.0);
                                 auto sphere = pbpt::geometry::transform::make_translation(
                                     pbpt::geometry::primitive::make_ellipsoid(
                                         pbpt::tensor::Vector<Scalar, 3>{0.2, 0.2, 0.2},
                                         pbpt::material::make_dielectric(refractive_index)
                                     ),
                                     std::move(position)
                                 );

                                 if constexpr (sizeof...(Is))
                                   return pbpt::geometry::csg::make_union(
                                       std::move(sphere), self(self, std::index_sequence<Is...>{})
                                   );
                                 else
                                   return sphere;
                               }](auto &&...args) { return function(function, std::forward<decltype(args)>(args)...); }(
                              std::make_index_sequence<200>{}
                          ),
                          // tiny sphere (reflection only)
                          [function =
                               [&]<auto I, auto... Is>(auto self, std::index_sequence<I, Is...>) constexpr {
                                 auto [coord_x, coord_z] =
                                     pbpt::random::uniform_in_unit_circle<Scalar, pbpt::tensor::Vector>(generator) *
                                     10.0;
                                 auto position = pbpt::tensor::Vector<Scalar, 3>{coord_x, -0.2, coord_z};
                                 pbpt::tensor::Vector<std::complex<Scalar>, 3> refractive_index{
                                     pbpt::random::uniform(generator, 0.0, 5.0) +
                                         pbpt::random::uniform(generator, 0.0, 5.0) * 1i,
                                     pbpt::random::uniform(generator, 0.0, 5.0) +
                                         pbpt::random::uniform(generator, 0.0, 5.0) * 1i,
                                     pbpt::random::uniform(generator, 0.0, 5.0) +
                                         pbpt::random::uniform(generator, 0.0, 5.0) * 1i,
                                 };
                                 auto sphere = pbpt::geometry::transform::make_translation(
                                     pbpt::geometry::primitive::make_ellipsoid(
                                         pbpt::tensor::Vector<Scalar, 3>{0.2, 0.2, 0.2},
                                         pbpt::material::make_metal(std::move(refractive_index))
                                     ),
                                     std::move(position)
                                 );

                                 if constexpr (sizeof...(Is))
                                   return pbpt::geometry::csg::make_union(
                                       std::move(sphere), self(self, std::index_sequence<Is...>{})
                                   );
                                 else
                                   return sphere;
                               }](auto &&...args) { return function(function, std::forward<decltype(args)>(args)...); }(
                              std::make_index_sequence<100>{}
                          )
                      )
                  )
              )
          )
      )
  );
}();

inline constexpr auto camera = []() constexpr {
  auto vertical_fov = 20.0 / 180.0 * std::numbers::pi;
  auto aspect_ratio = 1.5;
  auto focal_distance = 10.0;
  auto aperture_radius = 0.1;
  auto response_function = [](const auto &in_direction, const auto &lens_normal) {
    auto cos_theta = pbpt::tensor::dot(in_direction, lens_normal);
    return 1.0 / pbpt::math::pow(cos_theta, 4);
  };

  pbpt::tensor::Vector<Scalar, 3> position{12.0, -2.0, -4.0};
  pbpt::tensor::Vector<Scalar, 3> target{0.0, 0.0, 0.0};
  pbpt::tensor::Vector<Scalar, 3> down{0.0, 1.0, 0.0};

  auto w = pbpt::tensor::normalized(target - position);
  auto u = pbpt::tensor::normalized(pbpt::tensor::cross(down, w));
  auto v = pbpt::tensor::cross(w, u);
  auto rotation = pbpt::tensor::transposed(pbpt::tensor::Matrix<Scalar, 3, 3>{u, v, w});

  return pbpt::optics::Camera<Scalar, pbpt::tensor::Vector, pbpt::tensor::Matrix, decltype(response_function)>(
      vertical_fov, aspect_ratio, focal_distance, aperture_radius, response_function, position, rotation
  );
}();

inline constexpr auto background = [](const auto &ray) constexpr {
  return pbpt::math::lerp(
             pbpt::tensor::get<1>(ray.direction()), -1.0, 1.0, pbpt::tensor::Vector<Scalar, 3>{0.5, 0.75, 1.0},
             pbpt::tensor::Vector<Scalar, 3>{1.0, 1.0, 1.0}
         ) *
         ray.weight();
};

}  // namespace pbpt::scene::weekend
