/**
 * @file spatial_convolution.hpp
 * @brief Declares generic manual spatial convolution for grayscale images.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::spatial {

/**
 * @brief Applies a floating-point square kernel to an 8-bit grayscale image.
 *
 * @details The public operation is named `convolution` for consistency with
 * the terminology adopted by the laboratory. The implementation does not
 * rotate the kernel by 180 degrees; therefore, the computed operation is
 * technically spatial correlation.
 *
 * For symmetric kernels, rotating the kernel does not change its coefficients.
 * In those cases, this implementation produces the same numeric result as the
 * mathematical definition of convolution.
 *
 * Only pixels whose complete neighborhood fits inside the image are processed.
 * Border pixels that do not have a complete neighborhood remain zero in the
 * output image.
 */
class SpatialConvolution {
public:
    /**
     * @brief Applies the supplied kernel without rotating it.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param kernel Square, odd-sized, single-channel floating-point matrix
     * with type `CV_32FC1` or `CV_64FC1`.
     * @param normalize_kernel When true, divides the accumulated response by
     * the sum of all kernel coefficients.
     * @return Image with type `CV_8UC1` and the same dimensions as the input.
     *
     * @throws std::invalid_argument If the input image is empty.
     * @throws std::invalid_argument If the input image is not `CV_8UC1`.
     * @throws std::invalid_argument If the kernel is empty.
     * @throws std::invalid_argument If the kernel is not square.
     * @throws std::invalid_argument If the kernel dimension is even.
     * @throws std::invalid_argument If the kernel is not single-channel
     * floating point.
     * @throws std::invalid_argument If the kernel is larger than the image.
     * @throws std::invalid_argument If normalization is requested and the
     * kernel sum is zero or numerically close to zero.
     *
     * @note The kernel is applied in the order in which it is stored. No
     * 180-degree rotation is performed.
     * @note The internal accumulation uses `double`.
     * @note The result is rounded and saturated to the interval `[0, 255]`.
     *
     * @par Complexity
     * For an input image with `M` rows and `N` columns and a `K x K` kernel,
     * the time complexity is `O(M N K^2)`. The output matrix requires
     * `O(M N)` additional space.
     */
    [[nodiscard]] cv::Mat convolution(
        const cv::Mat& input_image,
        const cv::Mat& kernel,
        bool normalize_kernel = false
    ) const;
};

} // namespace pdi::spatial
