/**
 * @file morphological_pipeline.hpp
 * @brief Declares a configurable and traceable binary morphology pipeline.
 */

#pragma once

#include "pdi/io/processing_record.hpp"
#include "pdi/morphology/binary_morphology.hpp"
#include "pdi/segmentation/connected_component_labeler.hpp"

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>

namespace pdi::morphology {

/**
 * @brief Identifies one manual morphology operation in a pipeline.
 */
enum class MorphologicalOperation {
    Erode,
    Dilate,
    Open,
    Close,
};

/**
 * @brief Configures one reproducible morphology pipeline.
 */
struct MorphologicalPipelineConfig {
    BinaryStructuringElement element;
    std::vector<MorphologicalOperation> sequence;
    MorphologyBorderStrategy border_strategy =
        MorphologyBorderStrategy::OutsideBackground;
    pdi::segmentation::Connectivity connectivity =
        pdi::segmentation::Connectivity::Eight;
    bool compare_with_opencv = false;

    /**
     * @brief Opening-oriented preset for removal of isolated foreground noise.
     */
    [[nodiscard]] static MorphologicalPipelineConfig noise_removal_3x3();

    /**
     * @brief Closing-oriented preset for filling small foreground holes.
     */
    [[nodiscard]] static MorphologicalPipelineConfig hole_filling_3x3();
};

/**
 * @brief Stores metrics, intermediate matrices, and optional validation.
 */
struct MorphologicalPipelineResult {
    cv::Mat input;
    cv::Mat output;
    cv::Mat initial_labels;
    cv::Mat final_labels;
    std::vector<cv::Mat> intermediate_images;
    int foreground_area_before = 0;
    int foreground_area_after = 0;
    int component_count_before = 0;
    int component_count_after = 0;
    int removed_component_count = 0;
    std::vector<int> removed_component_labels;
    bool compared_with_opencv = false;
    bool matches_opencv = false;
};

/**
 * @brief Executes manual morphology sequences and computes traceability data.
 */
class MorphologicalPipeline {
public:
    /**
     * @brief Runs a sequence containing at least two operations.
     */
    [[nodiscard]] MorphologicalPipelineResult run(
        const cv::Mat& binary_image,
        const MorphologicalPipelineConfig& config
    ) const;

    /**
     * @brief Converts one execution into the generic YAML record model.
     */
    [[nodiscard]] pdi::io::ProcessingRecord make_processing_record(
        const MorphologicalPipelineResult& result,
        const MorphologicalPipelineConfig& config,
        const std::string& project_version,
        const std::string& input_path
    ) const;

    [[nodiscard]] static std::string operation_name(
        MorphologicalOperation operation
    );

    [[nodiscard]] static std::string sequence_name(
        const std::vector<MorphologicalOperation>& sequence
    );
};

} // namespace pdi::morphology
