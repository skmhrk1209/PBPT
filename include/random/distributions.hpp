#pragma once

#include "math.hpp"

namespace pbpt::random {

template <typename G>
constexpr auto uniform(G& generator, auto min, auto max) {
  return pbpt::math::lerp(generator(), G::min(), G::max(), min, max);
}

}  // namespace pbpt::random
