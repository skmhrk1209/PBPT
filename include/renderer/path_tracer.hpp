#pragma once

#include <iostream>
#include <mutex>

#include "math.hpp"
#include "optics.hpp"
#include "random.hpp"
#include "tensor.hpp"

namespace coex::renderer {

template <typename Scalar = double, template <typename, auto> typename Vector = coex::tensor::Vector,
          typename Generator = coex::random::LinearCongruentialGenerator<>>
constexpr auto path_tracer(const auto &object, const auto &camera, auto background, auto image_width, auto image_height, auto start_index,
                           auto stop_index, auto continuation, auto random_seed, auto &image_writer) {
#pragma omp parallel for
    for (auto pixel_index = start_index; pixel_index < stop_index; ++pixel_index) {
        Generator generator(random_seed + pixel_index);

        auto pixel_index_u = pixel_index % image_width;
        auto pixel_index_v = pixel_index / image_width;

        auto pixel_coord_u = (pixel_index_u + coex::random::uniform(generator, -0.5, 0.5)) / image_width;
        auto pixel_coord_v = (pixel_index_v + coex::random::uniform(generator, -0.5, 0.5)) / image_height;

        auto ray = camera.ray(pixel_coord_u, pixel_coord_v, generator);

        auto tracer = [function = [&](auto self, const auto &ray) -> Vector<Scalar, 3> {
            if (coex::random::uniform(generator, 0.0, 1.0) > continuation) return {};

            auto occupations = object.intersect(ray);

            while (!occupations.empty() && occupations.top().max().distance() <= 0) occupations.pop();

            if (occupations.empty()) return background(ray);

            auto intersection = occupations.top().min().distance() > 0 ? occupations.top().min() : occupations.top().max();

            return [&, ray = ray.advanced(intersection.distance()), &normal_evaluator = intersection.surface().normal_evaluator(),
                    &material_reference = intersection.surface().material_reference()]() {
                auto normal = normal_evaluator(ray.position());
                auto [radiance, traced_ray] = material_reference(ray, normal, generator);
                return traced_ray ? radiance * self(self, traced_ray.value()) / continuation : radiance;
            }();
        }](auto &&...args) { return function(function, std::forward<decltype(args)>(args)...); };

        image_writer(pixel_index, tracer(ray));
    }
}

}  // namespace coex::renderer
