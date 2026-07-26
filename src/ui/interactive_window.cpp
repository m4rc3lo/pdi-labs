/**
 * @file interactive_window.cpp
 * @brief Implements a reusable HighGUI interactive window.
 */

#include "pdi/ui/interactive_window.hpp"

#include <opencv2/highgui.hpp>

#include <stdexcept>
#include <utility>

namespace pdi::ui {

struct InteractiveWindow::TrackbarBinding {
    TrackbarCallback callback;
};

InteractiveWindow::InteractiveWindow(std::string window_name)
    : window_name_(std::move(window_name)) {
    if (window_name_.empty()) {
        throw std::invalid_argument(
            "Interactive window name must not be empty."
        );
    }

    cv::namedWindow(window_name_, cv::WINDOW_AUTOSIZE);
}

InteractiveWindow::~InteractiveWindow() {
    try {
        cv::destroyWindow(window_name_);
    } catch (...) {
    }
}

void InteractiveWindow::set_image(const cv::Mat& image) {
    if (image.empty()) {
        throw std::invalid_argument(
            "Interactive window image must not be empty."
        );
    }

    current_image_ = image.clone();
    cv::imshow(window_name_, current_image_);
}

void InteractiveWindow::add_trackbar(
    const std::string& name,
    int initial_value,
    int maximum_value,
    TrackbarCallback callback
) {
    if (name.empty()) {
        throw std::invalid_argument(
            "Trackbar name must not be empty."
        );
    }
    if (maximum_value < 0
        || initial_value < 0
        || initial_value > maximum_value) {
        throw std::invalid_argument(
            "Trackbar values are outside valid limits."
        );
    }
    if (!callback) {
        throw std::invalid_argument(
            "Trackbar callback must be defined."
        );
    }

    auto binding = std::make_unique<TrackbarBinding>();
    binding->callback = std::move(callback);
    TrackbarBinding* binding_pointer = binding.get();
    trackbars_.push_back(std::move(binding));

    cv::createTrackbar(
        name,
        window_name_,
        nullptr,
        maximum_value,
        &InteractiveWindow::dispatch_trackbar,
        binding_pointer
    );
    cv::setTrackbarPos(name, window_name_, initial_value);
}

void InteractiveWindow::set_trackbar_position(
    const std::string& name,
    int value
) {
    cv::setTrackbarPos(name, window_name_, value);
}

void InteractiveWindow::set_mouse_callback(MouseCallback callback) {
    mouse_callback_ = std::move(callback);
    cv::setMouseCallback(
        window_name_,
        &InteractiveWindow::dispatch_mouse,
        this
    );
}

void InteractiveWindow::set_keyboard_callback(
    KeyboardCallback callback
) {
    keyboard_callback_ = std::move(callback);
}

void InteractiveWindow::request_close() {
    close_requested_ = true;
}

void InteractiveWindow::run(int delay_ms) {
    if (delay_ms <= 0) {
        throw std::invalid_argument(
            "Interactive event delay must be positive."
        );
    }

    while (!close_requested_) {
        if (!current_image_.empty()) {
            cv::imshow(window_name_, current_image_);
        }

        const int raw_code = cv::waitKey(delay_ms);

        if (raw_code >= 0 && keyboard_callback_) {
            keyboard_callback_(
                KeyboardEvent::from_raw_code(raw_code)
            );
        }
    }
}

const std::string& InteractiveWindow::window_name() const {
    return window_name_;
}

void InteractiveWindow::dispatch_trackbar(
    int value,
    void* user_data
) noexcept {
    auto* binding = static_cast<TrackbarBinding*>(user_data);

    if (binding == nullptr || !binding->callback) {
        return;
    }

    try {
        binding->callback(value);
    } catch (...) {
    }
}

void InteractiveWindow::dispatch_mouse(
    int event,
    int x,
    int y,
    int flags,
    void* user_data
) noexcept {
    auto* window = static_cast<InteractiveWindow*>(user_data);

    if (window == nullptr || !window->mouse_callback_) {
        return;
    }

    try {
        window->mouse_callback_(
            MouseEvent::from_highgui(event, x, y, flags)
        );
    } catch (...) {
    }
}

} // namespace pdi::ui
