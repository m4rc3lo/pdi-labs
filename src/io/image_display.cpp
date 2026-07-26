/**
 * @file image_display.cpp
 * @brief Implements optional display of named images.
 */

#include "pdi/io/image_display.hpp"

#include "pdi/windowing/window_lifecycle.hpp"

#include <opencv2/highgui.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

[[nodiscard]] pdi::windowing::WindowVisibility query_visibility(
    const std::string& window_name
) noexcept {
    try {
        const double value = cv::getWindowProperty(
            window_name,
            cv::WND_PROP_VISIBLE
        );
        return pdi::windowing::WindowLifecycleState::
            interpret_visibility(value);
    } catch (const cv::Exception&) {
        return pdi::windowing::WindowVisibility::Unavailable;
    }
}

void destroy_all_windows_safely() noexcept {
    try {
        cv::destroyAllWindows();
        static_cast<void>(cv::waitKey(1));
    } catch (const cv::Exception&) {
    }
}

} // namespace

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
        std::vector<std::string> active_windows;
        active_windows.reserve(images.size());

        for (const WindowImage& item : images) {
            cv::namedWindow(item.title, cv::WINDOW_AUTOSIZE);
            cv::imshow(item.title, item.image);
            active_windows.push_back(item.title);
        }

        pdi::windowing::WindowLifecycleState lifecycle;

        while (lifecycle.should_continue()
               && !active_windows.empty()) {
            lifecycle.observe_key_code(cv::waitKey(20));

            std::erase_if(
                active_windows,
                [](const std::string& window_name) {
                    return query_visibility(window_name)
                        != pdi::windowing::WindowVisibility::Visible;
                }
            );

            if (active_windows.empty()) {
                lifecycle.request_close(
                    pdi::windowing::WindowCloseReason::WindowClosed
                );
            }
        }

        destroy_all_windows_safely();
    } catch (const cv::Exception& exception) {
        destroy_all_windows_safely();

        throw std::runtime_error(
            "Image display failed: " + std::string{exception.what()}
        );
    }
}

} // namespace pdi::io
