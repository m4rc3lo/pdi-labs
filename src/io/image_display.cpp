/**
 * @file image_display.cpp
 * @brief Implements optional display of named images.
 */

#include "pdi/io/image_display.hpp"

#include <opencv2/highgui.hpp>

#include <stdexcept>
#include <string>

namespace pdi::io {

void ImageDisplay::show_all(const std::vector<WindowImage>& images) const {
    if (images.empty()) {
        throw std::invalid_argument(
            "Image display failed: no images were provided."
        );
    }

    for (const WindowImage& item : images) {
        if (item.title.empty()) {
            throw std::invalid_argument(
                "Image display failed: a window title is empty."
            );
        }

        if (item.image.empty()) {
            throw std::invalid_argument(
                "Image display failed: image for window '"
                + item.title
                + "' is empty."
            );
        }
    }

    try {
        for (const WindowImage& item : images) {
            cv::namedWindow(item.title, cv::WINDOW_AUTOSIZE);
            cv::imshow(item.title, item.image);
        }

        cv::waitKey(0);
        cv::destroyAllWindows();
    } catch (const cv::Exception& exception) {
        cv::destroyAllWindows();

        throw std::runtime_error(
            "Image display failed: " + std::string{exception.what()}
        );
    }
}

} // namespace pdi::io
