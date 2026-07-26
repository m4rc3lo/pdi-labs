/**
 * @file library_segmentation_pipeline.cpp
 * @brief Implements explicit OpenCV-based segmentation examples.
 */

#include "pdi/segmentation/library_segmentation_pipeline.hpp"

#include "pdi/core/image_validator.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void require_grayscale(const cv::Mat& image) {
    pdi::core::ImageValidator::require_not_empty(image);
    pdi::core::ImageValidator::require_depth_8u(image);
    pdi::core::ImageValidator::require_channels(image, 1);
}

void require_color(const cv::Mat& image) {
    pdi::core::ImageValidator::require_not_empty(image);
    pdi::core::ImageValidator::require_depth_8u(image);
    pdi::core::ImageValidator::require_channels(image, 3);
}

cv::Mat normalize_float_visualization(const cv::Mat& input) {
    cv::Mat output;
    cv::normalize(input, output, 0.0, 255.0, cv::NORM_MINMAX, CV_8UC1);
    return output;
}

std::string number(double value) {
    return std::to_string(value);
}

} // namespace

namespace pdi::segmentation {

LibrarySegmentationResult LibrarySegmentationPipeline::run(
    const cv::Mat& input_image,
    const LibrarySegmentationParameters& parameters
) const {
    switch (parameters.operation) {
    case LibrarySegmentationOperation::BgrToHsv: {
        require_color(input_image);

        cv::Mat hsv;
        cv::cvtColor(input_image, hsv, cv::COLOR_BGR2HSV);

        std::vector<cv::Mat> channels;
        cv::split(hsv, channels);

        cv::Mat hsv_visualization;
        cv::cvtColor(hsv, hsv_visualization, cv::COLOR_HSV2BGR);

        return {
            .operation_name = "bgr_to_hsv",
            .data_file_name = "bgr_to_hsv_result.yml",
            .visual_outputs = {
                {"hsv_as_bgr", hsv_visualization},
                {"hue", channels[0]},
                {"saturation", channels[1]},
                {"value", channels[2]},
            },
            .parameters = {
                {"conversion", "COLOR_BGR2HSV"},
                {"hue_range", "[0, 179]"},
                {"saturation_range", "[0, 255]"},
                {"value_range", "[0, 255]"},
            },
            .numeric_artifacts = {
                {"hsv", hsv},
            },
        };
    }

    case LibrarySegmentationOperation::Otsu: {
        require_grayscale(input_image);

        cv::Mat binary;
        const double selected_threshold = cv::threshold(
            input_image,
            binary,
            0.0,
            255.0,
            cv::THRESH_BINARY | cv::THRESH_OTSU
        );

        return {
            .operation_name = "otsu",
            .data_file_name = "otsu_result.yml",
            .visual_outputs = {
                {"otsu_binary", binary},
            },
            .parameters = {
                {"threshold_type", "THRESH_BINARY|THRESH_OTSU"},
                {"selected_threshold", number(selected_threshold)},
                {"maximum_value", "255"},
            },
            .numeric_artifacts = {},
        };
    }

    case LibrarySegmentationOperation::AdaptiveMean: {
        require_grayscale(input_image);

        if (parameters.adaptive_block_size < 3
            || parameters.adaptive_block_size % 2 == 0) {
            throw std::invalid_argument(
                "Adaptive block size must be odd and at least 3."
            );
        }

        cv::Mat binary;
        cv::adaptiveThreshold(
            input_image,
            binary,
            255.0,
            cv::ADAPTIVE_THRESH_MEAN_C,
            cv::THRESH_BINARY,
            parameters.adaptive_block_size,
            parameters.adaptive_constant
        );

        return {
            .operation_name = "adaptive_mean",
            .data_file_name = "adaptive_mean_result.yml",
            .visual_outputs = {
                {"adaptive_mean_binary", binary},
            },
            .parameters = {
                {"adaptive_method", "ADAPTIVE_THRESH_MEAN_C"},
                {"threshold_type", "THRESH_BINARY"},
                {"block_size",
                 std::to_string(parameters.adaptive_block_size)},
                {"constant_c", number(parameters.adaptive_constant)},
                {"maximum_value", "255"},
            },
            .numeric_artifacts = {},
        };
    }

    case LibrarySegmentationOperation::DistanceTransform: {
        require_grayscale(input_image);

        if (parameters.distance_mask_size != 3.0
            && parameters.distance_mask_size != 5.0) {
            throw std::invalid_argument(
                "Distance mask size must be 3 or 5."
            );
        }

        cv::Mat distance;
        cv::distanceTransform(
            input_image,
            distance,
            cv::DIST_L2,
            static_cast<int>(parameters.distance_mask_size)
        );

        return {
            .operation_name = "distance_transform",
            .data_file_name = "distance_transform_result.yml",
            .visual_outputs = {
                {"distance_normalized",
                 normalize_float_visualization(distance)},
            },
            .parameters = {
                {"distance_type", "DIST_L2"},
                {"mask_size",
                 std::to_string(
                     static_cast<int>(parameters.distance_mask_size)
                 )},
            },
            .numeric_artifacts = {
                {"distance", distance},
            },
        };
    }

    case LibrarySegmentationOperation::Watershed: {
        require_color(input_image);

        if (parameters.foreground_ratio <= 0.0
            || parameters.foreground_ratio >= 1.0) {
            throw std::invalid_argument(
                "Watershed foreground ratio must be in (0, 1)."
            );
        }

        cv::Mat grayscale;
        cv::cvtColor(input_image, grayscale, cv::COLOR_BGR2GRAY);

        cv::Mat binary_inverse;
        const double otsu_threshold = cv::threshold(
            grayscale,
            binary_inverse,
            0.0,
            255.0,
            cv::THRESH_BINARY_INV | cv::THRESH_OTSU
        );

        cv::Mat distance;
        cv::distanceTransform(
            binary_inverse,
            distance,
            cv::DIST_L2,
            5
        );

        double maximum_distance = 0.0;
        cv::minMaxLoc(distance, nullptr, &maximum_distance);

        cv::Mat sure_foreground_float;
        cv::threshold(
            distance,
            sure_foreground_float,
            maximum_distance * parameters.foreground_ratio,
            255.0,
            cv::THRESH_BINARY
        );

        cv::Mat sure_foreground;
        sure_foreground_float.convertTo(sure_foreground, CV_8UC1);

        cv::Mat sure_background;
        cv::dilate(
            binary_inverse,
            sure_background,
            cv::Mat(),
            cv::Point{-1, -1},
            3
        );

        cv::Mat unknown = sure_background.clone();
        for (int row = 0; row < unknown.rows; ++row) {
            const auto* foreground_row =
                sure_foreground.ptr<std::uint8_t>(row);
            auto* unknown_row = unknown.ptr<std::uint8_t>(row);

            for (int col = 0; col < unknown.cols; ++col) {
                unknown_row[col] =
                    unknown_row[col] == 255
                    && foreground_row[col] == 0
                        ? static_cast<std::uint8_t>(255)
                        : static_cast<std::uint8_t>(0);
            }
        }

        cv::Mat markers;
        const int component_count =
            cv::connectedComponents(sure_foreground, markers);

        for (int row = 0; row < markers.rows; ++row) {
            auto* marker_row = markers.ptr<int>(row);
            const auto* unknown_row =
                unknown.ptr<std::uint8_t>(row);

            for (int col = 0; col < markers.cols; ++col) {
                marker_row[col] += 1;
                if (unknown_row[col] == 255) {
                    marker_row[col] = 0;
                }
            }
        }

        cv::watershed(input_image, markers);

        cv::Mat visualization = input_image.clone();
        for (int row = 0; row < markers.rows; ++row) {
            const auto* marker_row = markers.ptr<int>(row);
            auto* output_row = visualization.ptr<cv::Vec3b>(row);

            for (int col = 0; col < markers.cols; ++col) {
                if (marker_row[col] == -1) {
                    output_row[col][0] = 0;
                    output_row[col][1] = 0;
                    output_row[col][2] = 255;
                }
            }
        }

        return {
            .operation_name = "watershed",
            .data_file_name = "watershed_result.yml",
            .visual_outputs = {
                {"grayscale", grayscale},
                {"otsu_inverse", binary_inverse},
                {"distance_normalized",
                 normalize_float_visualization(distance)},
                {"sure_foreground", sure_foreground},
                {"sure_background", sure_background},
                {"unknown", unknown},
                {"watershed_boundaries", visualization},
            },
            .parameters = {
                {"grayscale_conversion", "COLOR_BGR2GRAY"},
                {"threshold_type", "THRESH_BINARY_INV|THRESH_OTSU"},
                {"selected_otsu_threshold", number(otsu_threshold)},
                {"distance_type", "DIST_L2"},
                {"distance_mask_size", "5"},
                {"foreground_ratio",
                 number(parameters.foreground_ratio)},
                {"background_dilation_iterations", "3"},
                {"initial_components",
                 std::to_string(component_count)},
            },
            .numeric_artifacts = {
                {"distance", distance},
                {"sure_foreground_float", sure_foreground_float},
                {"markers", markers},
            },
        };
    }
    }

    throw std::invalid_argument(
        "Unsupported library segmentation operation."
    );
}

} // namespace pdi::segmentation
