/**
 * @file binary_structuring_element.hpp
 * @brief Declares a GUI-independent binary structuring element.
 */

#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace pdi::morphology {

/**
 * @brief Binary structuring element with explicit mask and anchor.
 *
 * @details Active positions use 255 and inactive positions use 0. The initial
 * scope accepts only 3 x 3 masks. The type is independent of GUI concerns, so
 * a future interactive controller can replace the element without changing the
 * morphology implementation.
 */
struct BinaryStructuringElement {
    cv::Mat mask;
    cv::Point anchor;

    [[nodiscard]] static BinaryStructuringElement square_3x3();
    [[nodiscard]] static BinaryStructuringElement cross_3x3();

    /**
     * @brief Validates dimensions, type, values, anchor, and activity.
     */
    void validate() const;
};

} // namespace pdi::morphology
