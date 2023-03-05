#pragma once

#include "math.hpp"
#include "tensor.hpp"

namespace coex::geometry {

template <typename Geometry, typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
class Translator {
public:

    constexpr Translator() = default;

    constexpr Translator(const Geometry &geometry, const Vector<Scalar, 3> &translation) : m_geometry(geometry), m_translation(translation) {}

    constexpr Translator(Geometry &&geometry, Vector<Scalar, 3> &&translation)
        : m_geometry(std::move(geometry)),
          m_translation(std::move(translation)) {}

    constexpr auto &geometry() & { return m_geometry; }
    constexpr const auto &geometry() const & { return m_geometry; }
    constexpr auto &&geometry() && { return std::move(m_geometry); }
    constexpr const auto &&geometry() const && { return std::move(m_geometry); }

    constexpr auto &translation() & { return m_translation; }
    constexpr const auto &translation() const & { return m_translation; }
    constexpr auto &&translation() && { return std::move(m_translation); }
    constexpr const auto &&translation() const && { return std::move(m_translation); }

    constexpr auto intersect(const auto &ray) const {
        auto occupations = m_geometry.intersect(ray.translated(-m_translation));
        decltype(occupations) translated_occupations;
        while (!occupations.empty()) {
            std::decay_t<decltype(occupations.top().min().surface())> translated_min_surface(
                [normal_evaluator = occupations.top().min().surface().normal_evaluator(), translation = m_translation](const auto &position) {
                    return normal_evaluator(position - translation);
                },
                occupations.top().min().surface().material_reference());
            std::decay_t<decltype(occupations.top().max().surface())> translated_max_surface(
                [normal_evaluator = occupations.top().max().surface().normal_evaluator(), translation = m_translation](const auto &position) {
                    return normal_evaluator(position - translation);
                },
                occupations.top().max().surface().material_reference());
            std::decay_t<decltype(occupations.top().min())> translated_min_intersection(occupations.top().min().distance(),
                                                                                        std::move(translated_min_surface));
            std::decay_t<decltype(occupations.top().max())> translated_max_intersection(occupations.top().max().distance(),
                                                                                        std::move(translated_max_surface));
            occupations.pop();
            translated_occupations.emplace(std::move(translated_min_intersection), std::move(translated_max_intersection));
        }
        return translated_occupations;
    }

private:

    Geometry m_geometry;
    Vector<Scalar, 3> m_translation;
};

template <typename Geometry, typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector>
constexpr auto make_translator(Geometry &&geometry, auto &&...args) {
    return Translator<Geometry, Scalar, Vector>(std::forward<Geometry>(geometry), std::forward<decltype(args)>(args)...);
}

}  // namespace coex::geometry
