/**
 * @file m1_3_pipeline.cpp
 * @brief Implements the integrated Laboratory M1.3 pipeline.
 */

#include "pdi/labs/m1_3_pipeline.hpp"

#include "pdi/spatial/laplacian_enhancement.hpp"
#include "pdi/spatial/smoothing_filters.hpp"
#include "pdi/spatial/sobel_operator.hpp"
#include "pdi/spatial/spatial_kernels.hpp"

#include <opencv2/core.hpp>

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

[[nodiscard]] std::string border_name(
    pdi::spatial::BorderStrategy strategy
) {
    return strategy == pdi::spatial::BorderStrategy::CopyBorder
        ? "copy"
        : "replicate";
}

[[nodiscard]] std::string number_text(double value) {
    std::ostringstream stream;
    stream << std::setprecision(17) << value;
    return stream.str();
}

} // namespace

namespace pdi::labs {

M13PipelineResult M13Pipeline::run(
    const cv::Mat& input_image,
    const M13Parameters& parameters
) const {
    const spatial::SmoothingFilters smoothing;
    const std::string border = border_name(parameters.border_strategy);

    switch (parameters.operation) {
    case M13Operation::Mean3x3:
        return {
            .operation_name = "mean_3x3",
            .data_file_name = "mean_3x3_result.yml",
            .visual_outputs = {{
                "mean_3x3",
                smoothing.mean_3x3(
                    input_image,
                    parameters.border_strategy
                ),
            }},
            .parameters = {
                {"border_strategy", border},
                {"kernel_name", "mean_3x3"},
                {"kernel_sum", "9"},
            },
            .numeric_artifacts = {{"kernel", spatial::SpatialKernels::mean_3x3()}},
        };

    case M13Operation::WeightedMean3x3:
        return {
            .operation_name = "weighted_mean_3x3",
            .data_file_name = "weighted_mean_3x3_result.yml",
            .visual_outputs = {{
                "weighted_mean_3x3",
                smoothing.weighted_mean_3x3(
                    input_image,
                    parameters.border_strategy
                ),
            }},
            .parameters = {
                {"border_strategy", border},
                {"kernel_name", "weighted_mean_3x3"},
                {"kernel_sum", "16"},
            },
            .numeric_artifacts = {{
                "kernel",
                spatial::SpatialKernels::weighted_mean_3x3(),
            }},
        };

    case M13Operation::Mean5x5:
        return {
            .operation_name = "mean_5x5",
            .data_file_name = "mean_5x5_result.yml",
            .visual_outputs = {{
                "mean_5x5",
                smoothing.mean_5x5(
                    input_image,
                    parameters.border_strategy
                ),
            }},
            .parameters = {
                {"border_strategy", border},
                {"kernel_name", "mean_5x5"},
                {"kernel_sum", "25"},
            },
            .numeric_artifacts = {{"kernel", spatial::SpatialKernels::mean_5x5()}},
        };

    case M13Operation::Laplacian4: {
        const auto result = spatial::LaplacianEnhancement{}.apply(
            input_image,
            spatial::LaplacianKernel::FourNeighbor,
            parameters.enhancement_factor,
            parameters.border_strategy
        );

        return {
            .operation_name = "laplacian_4",
            .data_file_name = "laplacian_4_result.yml",
            .visual_outputs = {
                {"laplacian_4_response", result.response_visualization},
                {"laplacian_4_enhanced", result.enhanced_image},
            },
            .parameters = {
                {"border_strategy", border},
                {"kernel_name", "laplacian_4"},
                {
                    "enhancement_factor",
                    number_text(parameters.enhancement_factor),
                },
            },
            .numeric_artifacts = {
                {"kernel", spatial::SpatialKernels::laplacian_4()},
                {"raw_response", result.raw_response},
                {"enhanced_raw", result.enhanced_raw},
            },
        };
    }

    case M13Operation::Laplacian8: {
        const auto result = spatial::LaplacianEnhancement{}.apply(
            input_image,
            spatial::LaplacianKernel::EightNeighbor,
            parameters.enhancement_factor,
            parameters.border_strategy
        );

        return {
            .operation_name = "laplacian_8",
            .data_file_name = "laplacian_8_result.yml",
            .visual_outputs = {
                {"laplacian_8_response", result.response_visualization},
                {"laplacian_8_enhanced", result.enhanced_image},
            },
            .parameters = {
                {"border_strategy", border},
                {"kernel_name", "laplacian_8"},
                {
                    "enhancement_factor",
                    number_text(parameters.enhancement_factor),
                },
            },
            .numeric_artifacts = {
                {"kernel", spatial::SpatialKernels::laplacian_8()},
                {"raw_response", result.raw_response},
                {"enhanced_raw", result.enhanced_raw},
            },
        };
    }

    case M13Operation::Sobel: {
        const auto result = spatial::SobelOperator{}.apply(
            input_image,
            parameters.border_strategy
        );

        return {
            .operation_name = "sobel",
            .data_file_name = "sobel_result.yml",
            .visual_outputs = {
                {"sobel_gx", result.gradient_x_visualization},
                {"sobel_gy", result.gradient_y_visualization},
                {
                    "sobel_magnitude_approximate",
                    result.magnitude_approximate_visualization,
                },
                {
                    "sobel_magnitude_euclidean",
                    result.magnitude_euclidean_visualization,
                },
            },
            .parameters = {{"border_strategy", border}},
            .numeric_artifacts = {
                {"kernel_gx", spatial::SpatialKernels::sobel_x()},
                {"kernel_gy", spatial::SpatialKernels::sobel_y()},
                {"gradient_x", result.gradient_x},
                {"gradient_y", result.gradient_y},
                {
                    "magnitude_approximate",
                    result.magnitude_approximate,
                },
                {
                    "magnitude_euclidean",
                    result.magnitude_euclidean,
                },
            },
        };
    }
    }

    throw std::invalid_argument("Unsupported M1.3 operation.");
}

} // namespace pdi::labs
