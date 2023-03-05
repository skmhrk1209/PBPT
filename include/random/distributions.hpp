#pragma once

#include "math.hpp"

namespace coex::random {

template <typename G>
constexpr auto uniform(G& generator, auto min, auto max) {
    return coex::math::lerp(generator(), G::min, G::max, min, max);
}

}  // namespace coex::random
