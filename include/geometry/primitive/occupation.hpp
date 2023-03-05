#pragma once

#include <tuple>

namespace coex::geometry {

template <typename NormalEvaluator, typename MaterialReference>
struct Surface : std::tuple<NormalEvaluator, MaterialReference> {
    using std::tuple<NormalEvaluator, MaterialReference>::tuple;

    constexpr decltype(auto) normal_evaluator() & { return std::get<0>(*this); }
    constexpr decltype(auto) normal_evaluator() && { return std::get<0>(*this); }
    constexpr decltype(auto) normal_evaluator() const& { return std::get<0>(*this); }
    constexpr decltype(auto) normal_evaluator() const&& { return std::get<0>(*this); }

    constexpr decltype(auto) material_reference() & { return std::get<1>(*this); }
    constexpr decltype(auto) material_reference() && { return std::get<1>(*this); }
    constexpr decltype(auto) material_reference() const& { return std::get<1>(*this); }
    constexpr decltype(auto) material_reference() const&& { return std::get<1>(*this); }
};

template <typename Scalar, typename NormalEvaluator, typename MaterialReference>
struct Intersection : std::pair<Scalar, Surface<NormalEvaluator, MaterialReference>> {
    using std::pair<Scalar, Surface<NormalEvaluator, MaterialReference>>::pair;

    constexpr decltype(auto) distance() & { return std::get<0>(*this); }
    constexpr decltype(auto) distance() && { return std::get<0>(*this); }
    constexpr decltype(auto) distance() const& { return std::get<0>(*this); }
    constexpr decltype(auto) distance() const&& { return std::get<0>(*this); }

    constexpr decltype(auto) surface() & { return std::get<1>(*this); }
    constexpr decltype(auto) surface() && { return std::get<1>(*this); }
    constexpr decltype(auto) surface() const& { return std::get<1>(*this); }
    constexpr decltype(auto) surface() const&& { return std::get<1>(*this); }
};

template <typename Scalar, typename NormalEvaluator, typename MaterialReference>
struct Occupation : std::pair<Intersection<Scalar, NormalEvaluator, MaterialReference>, Intersection<Scalar, NormalEvaluator, MaterialReference>> {
    using std::pair<Intersection<Scalar, NormalEvaluator, MaterialReference>, Intersection<Scalar, NormalEvaluator, MaterialReference>>::pair;

    constexpr decltype(auto) min() & { return std::get<0>(*this); }
    constexpr decltype(auto) min() && { return std::get<0>(*this); }
    constexpr decltype(auto) min() const& { return std::get<0>(*this); }
    constexpr decltype(auto) min() const&& { return std::get<0>(*this); }

    constexpr decltype(auto) max() & { return std::get<1>(*this); }
    constexpr decltype(auto) max() && { return std::get<1>(*this); }
    constexpr decltype(auto) max() const& { return std::get<1>(*this); }
    constexpr decltype(auto) max() const&& { return std::get<1>(*this); }
};

}  // namespace coex::geometry
