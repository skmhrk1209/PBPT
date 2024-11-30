#pragma once

#include <numbers>

#include "../transform.hpp"
#include "material.hpp"
#include "plane.hpp"
#include "tensor.hpp"

namespace pbpt::geometry::primitive {

template <typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector,
          template <typename, auto, auto> typename Matrix = pbpt::tensor::Matrix,
          template <typename, template <typename, auto> typename> typename Material = pbpt::material::Material>
constexpr auto make_cuboid(const auto &radii, const auto &...args) {
  auto [width, height, depth] = radii;
  return pbpt::geometry::csg::make_enclosure(
      pbpt::geometry::transform::make_translation(
          pbpt::geometry::transform::make_rotation(
              Plane<Scalar, Vector, Material>({depth, width}, args...),
              pbpt::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(pbpt::tensor::Vector<Scalar, 3>{0.0, 0.0, 1.0}, 0.0)),
          pbpt::tensor::Vector<Scalar, 3>{0.0, -height, 0.0}),
      pbpt::geometry::transform::make_translation(
          pbpt::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({depth, width}, args...),
                                                   pbpt::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                       pbpt::tensor::Vector<Scalar, 3>{0.0, 0.0, 1.0}, std::numbers::pi)),
          pbpt::tensor::Vector<Scalar, 3>{0.0, height, 0.0}),
      pbpt::geometry::transform::make_translation(
          pbpt::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({width, height}, args...),
                                                   pbpt::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                       pbpt::tensor::Vector<Scalar, 3>{1.0, 0.0, 0.0}, std::numbers::pi / 2.0)),
          pbpt::tensor::Vector<Scalar, 3>{0.0, 0.0, -depth}),
      pbpt::geometry::transform::make_translation(
          pbpt::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({width, height}, args...),
                                                   pbpt::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                       pbpt::tensor::Vector<Scalar, 3>{1.0, 0.0, 0.0}, -std::numbers::pi / 2.0)),
          pbpt::tensor::Vector<Scalar, 3>{0.0, 0.0, depth}),
      pbpt::geometry::transform::make_translation(
          pbpt::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({height, depth}, args...),
                                                   pbpt::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                       pbpt::tensor::Vector<Scalar, 3>{0.0, 0.0, 1.0}, -std::numbers::pi / 2.0)),
          pbpt::tensor::Vector<Scalar, 3>{-width, 0.0, 0.0}),
      pbpt::geometry::transform::make_translation(
          pbpt::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({height, depth}, args...),
                                                   pbpt::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                       pbpt::tensor::Vector<Scalar, 3>{0.0, 0.0, 1.0}, std::numbers::pi / 2.0)),
          pbpt::tensor::Vector<Scalar, 3>{width, 0.0, 0.0}));
}

}  // namespace pbpt::geometry::primitive
