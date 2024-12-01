#pragma once

#include <cmath>
#include <cstdint>
#include <fstream>

namespace pbpt::image {

auto write_ppm(const auto &filename, const auto &colors, auto width, auto height) {
  std::ofstream ostream(filename);

  ostream << "P3\n";
  ostream << width << " " << height << "\n";
  ostream << (1 << 8) - 1 << "\n";

  for (const auto &color : colors) {
    for (const auto &component : color) {
      ostream << +static_cast<std::uint8_t>(std::clamp(component, 0.0, 1.0) * ((1 << 8) - 1)) << " ";
    }
    ostream << "\n";
  }
}

}  // namespace pbpt::image
