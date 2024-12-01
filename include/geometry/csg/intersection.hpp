#pragma once

#include <utility>

namespace pbpt::geometry::csg {

template <typename Geometry1, typename Geometry2>
struct Intersection : std::pair<Geometry1, Geometry2> {
  using std::pair<Geometry1, Geometry2>::pair;

  constexpr auto intersect(const auto &ray) const {
    auto occupations_1 = this->first.intersect(ray);
    auto occupations_2 = this->second.intersect(ray);

    decltype(occupations_1) occupations;

    while (!occupations_1.empty() && !occupations_2.empty()) {
      if (occupations_1.top().min().distance() < occupations_2.top().min().distance()
              ? occupations_1.top().max().distance() > occupations_2.top().min().distance()
              : occupations_1.top().min().distance() < occupations_2.top().max().distance()) {
        if (occupations_1.top().min().distance() < occupations_2.top().min().distance()) {
          /**********************************
           *   <-------1-------->            *
           *            <-------2------->    *
           *            <---2--->            *
           **********************************/
          if (occupations_1.top().max().distance() < occupations_2.top().max().distance()) {
            occupations.emplace(occupations_2.top().min(), occupations_1.top().max());
            occupations_1.pop();
          }
          /**********************************
           *   <------------1------------>   *
           *        <-------2------->        *
           *        <-------2------->        *
           **********************************/
          else {
            occupations.push(occupations_2.top());
            occupations_2.pop();
          }
        } else {
          /**********************************
           *            <-------1------->    *
           *   <-------2-------->            *
           *            <---1--->            *
           **********************************/
          if (occupations_2.top().max().distance() < occupations_1.top().max().distance()) {
            occupations.emplace(occupations_1.top().min(), occupations_2.top().max());
            occupations_2.pop();
          }
          /**********************************
           *        <-------1------->        *
           *   <------------2------------>   *
           *        <-------1------->        *
           **********************************/
          else {
            occupations.push(occupations_1.top());
            occupations_1.pop();
          }
        }
      } else {
        /**********************************
         *   <---1--->                     *
         *            <-------2------->    *
         *            <-------2------->    *
         **********************************/
        if (occupations_1.top().min().distance() < occupations_2.top().min().distance()) {
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

    return occupations;
  }
};

template <typename Geometry1, typename Geometry2>
constexpr auto make_intersection(Geometry1 &&geometry_1, Geometry2 &&geometry_2) {
  return Intersection<std::decay_t<Geometry1>, std::decay_t<Geometry2>>(
      std::forward<Geometry1>(geometry_1), std::forward<Geometry2>(geometry_2)
  );
}

}  // namespace pbpt::geometry::csg
