/**
 * @file interactive_window.cpp
 * @brief Implements a reusable HighGUI interactive window.
 */

#include "pdi/ui/interactive_window.hpp"

#include "pdi/ui/keyboard_event_dispatcher.hpp"

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
    window_created_ = true;
}

InteractiveWindow::~InteractiveWindow() noexcept {
    destroy_window();
}

void InteractiveWindow::set_image(const cv::Mat& image) {
    if (image.empty()) {
        throw std::invalid_argument(
            "Interactive window image must not be empty."
        );
    }

    current_image_ = image.clone();

    if (!window_created_ || !lifecycle_.should_continue()) {
        return;
    }

    if (event_loop_running_) {
        const auto current_visibility = visibility();
        if (current_visibility
            != pdi::windowing::WindowVisibility::Visible) {
            lifecycle_.observe_visibility(current_visibility);
            return;
        }
    }

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
    if (!lifecycle_.should_continue()) {
        return;
    }

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
    lifecycle_.request_close();
}

void InteractiveWindow::run(int delay_ms) {
    if (delay_ms <= 0) {
        throw std::invalid_argument(
            "Interactive event delay must be positive."
        );
    }

    event_loop_running_ = true;

    while (lifecycle_.should_continue()) {
        const auto current_visibility = visibility();
        lifecycle_.observe_visibility(current_visibility);

        if (!lifecycle_.should_continue()) {
            break;
        }

        KeyboardEventDispatcher::dispatch(
            cv::waitKey(delay_ms),
            lifecycle_,
            keyboard_callback_
        );
    }

    event_loop_running_ = false;
    destroy_window();
}

pdi::windowing::WindowCloseReason InteractiveWindow::close_reason() const {
    return lifecycle_.close_reason();
}

const std::string& InteractiveWindow::window_name() const {
    return window_name_;
}

void InteractiveWindow::destroy_all_windows() noexcept {
    try {
        cv::destroyAllWindows();
        static_cast<void>(cv::waitKey(1));
    } catch (const cv::Exception&) {
    }
}

pdi::windowing::WindowVisibility InteractiveWindow::visibility() const noexcept {
    if (!window_created_) {
        return pdi::windowing::WindowVisibility::Closed;
    }

    try {
        const double property = cv::getWindowProperty(
            window_name_,
            cv::WND_PROP_VISIBLE
        );
        return pdi::windowing::WindowLifecycleState::
            interpret_visibility(property);
    } catch (const cv::Exception&) {
        return pdi::windowing::WindowVisibility::Unavailable;
    }
}

void InteractiveWindow::destroy_window() noexcept {
    if (!window_created_) {
        return;
    }

    try {
        cv::destroyWindow(window_name_);
        static_cast<void>(cv::waitKey(1));
    } catch (const cv::Exception&) {
    }

    window_created_ = false;
    event_loop_running_ = false;
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
