/**
 * @file grayscale_quantizer.hpp
 * @brief Declares manual grayscale quantization.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::value {

/**
 * @brief Quantizes 8-bit grayscale images to a supported number of levels.
 *
 * @details The input must have type `CV_8UC1`. Supported level counts are
 * `2`, `4`, `8`, `16`, and `256`.
 *
 * A source intensity `v` is assigned to the bin
 * `floor(v * levels / 256)`. The bin index `k` is reconstructed with the
 * endpoint-preserving policy `round(k * 255 / (levels - 1))`.
 *
 * All intermediate calculations avoid accidental integer truncation. The
 * multiplication used for bin selection is safe for the supported 8-bit input
 * and level range.
 */
class GrayscaleQuantizer {
public:
    /**
     * @brief Quantizes a grayscale image manually.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param levels Requested number of reconstruction levels.
     * @return Independent quantized image with type `CV_8UC1`.
     *
     * @throws std::invalid_argument If the image is empty.
     * @throws std::invalid_argument If its depth is not `CV_8U`.
     * @throws std::invalid_argument If it does not contain exactly one channel.
     * @throws std::invalid_argument If `levels` is not a power of two in
     * `[2, 256]`.
     * @throws std::invalid_argument If `levels` is not one of the supported
     * values `2`, `4`, `8`, `16`, or `256`.
     */
    [[nodiscard]] cv::Mat quantize(
        const cv::Mat& input_image,
        int levels
    ) const;
};

} // namespace pdi::value
