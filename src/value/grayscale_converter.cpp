/**
 * @file grayscale_converter.cpp
 * @brief Implements manual BGR-to-grayscale conversions.
 */

#include "pdi/value/grayscale_converter.hpp"

#include "pdi/core/image_validator.hpp"
#include "pdi/core/saturation.hpp"

#include <opencv2/core.hpp>

#include <cstdint>

namespace {

void validate_input(const cv::Mat& input_image) {
    pdi::core::ImageValidator::require_not_empty(input_image);
    pdi::core::ImageValidator::require_depth_8u(input_image);
    pdi::core::ImageValidator::require_channels(input_image, 3);
}

template <typename Conversion>
[[nodiscard]] cv::Mat convert_pixels(
    const cv::Mat& input_image,
    Conversion conversion
) {
    cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int row = 0; row < input_image.rows; ++row) {
        const auto* input_row = input_image.ptr<cv::Vec3b>(row);
        auto* output_row = output_image.ptr<std::uint8_t>(row);

        for (int col = 0; col < input_image.cols; ++col) {
            const cv::Vec3b pixel = input_row[col];
            output_row[col] = pdi::core::saturate_to_byte(conversion(pixel));
        }
    }

    return output_image;
}

} // namespace

namespace pdi::value {

cv::Mat GrayscaleConverter::convert_average(const cv::Mat& input_image) const {
    validate_input(input_image);

    return convert_pixels(input_image, [](const cv::Vec3b& pixel) {
        const double blue = static_cast<double>(pixel[0]);
        const double green = static_cast<double>(pixel[1]);
        const double red = static_cast<double>(pixel[2]);

        return (blue + green + red) / 3.0;
    });
}

cv::Mat GrayscaleConverter::convert_weighted(const cv::Mat& input_image) const {
    validate_input(input_image);

    return convert_pixels(input_image, [](const cv::Vec3b& pixel) {
        const double blue = static_cast<double>(pixel[0]);
        const double green = static_cast<double>(pixel[1]);
        const double red = static_cast<double>(pixel[2]);

        return (0.114 * blue) + (0.587 * green) + (0.299 * red);
    });
}

} // namespace pdi::value
