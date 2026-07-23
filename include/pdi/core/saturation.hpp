/**
 * @file saturation.hpp
 * @brief Declares numeric saturation utilities used by pixel operations.
 */

#pragma once

#include <cstdint>

namespace pdi::core {

/**
 * @brief Saturates and rounds a numeric value to the unsigned 8-bit range.
 *
 * @details Values below zero become 0, values above 255 become 255, and
 * intermediate values are rounded to the nearest integer. NaN values become 0.
 *
 * @param value Numeric value to convert.
 * @return Saturated value in the inclusive range [0, 255].
 *
 * @post The returned value is always representable by std::uint8_t.
 */
[[nodiscard]] std::uint8_t saturate_to_byte(double value) noexcept;

} // namespace pdi::core
