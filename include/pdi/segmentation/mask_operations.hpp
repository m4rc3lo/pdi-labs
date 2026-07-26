/**
 * @file mask_operations.hpp
 * @brief Declares manual operations with binary masks.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::segmentation {

/**
 * @brief Identifies a binary mask operation independently of any interface.
 *
 * @details Applications may map command-line options, trackbars, buttons or
 * other controls to this enumeration without coupling the algorithms to GUI
 * code.
 */
enum class MaskOperation {
    Invert,
    Intersection,
    Union,
    Difference,
};

/**
 * @brief Performs manual logical operations and mask application.
 *
 * @details Binary masks must use type `CV_8UC1` and contain only `0` or `255`.
 * Logical operations are implemented by explicit pixel traversal and never use
 * OpenCV bitwise functions.
 */
class MaskOperations {
public:
    /**
     * @brief Inverts one binary mask.
     *
     * @param mask Source mask with type `CV_8UC1`.
     * @return Binary mask where `0` becomes `255` and `255` becomes `0`.
     *
     * @throws std::invalid_argument If the mask is empty, has an unsupported
     * type or contains a value different from `0` and `255`.
     */
    [[nodiscard]] cv::Mat invert(const cv::Mat& mask) const;

    /**
     * @brief Computes the intersection of two binary masks.
     *
     * @param first_mask First mask with type `CV_8UC1`.
     * @param second_mask Second mask with the same dimensions and type.
     * @return `255` only where both masks contain `255`.
     */
    [[nodiscard]] cv::Mat intersection(
        const cv::Mat& first_mask,
        const cv::Mat& second_mask
    ) const;

    /**
     * @brief Computes the union of two binary masks.
     *
     * @param first_mask First mask with type `CV_8UC1`.
     * @param second_mask Second mask with the same dimensions and type.
     * @return `255` where at least one mask contains `255`.
     */
    [[nodiscard]] cv::Mat union_of(
        const cv::Mat& first_mask,
        const cv::Mat& second_mask
    ) const;

    /**
     * @brief Computes the directional difference `first_mask \ second_mask`.
     *
     * @param first_mask First mask with type `CV_8UC1`.
     * @param second_mask Second mask with the same dimensions and type.
     * @return `255` only where the first mask contains `255` and the second
     * contains `0`.
     *
     * @note Difference is not commutative.
     */
    [[nodiscard]] cv::Mat difference(
        const cv::Mat& first_mask,
        const cv::Mat& second_mask
    ) const;

    /**
     * @brief Applies a binary mask to a grayscale image.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param mask Binary mask with matching dimensions.
     * @return Source intensity where the mask is `255`, otherwise `0`.
     */
    [[nodiscard]] cv::Mat apply_to_grayscale(
        const cv::Mat& input_image,
        const cv::Mat& mask
    ) const;

    /**
     * @brief Applies a binary mask to a BGR color image.
     *
     * @param input_image Source image with type `CV_8UC3`.
     * @param mask Binary mask with matching dimensions.
     * @return Source BGR pixel where the mask is `255`, otherwise black.
     */
    [[nodiscard]] cv::Mat apply_to_color(
        const cv::Mat& input_image,
        const cv::Mat& mask
    ) const;
};

} // namespace pdi::segmentation
