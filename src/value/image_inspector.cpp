/**
 * @file image_inspector.cpp
 * @brief Implements manual structural and intensity inspection of images.
 */

#include "pdi/value/image_inspector.hpp"

#include "pdi/core/image_validator.hpp"

#include <opencv2/core.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace {

struct StatisticsAccumulator {
    std::uint8_t minimum{255};
    std::uint8_t maximum{0};
    std::uint64_t sum{0};
    std::uint64_t sample_count{0};
};

void add_sample(StatisticsAccumulator& accumulator, const std::uint8_t value) noexcept {
    if (value < accumulator.minimum) {
        accumulator.minimum = value;
    }

    if (value > accumulator.maximum) {
        accumulator.maximum = value;
    }

    accumulator.sum += value;
    ++accumulator.sample_count;
}

[[nodiscard]] pdi::value::IntensityStatistics finalize(
    const StatisticsAccumulator& accumulator
) noexcept {
    return pdi::value::IntensityStatistics{
        .minimum = accumulator.minimum,
        .maximum = accumulator.maximum,
        .sum = accumulator.sum,
        .mean = static_cast<double>(accumulator.sum)
            / static_cast<double>(accumulator.sample_count),
    };
}

} // namespace

namespace pdi::value {

ImageStatistics ImageInspector::inspect(const cv::Mat& image) const {
    core::ImageValidator::require_not_empty(image);
    core::ImageValidator::require_depth_8u(image);

    const int channel_count = image.channels();

    if (channel_count != 1 && channel_count != 3) {
        throw std::invalid_argument(
            "Image inspection failed: expected CV_8UC1 or CV_8UC3, but received "
            + std::to_string(channel_count)
            + " channel(s)."
        );
    }

    const auto pixel_count = static_cast<std::uint64_t>(image.rows)
        * static_cast<std::uint64_t>(image.cols);

    StatisticsAccumulator global_accumulator;

    if (channel_count == 1) {
        for (int row = 0; row < image.rows; ++row) {
            const auto* row_ptr = image.ptr<std::uint8_t>(row);

            for (int col = 0; col < image.cols; ++col) {
                add_sample(global_accumulator, row_ptr[col]);
            }
        }

        return ImageStatistics{
            .width = image.cols,
            .height = image.rows,
            .pixel_count = pixel_count,
            .channel_count = channel_count,
            .opencv_type = image.type(),
            .intensity = finalize(global_accumulator),
            .bgr = std::nullopt,
        };
    }

    StatisticsAccumulator blue_accumulator;
    StatisticsAccumulator green_accumulator;
    StatisticsAccumulator red_accumulator;

    for (int row = 0; row < image.rows; ++row) {
        const auto* row_ptr = image.ptr<cv::Vec3b>(row);

        for (int col = 0; col < image.cols; ++col) {
            const cv::Vec3b pixel = row_ptr[col];
            const std::uint8_t blue = pixel[0];
            const std::uint8_t green = pixel[1];
            const std::uint8_t red = pixel[2];

            add_sample(global_accumulator, blue);
            add_sample(global_accumulator, green);
            add_sample(global_accumulator, red);

            add_sample(blue_accumulator, blue);
            add_sample(green_accumulator, green);
            add_sample(red_accumulator, red);
        }
    }

    return ImageStatistics{
        .width = image.cols,
        .height = image.rows,
        .pixel_count = pixel_count,
        .channel_count = channel_count,
        .opencv_type = image.type(),
        .intensity = finalize(global_accumulator),
        .bgr = BgrStatistics{
            .blue = finalize(blue_accumulator),
            .green = finalize(green_accumulator),
            .red = finalize(red_accumulator),
        },
    };
}

} // namespace pdi::value
