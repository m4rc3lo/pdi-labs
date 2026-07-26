/**
 * @file m1_3_pipeline.hpp
 * @brief Declares the integrated processing pipeline for Laboratory M1.3.
 */

#pragma once

#include "pdi/spatial/spatial_convolution.hpp"

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>

namespace pdi::labs {

/**
 * @brief Selects one integrated M1.3 operation.
 */
enum class M13Operation {
    Mean3x3,
    WeightedMean3x3,
    Mean5x5,
    Laplacian4,
    Laplacian8,
    Sobel,
};

/**
 * @brief Configures an integrated M1.3 execution.
 */
struct M13Parameters {
    M13Operation operation;
    spatial::BorderStrategy border_strategy;
    double enhancement_factor;
};

/**
 * @brief Identifies one generated image.
 */
struct M13Output {
    std::string name;
    cv::Mat image;
};

/**
 * @brief Executes one selected M1.3 operation without file-system coupling.
 */
class M13Pipeline {
public:
    /**
     * @brief Processes a grayscale image with the selected operation.
     *
     * @param input_image Source image with type `CV_8UC1`.
     * @param parameters Operation, border strategy and numeric parameters.
     * @return Identified visual outputs ready for persistence or display.
     */
    [[nodiscard]] std::vector<M13Output> run(
        const cv::Mat& input_image,
        const M13Parameters& parameters
    ) const;
};

} // namespace pdi::labs
