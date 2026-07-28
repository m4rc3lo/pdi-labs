/**
 * @file binary_morphology.hpp
 * @brief Declares manual binary erosion and dilation.
 */

#pragma once

#include "pdi/morphology/binary_structuring_element.hpp"

#include <opencv2/core/mat.hpp>

namespace pdi::morphology {

enum class MorphologyBorderStrategy {
    OutsideBackground,
};

/**
 * @brief Implements binary morphology without `cv::erode` or `cv::dilate`.
 */
class BinaryMorphology {
public:
    [[nodiscard]] cv::Mat erode(
        const cv::Mat& binary_image,
        const BinaryStructuringElement& element,
        MorphologyBorderStrategy border_strategy =
            MorphologyBorderStrategy::OutsideBackground
    ) const;

    [[nodiscard]] cv::Mat dilate(
        const cv::Mat& binary_image,
        const BinaryStructuringElement& element,
        MorphologyBorderStrategy border_strategy =
            MorphologyBorderStrategy::OutsideBackground
    ) const;
};

} // namespace pdi::morphology
