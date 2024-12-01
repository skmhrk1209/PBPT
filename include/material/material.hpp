#pragma once

#include <variant>

#include "tensor.hpp"

namespace pbpt::material {

template <typename... Materials>
struct GenericMaterial : std::variant<Materials...> {
  using std::variant<Materials...>::variant;

  constexpr auto operator()(auto &&...args) const {
    return std::visit(
        [&](const auto &material) constexpr { return material(std::forward<decltype(args)>(args)...); }, *this
    );
  }
};

template <typename Scalar, template <typename, auto> typename Vector>
struct Lambertian;

template <typename Scalar, template <typename, auto> typename Vector>
struct Metal;

template <typename Scalar, template <typename, auto> typename Vector>
struct Dielectric;

template <typename Scalar, template <typename, auto> typename Vector>
struct Emissive;

template <typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector>
using Material = GenericMaterial<
    Lambertian<Scalar, Vector>, Metal<Scalar, Vector>, Dielectric<Scalar, Vector>, Emissive<Scalar, Vector>>;

}  // namespace pbpt::material
