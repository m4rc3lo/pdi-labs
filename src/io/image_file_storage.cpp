/**
 * @file image_file_storage.cpp
 * @brief Implements shared image loading and persistence.
 */

#include "pdi/io/image_file_storage.hpp"

#include <opencv2/imgcodecs.hpp>

#include <filesystem>
#include <stdexcept>
#include <string>
#include <system_error>

namespace pdi::io {

cv::Mat ImageFileStorage::load_color(
    const std::filesystem::path& input_path
) const {
    const cv::Mat image =
        cv::imread(input_path.string(), cv::IMREAD_COLOR);

    if (image.empty()) {
        throw std::runtime_error(
            "Could not read color image: " + input_path.string()
        );
    }

    return image;
}

cv::Mat ImageFileStorage::load_grayscale(
    const std::filesystem::path& input_path
) const {
    const cv::Mat image =
        cv::imread(input_path.string(), cv::IMREAD_GRAYSCALE);

    if (image.empty()) {
        throw std::runtime_error(
            "Could not read grayscale image: " + input_path.string()
        );
    }

    return image;
}

void ImageFileStorage::save(
    const std::filesystem::path& output_path,
    const cv::Mat& image
) const {
    if (image.empty()) {
        throw std::invalid_argument(
            "Could not write empty image: " + output_path.string()
        );
    }

    const std::filesystem::path parent_path = output_path.parent_path();

    if (!parent_path.empty()) {
        std::error_code error;
        std::filesystem::create_directories(parent_path, error);

        if (error) {
            throw std::runtime_error(
                "Could not create output directory '"
                + parent_path.string()
                + "': "
                + error.message()
            );
        }
    }

    try {
        if (!cv::imwrite(output_path.string(), image)) {
            throw std::runtime_error(
                "Could not write image: " + output_path.string()
            );
        }
    } catch (const cv::Exception& exception) {
        throw std::runtime_error(
            "Could not write image '"
            + output_path.string()
            + "': "
            + exception.what()
        );
    }
}

} // namespace pdi::io
