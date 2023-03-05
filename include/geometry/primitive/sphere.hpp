#pragma once

#include <optional>
#include <utility>

#include "material.hpp"
#include "math.hpp"
#include "primitive.hpp"
#include "tensor.hpp"

namespace coex::geometry {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, template <typename, auto> typename> typename Material = coex::material::Material>
class Sphere {
public:

    constexpr Sphere() = default;
    constexpr Sphere(Scalar radius, const Material<Scalar, Vector> &material) : m_radius(radius), m_material(material) {}
    constexpr Sphere(Scalar radius, Material<Scalar, Vector> &&material) : m_radius(radius), m_material(std::move(material)) {}

    constexpr auto &radius() & { return m_radius; }
    constexpr const auto &radius() const & { return m_radius; }
    constexpr auto &&radius() && { return std::move(m_radius); }
    constexpr const auto &&radius() const && { return std::move(m_radius); }

    constexpr auto &material() & { return m_material; }
    constexpr const auto &material() const & { return m_material; }
    constexpr auto &&material() && { return std::move(m_material); }
    constexpr const auto &&material() const && { return std::move(m_material); }

    constexpr auto normal(const auto &position) const { return position / m_radius; }

    constexpr auto intersect(const auto &ray) const {
        auto a = coex::tensor::dot(ray.direction(), ray.direction());
        auto b = coex::tensor::dot(ray.direction(), ray.position());
        auto c = coex::tensor::dot(ray.position(), ray.position()) - m_radius * m_radius;
        auto d = b * b - a * c;

        using NormalEvaluator = std::function<Vector<Scalar, 3>(const Vector<Scalar, 3> &)>;
        using MaterialReference = std::reference_wrapper<const Material<Scalar, Vector>>;
        using OccupationType = Occupation<Scalar, NormalEvaluator, MaterialReference>;
        using OccupationComp = decltype([](const auto &occupation_1, const auto &occupation_2) {
            return occupation_1.min().distance() > occupation_2.min().distance();
        });
        using OccupationQueue = std::priority_queue<OccupationType, std::vector<OccupationType>, OccupationComp>;

        OccupationQueue occupations;
        if (d >= 0) {
            auto min_distance = (-b - coex::math::sqrt(d)) / a;
            auto max_distance = (-b + coex::math::sqrt(d)) / a;
            if (max_distance > 0.0) {
                Surface<NormalEvaluator, MaterialReference> surface([this](const auto &position) { return normal(position); }, std::cref(material()));
                Intersection<Scalar, NormalEvaluator, MaterialReference> min_intersection(min_distance, surface);
                Intersection<Scalar, NormalEvaluator, MaterialReference> max_intersection(max_distance, surface);
                occupations.emplace(std::move(min_intersection), std::move(max_intersection));
            }
        }
        return occupations;
    }

private:

    Scalar m_radius;
    Material<Scalar, Vector> m_material;
};

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          template <typename, template <typename, auto> typename> typename Material = coex::material::Material>
constexpr auto make_sphere(auto &&...args) {
    return Primitive<Scalar, Vector, Material>(Sphere<Scalar, Vector, Material>(std::forward<decltype(args)>(args)...));
}

}  // namespace coex::geometry
