#pragma once

#include <variant>

namespace coex::material {

template <typename... Materials>
struct GenericMaterial : std::variant<Materials...> {
    using std::variant<Materials...>::variant;

    constexpr auto operator()(auto &&...args) const {
        return std::visit([&](const auto &material) { return material(std::forward<decltype(args)>(args)...); }, *this);
    }
};

template <typename Scalar, template <typename, auto> typename Vector>
class Lambertian;

template <typename Scalar, template <typename, auto> typename Vector>
class Metal;

template <typename Scalar, template <typename, auto> typename Vector>
class Dielectric;

template <typename Scalar, template <typename, auto> typename Vector>
class Emissive;

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
using Material = GenericMaterial<Lambertian<Scalar, Vector>, Metal<Scalar, Vector>, Dielectric<Scalar, Vector>, Emissive<Scalar, Vector>>;

}  // namespace coex::material
