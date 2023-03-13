#pragma once

#include <utility>

namespace coex::geometry::csg {

template <typename Geometry1, typename Geometry2>
struct Difference : std::pair<Geometry1, Geometry2> {
    using std::pair<Geometry1, Geometry2>::pair;

    constexpr auto intersect(const auto &ray) const {
        auto occupations_1 = this->first.intersect(ray);
        auto occupations_2 = this->second.intersect(ray);

        decltype(occupations_1) occupations;

        auto invert_normal = [&]<typename Intersection>(const Intersection &intersection) constexpr -> Intersection {
            auto inverted_normal_evaluator = [normal_evaluator = intersection.surface().normal_evaluator()](const auto &position) constexpr {
                return -normal_evaluator(position);
            };
            typename Intersection::second_type surface(inverted_normal_evaluator, intersection.surface().material_reference());
            return {intersection.distance(), std::move(surface)};
        };

        while (!occupations_1.empty() && !occupations_2.empty()) {
            if (occupations_1.top().min().distance() < occupations_2.top().min().distance()
                    ? occupations_1.top().max().distance() > occupations_2.top().min().distance()
                    : occupations_1.top().min().distance() < occupations_2.top().max().distance()) {
                if (occupations_1.top().min().distance() < occupations_2.top().min().distance()) {
                    /**********************************
                     *   <-------1------->             *
                     *            <-------2------->    *
                     *   <---1--->                     *
                     **********************************/
                    if (occupations_1.top().max().distance() < occupations_2.top().max().distance()) {
                        occupations.emplace(occupations_1.top().min(), invert_normal(occupations_2.top().min()));
                        occupations_1.pop();
                    }
                    /**********************************
                     *   <------------1------------>   *
                     *        <-------2------->        *
                     *   <-1->                 <-1->   *
                     **********************************/
                    else {
                        occupations_1.emplace(invert_normal(occupations_2.top().max()), occupations_1.top().max());
                        occupations.emplace(occupations_1.top().min(), invert_normal(occupations_2.top().min()));
                        occupations_1.pop();
                        occupations_2.pop();
                    }
                } else {
                    /**********************************
                     *            <-------1------->    *
                     *   <-------2------->             *
                     *                    <---1--->    *
                     **********************************/
                    if (occupations_2.top().max().distance() < occupations_1.top().max().distance()) {
                        occupations_1.emplace(invert_normal(occupations_2.top().max()), occupations_1.top().max());
                        occupations_1.pop();
                        occupations_2.pop();
                    }
                    /**********************************
                     *        <-------1------->        *
                     *   <------------2------------>   *
                     *                                 *
                     **********************************/
                    else {
                        occupations_1.pop();
                    }
                }
            } else {
                /**********************************
                 *   <---1--->                     *
                 *            <-------2------->    *
                 *   <---1--->                     *
                 **********************************/
                if (occupations_1.top().min().distance() < occupations_2.top().min().distance()) {
                    occupations.push(occupations_1.top());
                    occupations_1.pop();
                }
                /**********************************
                 *            <-------1------->    *
                 *   <---2--->                     *
                 *            <-------1------->    *
                 **********************************/
                else {
                    occupations_2.pop();
                }
            }
        }

        while (!occupations_1.empty()) {
            occupations.push(occupations_1.top());
            occupations_1.pop();
        }

        return occupations;
    }
};

template <typename Geometry1, typename Geometry2>
constexpr auto make_difference(Geometry1 &&geometry_1, Geometry2 &&geometry_2) {
    return Difference<std::decay_t<Geometry1>, std::decay_t<Geometry2>>(std::forward<Geometry1>(geometry_1), std::forward<Geometry2>(geometry_2));
}

}  // namespace coex::geometry::csg
