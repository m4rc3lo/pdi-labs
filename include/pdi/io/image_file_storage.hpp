/**
 * @file image_file_storage.hpp
 * @brief Declares shared image loading and persistence.
 */

#pragma once

#include <opencv2/core/mat.hpp>

#include <filesystem>

namespace pdi::io {

/**
 * @brief Loads and saves images used by laboratory applications.
 *
 * @details This component centralizes OpenCV image-codec operations and
 * filesystem preparation. It is infrastructure only and does not perform image
 * processing.
 */
class ImageFileStorage {
public:
    /**
     * @brief Loads a color image in OpenCV BGR order.
     *
     * @param input_path Path of the image to load.
     * @return Non-empty image with three BGR channels.
     *
     * @throws std::runtime_error If the image cannot be read.
     */
    [[nodiscard]] cv::Mat load_color(
        const std::filesystem::path& input_path
    ) const;

    /**
     * @brief Saves an image after creating its parent directory.
     *
     * @param output_path Destination path, including file name and extension.
     * @param image Non-empty image to save.
     *
     * @throws std::invalid_argument If the image is empty.
     * @throws std::runtime_error If directory creation or image writing fails.
     */
    void save(
        const std::filesystem::path& output_path,
        const cv::Mat& image
    ) const;
};

} // namespace pdi::io
