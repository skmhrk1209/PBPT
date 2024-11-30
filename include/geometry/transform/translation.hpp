#pragma once

#include "math.hpp"
#include "tensor.hpp"

namespace pbpt::geometry::transform {

template <typename Geometry, typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector>
struct Translation {
  constexpr Translation() = default;

  constexpr Translation(const Geometry &geometry, const Vector<Scalar, 3> &translation) : m_geometry(geometry), m_translation(translation) {}

  constexpr Translation(Geometry &&geometry, Vector<Scalar, 3> &&translation)
      : m_geometry(std::move(geometry)), m_translation(std::move(translation)) {}

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

    auto translate_normal = [&]<typename Intersection>(const Intersection &intersection) constexpr -> Intersection {
      auto translated_normal_evaluator = [this, normal_evaluator = intersection.surface().normal_evaluator()](const auto &position) constexpr {
        return normal_evaluator(position - m_translation);
      };
      typename Intersection::second_type surface(translated_normal_evaluator, intersection.surface().material_reference());
      return {intersection.distance(), std::move(surface)};
    };

    decltype(occupations) translated_occupations;
    while (!occupations.empty()) {
      translated_occupations.emplace(translate_normal(occupations.top().min()), translate_normal(occupations.top().max()));
      occupations.pop();
    }
    return translated_occupations;
  }

 private:
  Geometry m_geometry;
  Vector<Scalar, 3> m_translation;
};

template <typename Geometry, typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector>
constexpr auto make_translation(Geometry &&geometry, auto &&...args) {
  return Translation<std::decay_t<Geometry>, Scalar, Vector>(std::forward<Geometry>(geometry), std::forward<decltype(args)>(args)...);
}

}  // namespace pbpt::geometry::transform
