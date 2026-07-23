/**
 * @file saturation.cpp
 * @brief Implements numeric saturation utilities for pixel operations.
 */

#include "pdi/core/saturation.hpp"

#include <cmath>
#include <cstdint>

namespace pdi::core {

std::uint8_t saturate_to_byte(const double value) noexcept {
    if (std::isnan(value) || value <= 0.0) {
        return std::uint8_t{0};
    }

    if (value >= 255.0) {
        return std::uint8_t{255};
    }

    return static_cast<std::uint8_t>(std::lround(value));
}

} // namespace pdi::core
