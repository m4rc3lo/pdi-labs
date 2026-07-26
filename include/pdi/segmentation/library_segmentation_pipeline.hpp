/**
 * @file library_segmentation_pipeline.hpp
 * @brief Declares documented segmentation examples based on OpenCV library calls.
 */

#pragma once

#include "pdi/io/processing_record.hpp"

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>

namespace pdi::segmentation {

/**
 * @brief Identifies one library-based segmentation example.
 */
enum class LibrarySegmentationOperation {
    BgrToHsv,
    Otsu,
    AdaptiveMean,
    DistanceTransform,
    Watershed,
};

/**
 * @brief Stores configurable parameters independently of CLI or GUI.
 */
struct LibrarySegmentationParameters {
    LibrarySegmentationOperation operation;
    int adaptive_block_size = 11;
    double adaptive_constant = 2.0;
    double distance_mask_size = 3.0;
    double foreground_ratio = 0.4;
};

/**
 * @brief Associates a visual output name with an image.
 */
struct SegmentationVisualOutput {
    std::string name;
    cv::Mat image;
};

/**
 * @brief Separates visual outputs, parameters and numeric artifacts.
 */
struct LibrarySegmentationResult {
    std::string operation_name;
    std::string data_file_name;
    std::vector<SegmentationVisualOutput> visual_outputs;
    std::vector<pdi::io::NamedValue> parameters;
    std::vector<pdi::io::NamedMatrix> numeric_artifacts;
};

/**
 * @brief Runs explicit, documented OpenCV segmentation pipelines.
 *
 * @details This class intentionally contains library-based examples and is
 * separate from ManualThreshold and MaskOperations. It contains no HighGUI,
 * file-system or YAML persistence calls.
 */
class LibrarySegmentationPipeline {
public:
    /**
     * @brief Runs the selected operation.
     *
     * @param input_image `CV_8UC3` for HSV and Watershed; `CV_8UC1` for the
     * other operations.
     * @param parameters Operation and algorithm parameters.
     * @return Visual outputs, textual parameters and numeric artifacts.
     */
    [[nodiscard]] LibrarySegmentationResult run(
        const cv::Mat& input_image,
        const LibrarySegmentationParameters& parameters
    ) const;
};

} // namespace pdi::segmentation
