#pragma once

#include "tensor.hpp"

namespace pbpt::optics {
template <typename Scalar = double, template <typename, auto> typename Vector = pbpt::tensor::Vector>
struct Ray {
  constexpr Ray() = default;

  constexpr Ray(const Vector<Scalar, 3> &position, const Vector<Scalar, 3> &direction, Scalar weight)
      : m_position(position), m_direction(direction), m_weight(weight) {}

  constexpr Ray(Vector<Scalar, 3> &&position, Vector<Scalar, 3> &&direction, Scalar weight)
      : m_position(std::move(position)), m_direction(std::move(direction)), m_weight(weight) {}

  constexpr auto &position() & { return m_position; }
  constexpr const auto &position() const & { return m_position; }
  constexpr auto &&position() && { return std::move(m_position); }
  constexpr const auto &&position() const && { return std::move(m_position); }

  constexpr auto &direction() & { return m_direction; }
  constexpr const auto &direction() const & { return m_direction; }
  constexpr auto &&direction() && { return std::move(m_direction); }
  constexpr const auto &&direction() const && { return std::move(m_direction); }

  constexpr auto &weight() & { return m_weight; }
  constexpr const auto &weight() const & { return m_weight; }
  constexpr auto &&weight() && { return std::move(m_weight); }
  constexpr const auto &&weight() const && { return std::move(m_weight); }

  constexpr auto at(auto distance) const { return m_position + m_direction * distance; }

  constexpr auto advance(auto distance) { m_position = m_position + m_direction * distance; }
  constexpr auto advanced(auto distance) const -> Ray<Scalar, Vector> {
    return {m_position + m_direction * distance, m_direction, m_weight};
  }

  constexpr auto translate(const auto &translation) { m_position = m_position + translation; }
  constexpr auto translated(const auto &translation) const -> Ray<Scalar, Vector> {
    return {m_position + translation, m_direction, m_weight};
  }

  constexpr auto rotate(const auto &rotation) {
    m_position = rotation % m_position;
    m_direction = rotation % m_direction;
  }
  constexpr auto rotated(const auto &rotation) const -> Ray<Scalar, Vector> {
    return {rotation % m_position, rotation % m_direction, m_weight};
  }

 private:
  Vector<Scalar, 3> m_position;
  Vector<Scalar, 3> m_direction;
  Scalar m_weight;
};
}  // namespace pbpt::optics
