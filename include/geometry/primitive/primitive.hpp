#pragma once

#include <queue>
#include <variant>

#include "material.hpp"
#include "occupation.hpp"

namespace coex::geometry {

template <typename... Primitives>
struct GenericPrimitive : std::variant<Primitives...> {
    using std::variant<Primitives...>::variant;

    constexpr auto intersect(auto&&... args) const {
        return std::visit([&](const auto& primitive) { return primitive.intersect(std::forward<decltype(args)>(args)...); }, *this);
    }
};

template <typename Scalar, template <typename, auto> typename Vector, template <typename, template <typename, auto> typename> typename Material>
class Sphere;

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, template <typename, auto> typename> typename Material = coex::material::Material>
using Primitive = GenericPrimitive<Sphere<Scalar, Vector, Material>>;

}  // namespace coex::geometry
