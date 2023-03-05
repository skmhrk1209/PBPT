#pragma once

#include "tensor.hpp"

namespace coex::geometry {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, auto, auto> typename Matrix = coex::tensor::Matrix>
constexpr auto make_rotation_matrix(const auto &axis, auto angle) {
    auto cos = std::cos(angle);
    auto sin = std::sin(angle);
    return (1.0 - cos) * Matrix<Scalar, 3, 3>{
        Vector<Scalar, 3>{coex::tensor::get<0>(axis) * coex::tensor::get<0>(axis), coex::tensor::get<0>(axis) * coex::tensor::get<1>(axis), coex::tensor::get<0>(axis) * coex::tensor::get<2>(axis)},
        Vector<Scalar, 3>{coex::tensor::get<1>(axis) * coex::tensor::get<0>(axis), coex::tensor::get<1>(axis) * coex::tensor::get<1>(axis), coex::tensor::get<1>(axis) * coex::tensor::get<2>(axis)},
        Vector<Scalar, 3>{coex::tensor::get<2>(axis) * coex::tensor::get<0>(axis), coex::tensor::get<2>(axis) * coex::tensor::get<1>(axis), coex::tensor::get<2>(axis) * coex::tensor::get<2>(axis)}
    } + Matrix<Scalar, 3, 3>{
        Vector<Scalar, 3>{                              cos, -coex::tensor::get<2>(axis) * sin,  coex::tensor::get<1>(axis) * sin},
        Vector<Scalar, 3>{ coex::tensor::get<2>(axis) * sin,                               cos, -coex::tensor::get<0>(axis) * sin},
        Vector<Scalar, 3>{-coex::tensor::get<1>(axis) * sin,  coex::tensor::get<0>(axis) * sin,                               cos}
    };
}

}  // namespace coex::geometry
