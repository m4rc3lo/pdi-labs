/**
 * @file m1_2_study_case.hpp
 * @brief Declares the preliminary applied study for Laboratory M1.2.
 */

#pragma once

#include <filesystem>
#include <vector>

#include <opencv2/core/mat.hpp>

#include "pdi/io/image_display.hpp"

namespace pdi::labs {

/**
 * @brief Stores the preliminary outputs and success indicator of the M1.2 study.
 */
struct M12StudyResult {
    cv::Mat input;
    cv::Mat grayscale_weighted;
    cv::Mat quantized_8;
    cv::Mat quantized_4;
    int distinct_quantized_levels{};
    bool success{};
};

/**
 * @brief Runs a preliminary study of illumination-zone simplification.
 *
 * @details The study converts a BGR image to weighted grayscale and quantizes
 * it to eight and four levels. The four-level result is considered
 * preliminarily successful when it preserves at least three distinct
 * intensity levels, representing dark, intermediate, and bright zones.
 *
 * This is a didactic feasibility study, not a complete illumination-analysis
 * or segmentation system.
 */
class M12StudyCase {
public:
    /**
     * @brief Executes the study and persists its preliminary artifacts.
     *
     * @param input_path Path of a color input image.
     * @param output_directory Directory for images and the CSV report.
     * @return Images, metric, and preliminary success status.
     *
     * @throws std::runtime_error If input or output operations fail.
     * @throws std::invalid_argument If an image precondition is violated.
     */
    [[nodiscard]] M12StudyResult run(
        const std::filesystem::path& input_path,
        const std::filesystem::path& output_directory
    ) const;

    /**
     * @brief Creates named images for optional graphical display.
     *
     * @param result Complete preliminary study result.
     * @return Ordered collection with unambiguous titles.
     */
    [[nodiscard]] std::vector<pdi::io::WindowImage> display_images(
        const M12StudyResult& result
    ) const;
};

} // namespace pdi::labs
