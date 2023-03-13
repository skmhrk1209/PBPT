#pragma once

#include <numbers>

#include "../transform.hpp"
#include "material.hpp"
#include "plane.hpp"
#include "tensor.hpp"

namespace coex::geometry::primitive {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, auto, auto> typename Matrix = coex::tensor::Matrix,
          template <typename, template <typename, auto> typename> typename Material = coex::material::Material>
constexpr auto make_cuboid(const auto &radii, const auto &...args) {
    auto [width, height, depth] = radii;
    return coex::geometry::csg::make_enclosure(
        coex::geometry::transform::make_translation(
            coex::geometry::transform::make_rotation(
                Plane<Scalar, Vector, Material>({depth, width}, args...),
                coex::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(coex::tensor::Vector<Scalar, 3>{0.0, 0.0, 1.0}, 0.0)),
            coex::tensor::Vector<Scalar, 3>{0.0, -height, 0.0}),
        coex::geometry::transform::make_translation(
            coex::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({depth, width}, args...),
                                                     coex::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                         coex::tensor::Vector<Scalar, 3>{0.0, 0.0, 1.0}, std::numbers::pi)),
            coex::tensor::Vector<Scalar, 3>{0.0, height, 0.0}),
        coex::geometry::transform::make_translation(
            coex::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({width, height}, args...),
                                                     coex::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                         coex::tensor::Vector<Scalar, 3>{1.0, 0.0, 0.0}, std::numbers::pi / 2.0)),
            coex::tensor::Vector<Scalar, 3>{0.0, 0.0, -depth}),
        coex::geometry::transform::make_translation(
            coex::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({width, height}, args...),
                                                     coex::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                         coex::tensor::Vector<Scalar, 3>{1.0, 0.0, 0.0}, -std::numbers::pi / 2.0)),
            coex::tensor::Vector<Scalar, 3>{0.0, 0.0, depth}),
        coex::geometry::transform::make_translation(
            coex::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({height, depth}, args...),
                                                     coex::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                         coex::tensor::Vector<Scalar, 3>{0.0, 0.0, 1.0}, -std::numbers::pi / 2.0)),
            coex::tensor::Vector<Scalar, 3>{-width, 0.0, 0.0}),
        coex::geometry::transform::make_translation(
            coex::geometry::transform::make_rotation(Plane<Scalar, Vector, Material>({height, depth}, args...),
                                                     coex::geometry::transform::make_rotation_matrix<Scalar, Vector, Matrix>(
                                                         coex::tensor::Vector<Scalar, 3>{0.0, 0.0, 1.0}, std::numbers::pi / 2.0)),
            coex::tensor::Vector<Scalar, 3>{width, 0.0, 0.0}));
}

}  // namespace coex::geometry::primitive
