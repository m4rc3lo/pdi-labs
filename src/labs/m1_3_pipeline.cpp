/**
 * @file m1_3_pipeline.cpp
 * @brief Implements the integrated Laboratory M1.3 pipeline.
 */

#include "pdi/labs/m1_3_pipeline.hpp"

#include "pdi/spatial/laplacian_enhancement.hpp"
#include "pdi/spatial/smoothing_filters.hpp"
#include "pdi/spatial/sobel_operator.hpp"

#include <stdexcept>
#include <vector>

namespace pdi::labs {

std::vector<M13Output> M13Pipeline::run(
    const cv::Mat& input_image,
    const M13Parameters& parameters
) const {
    const spatial::SmoothingFilters smoothing;

    switch (parameters.operation) {
    case M13Operation::Mean3x3:
        return {{
            "mean_3x3",
            smoothing.mean_3x3(
                input_image,
                parameters.border_strategy
            ),
        }};

    case M13Operation::WeightedMean3x3:
        return {{
            "weighted_mean_3x3",
            smoothing.weighted_mean_3x3(
                input_image,
                parameters.border_strategy
            ),
        }};

    case M13Operation::Mean5x5:
        return {{
            "mean_5x5",
            smoothing.mean_5x5(
                input_image,
                parameters.border_strategy
            ),
        }};

    case M13Operation::Laplacian4: {
        const auto result = spatial::LaplacianEnhancement{}.apply(
            input_image,
            spatial::LaplacianKernel::FourNeighbor,
            parameters.enhancement_factor,
            parameters.border_strategy
        );

        return {
            {"laplacian_4_response", result.response_visualization},
            {"laplacian_4_enhanced", result.enhanced_image},
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
            {"laplacian_8_response", result.response_visualization},
            {"laplacian_8_enhanced", result.enhanced_image},
        };
    }

    case M13Operation::Sobel: {
        const auto result = spatial::SobelOperator{}.apply(
            input_image,
            parameters.border_strategy
        );

        return {
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
        };
    }
    }

    throw std::invalid_argument("Unsupported M1.3 operation.");
}

} // namespace pdi::labs
