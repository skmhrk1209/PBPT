#pragma once

#include "math.hpp"
#include "tensor.hpp"

namespace coex::geometry {

template <typename Geometry, typename Scalar = double, template <typename, auto, auto> typename Matrix = coex::tensor::Matrix>
class Rotator {
public:

    constexpr Rotator() = default;
    constexpr Rotator(const Geometry &geometry, const Matrix<Scalar, 3, 3> &rotation) : m_geometry(geometry), m_rotation(rotation) {}
    constexpr Rotator(Geometry &&geometry, Matrix<Scalar, 3, 3> &&rotation) : m_geometry(std::move(geometry)), m_rotation(std::move(rotation)) {}

    constexpr auto &geometry() & { return m_geometry; }
    constexpr const auto &geometry() const & { return m_geometry; }
    constexpr auto &&geometry() && { return std::move(m_geometry); }
    constexpr const auto &&geometry() const && { return std::move(m_geometry); }

    constexpr auto &rotation() & { return m_rotation; }
    constexpr const auto &rotation() const & { return m_rotation; }
    constexpr auto &&rotation() && { return std::move(m_rotation); }
    constexpr const auto &&rotation() const && { return std::move(m_rotation); }

    constexpr auto intersect(const auto &ray) const {
        auto occupations = m_geometry.intersect(ray.rotated(coex::tensor::transposed(m_rotation)));
        decltype(occupations) rotated_occupations;
        while (!occupations.empty()) {
            std::decay_t<decltype(occupations.top().min().surface())> rotated_min_surface(
                [normal_evaluator = occupations.top().min().surface().normal_evaluator(), &rotation = m_rotation](const auto &position) {
                    return rotation % normal_evaluator(coex::tensor::transposed(rotation) % position);
                },
                occupations.top().min().surface().material_reference());
            std::decay_t<decltype(occupations.top().max().surface())> rotated_max_surface(
                [normal_evaluator = occupations.top().max().surface().normal_evaluator(), &rotation = m_rotation](const auto &position) {
                    return rotation % normal_evaluator(coex::tensor::transposed(rotation) % position);
                },
                occupations.top().max().surface().material_reference());
            std::decay_t<decltype(occupations.top().min())> rotated_min_intersection(occupations.top().min().distance(),
                                                                                     std::move(rotated_min_surface));
            std::decay_t<decltype(occupations.top().max())> rotated_max_intersection(occupations.top().max().distance(),
                                                                                     std::move(rotated_max_surface));
            occupations.pop();
            rotated_occupations.emplace(std::move(rotated_min_intersection), std::move(rotated_max_intersection));
        }
        return rotated_occupations;
    }

private:

    Geometry m_geometry;
    Matrix<Scalar, 3, 3> m_rotation;
};

template <typename Geometry, typename Scalar = double, template <typename, auto, auto> typename Matrix = coex::tensor::Matrix>
constexpr auto make_rotator(Geometry &&geometry, auto &&...args) {
    return Rotator<Geometry, Scalar, Matrix>(std::forward<Geometry>(geometry), std::forward<decltype(args)>(args)...);
}

}  // namespace coex::geometry
