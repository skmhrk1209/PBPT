#pragma once

#include "math.hpp"
#include "tensor.hpp"

namespace pbpt::geometry::transform {

template <typename Geometry, typename Scalar = double, template <typename, auto, auto> typename Matrix = pbpt::tensor::Matrix>
struct Rotation {
    constexpr Rotation() = default;
    constexpr Rotation(const Geometry &geometry, const Matrix<Scalar, 3, 3> &rotation) : m_geometry(geometry), m_rotation(rotation) {}
    constexpr Rotation(Geometry &&geometry, Matrix<Scalar, 3, 3> &&rotation) : m_geometry(std::move(geometry)), m_rotation(std::move(rotation)) {}

    constexpr auto &geometry() & { return m_geometry; }
    constexpr const auto &geometry() const & { return m_geometry; }
    constexpr auto &&geometry() && { return std::move(m_geometry); }
    constexpr const auto &&geometry() const && { return std::move(m_geometry); }

    constexpr auto &rotation() & { return m_rotation; }
    constexpr const auto &rotation() const & { return m_rotation; }
    constexpr auto &&rotation() && { return std::move(m_rotation); }
    constexpr const auto &&rotation() const && { return std::move(m_rotation); }

    constexpr auto intersect(const auto &ray) const {
        auto occupations = m_geometry.intersect(ray.rotated(pbpt::tensor::transposed(m_rotation)));

        auto rotate_normal = [&]<typename Intersection>(const Intersection &intersection) constexpr -> Intersection {
            auto rotated_normal_evaluator = [this, normal_evaluator = intersection.surface().normal_evaluator()](const auto &position) constexpr {
                return m_rotation % normal_evaluator(pbpt::tensor::transposed(m_rotation) % position);
            };
            typename Intersection::second_type surface(rotated_normal_evaluator, intersection.surface().material_reference());
            return {intersection.distance(), std::move(surface)};
        };

        decltype(occupations) rotated_occupations;
        while (!occupations.empty()) {
            rotated_occupations.emplace(rotate_normal(occupations.top().min()), rotate_normal(occupations.top().max()));
            occupations.pop();
        }
        return rotated_occupations;
    }

private:

    Geometry m_geometry;
    Matrix<Scalar, 3, 3> m_rotation;
};

template <typename Geometry, typename Scalar = double, template <typename, auto, auto> typename Matrix = pbpt::tensor::Matrix>
constexpr auto make_rotation(Geometry &&geometry, auto &&...args) {
    return Rotation<std::decay_t<Geometry>, Scalar, Matrix>(std::forward<Geometry>(geometry), std::forward<decltype(args)>(args)...);
}

template <typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector,
          template <typename, auto, auto> typename Matrix = pbpt::tensor::Matrix>
constexpr auto make_rotation_matrix(const auto &axis, auto angle) {
    auto [x, y, z] = axis;
    auto cos = std::cos(angle);
    auto sin = std::sin(angle);
    return (1.0 - cos) * Matrix<Scalar, 3, 3>{
        Vector<Scalar, 3>{x * x, x * y, x * z},
        Vector<Scalar, 3>{y * x, y * y, y * z},
        Vector<Scalar, 3>{z * x, z * y, z * z}
    } + Matrix<Scalar, 3, 3>{
        Vector<Scalar, 3>{     cos, -z * sin,  y * sin},
        Vector<Scalar, 3>{ z * sin,      cos, -x * sin},
        Vector<Scalar, 3>{-y * sin,  x * sin,      cos}
    };
}

}  // namespace pbpt::geometry::transform
