/**
 * @file image_inspector.hpp
 * @brief Declares manual structural and intensity inspection of OpenCV images.
 */

#pragma once

#include <opencv2/core/mat.hpp>

#include <cstdint>
#include <optional>

namespace pdi::value {

/**
 * @brief Stores intensity statistics for one scalar stream.
 *
 * @details For a grayscale image, the stream contains one value per pixel. For
 * a color image, a stream can represent all BGR samples together or one
 * specific channel.
 */
struct IntensityStatistics {
    std::uint8_t minimum{};
    std::uint8_t maximum{};
    std::uint64_t sum{};
    double mean{};
};

/**
 * @brief Stores statistics for the blue, green, and red OpenCV channels.
 *
 * @details OpenCV represents a standard three-channel color pixel as BGR.
 * Therefore, channel index 0 is blue, index 1 is green, and index 2 is red.
 */
struct BgrStatistics {
    IntensityStatistics blue;
    IntensityStatistics green;
    IntensityStatistics red;
};

/**
 * @brief Stores the complete result of an image inspection.
 *
 * @details The global intensity statistics use all scalar samples. A
 * `CV_8UC1` image contributes one sample per pixel. A `CV_8UC3` image
 * contributes three samples per pixel. Per-channel statistics are available
 * only for color images.
 */
struct ImageStatistics {
    int width{};
    int height{};
    std::uint64_t pixel_count{};
    int channel_count{};
    int opencv_type{};
    IntensityStatistics intensity;
    std::optional<BgrStatistics> bgr;
};

/**
 * @brief Calculates image metadata and intensity statistics manually.
 *
 * @details Pixel values are traversed using typed row pointers. Grayscale
 * images use `std::uint8_t*`, while color images use `cv::Vec3b*`. Color
 * channels are accessed directly as B, G, and R; no additional channel loop is
 * used.
 *
 * The implementation does not call `cv::minMaxLoc`, `cv::mean`,
 * `cv::calcHist`, or another ready-made image statistics operation.
 */
class ImageInspector {
public:
    /**
     * @brief Inspects a grayscale or BGR unsigned 8-bit image.
     *
     * @param image Input image with type `CV_8UC1` or `CV_8UC3`.
     * @return Typed metadata and intensity statistics.
     *
     * @throws std::invalid_argument If the image is empty.
     * @throws std::invalid_argument If the image depth is not `CV_8U`.
     * @throws std::invalid_argument If the image does not have one or three
     * channels.
     *
     * @post The input image remains unchanged.
     */
    [[nodiscard]] ImageStatistics inspect(const cv::Mat& image) const;
};

} // namespace pdi::value
