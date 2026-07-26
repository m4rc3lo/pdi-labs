/**
 * @file m2_1_pipeline.hpp
 * @brief Declares the integrated Laboratory M2.1 processing pipeline.
 */

#pragma once

#include "pdi/io/processing_record.hpp"

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>

namespace pdi::labs {

/**
 * @brief Identifies one operation exposed by the integrated M2.1 application.
 */
enum class M21Operation {
    ManualGlobal,
    ManualInterval,
    MaskInvert,
    MaskIntersection,
    MaskUnion,
    MaskDifference,
    MaskApplyGrayscale,
    MaskApplyColor,
    BgrToHsv,
    Otsu,
    AdaptiveMean,
    DistanceTransform,
    Watershed,
};

/**
 * @brief Stores all configurable M2.1 parameters independently of CLI and GUI.
 */
struct M21Parameters {
    M21Operation operation;
    int threshold = 128;
    int minimum_value = 80;
    int maximum_value = 160;
    int adaptive_block_size = 11;
    double adaptive_constant = 2.0;
    int distance_mask_size = 3;
    double foreground_ratio = 0.4;
};

/**
 * @brief Associates one stable visual-output name with an image.
 */
struct M21VisualOutput {
    std::string name;
    cv::Mat image;
};

/**
 * @brief Separates visual outputs from numeric artifacts and parameters.
 */
struct M21PipelineResult {
    std::string operation_name;
    std::vector<M21VisualOutput> visual_outputs;
    std::vector<pdi::io::NamedValue> parameters;
    std::vector<pdi::io::NamedMatrix> numeric_artifacts;
};

/**
 * @brief Integrates the manual and library-based examples of Laboratory M2.1.
 *
 * @details The pipeline contains no file-system, YAML or HighGUI calls. The
 * optional second input represents another mask or the mask applied to an
 * image, depending on the selected operation.
 */
class M21Pipeline {
public:
    /**
     * @brief Runs one M2.1 operation.
     *
     * @param input_image Primary image or first mask.
     * @param secondary_image Optional second mask.
     * @param parameters Selected operation and parameters.
     * @return Named visual outputs, parameters and numeric artifacts.
     */
    [[nodiscard]] M21PipelineResult run(
        const cv::Mat& input_image,
        const cv::Mat& secondary_image,
        const M21Parameters& parameters
    ) const;

    /** @brief Returns whether the operation requires a color primary image. */
    [[nodiscard]] static bool requires_color_input(M21Operation operation);

    /** @brief Returns whether the operation requires a secondary mask. */
    [[nodiscard]] static bool requires_secondary_input(M21Operation operation);
};

} // namespace pdi::labs
