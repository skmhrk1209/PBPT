#pragma once

#include <optional>
#include <tuple>

namespace coex::geometry::csg {

template <typename... Geometries>
struct Enclosure : std::tuple<Geometries...> {
    using std::tuple<Geometries...>::tuple;

    constexpr auto intersect(const auto &ray) const {
        decltype(std::get<0>(*this).intersect(ray)) occupations;

        std::optional<typename decltype(occupations)::value_type::first_type> min_intersection;
        std::optional<typename decltype(occupations)::value_type::second_type> max_intersection;

        [&]<auto... Is>(std::index_sequence<Is...>) constexpr {
            [](auto &&...) {}((
                [&](const auto &geometry) constexpr {
                    auto occupations = geometry.intersect(ray);
                    while (!occupations.empty()) {
                        if (!min_intersection || occupations.top().min().distance() < min_intersection.value().distance()) {
                            min_intersection = occupations.top().min();
                        }
                        if (!max_intersection || occupations.top().max().distance() > max_intersection.value().distance()) {
                            max_intersection = occupations.top().max();
                        }
                        occupations.pop();
                    }
                }(std::get<Is>(*this)),
                0)...);
        }
        (std::make_index_sequence<sizeof...(Geometries)>{});

        if (min_intersection && max_intersection) {
            occupations.emplace(std::move(min_intersection.value()), std::move(max_intersection.value()));
        }

        return occupations;
    }
};

template <typename... Geometries>
constexpr auto make_enclosure(Geometries &&...geometries) {
    return Enclosure<std::decay_t<Geometries>...>(std::forward<Geometries>(geometries)...);
}

}  // namespace coex::geometry::csg
