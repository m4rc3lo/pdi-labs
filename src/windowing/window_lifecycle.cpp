/**
 * @file window_lifecycle.cpp
 * @brief Implements GUI-independent window lifecycle decisions.
 */

#include "pdi/windowing/window_lifecycle.hpp"

namespace pdi::windowing {

bool WindowLifecycleState::is_exit_key_code(int raw_code) {
    if (raw_code < 0) {
        return false;
    }

    const int normalized_code = raw_code & 0xff;
    return normalized_code == 27
        || normalized_code == 'q'
        || normalized_code == 'Q';
}

WindowVisibility WindowLifecycleState::interpret_visibility(
    double property_value
) {
    if (property_value < 0.0) {
        return WindowVisibility::Unavailable;
    }

    if (property_value < 1.0) {
        return WindowVisibility::Closed;
    }

    return WindowVisibility::Visible;
}

void WindowLifecycleState::observe_key_code(int raw_code) {
    if (is_exit_key_code(raw_code)) {
        request_close(WindowCloseReason::Keyboard);
    }
}

void WindowLifecycleState::observe_visibility(
    WindowVisibility visibility
) {
    if (visibility == WindowVisibility::Closed) {
        request_close(WindowCloseReason::WindowClosed);
    } else if (visibility == WindowVisibility::Unavailable) {
        request_close(WindowCloseReason::BackendUnavailable);
    }
}

void WindowLifecycleState::request_close(WindowCloseReason reason) {
    if (close_requested_) {
        return;
    }

    close_requested_ = true;
    close_reason_ = reason;
}

bool WindowLifecycleState::should_continue() const {
    return !close_requested_;
}

WindowCloseReason WindowLifecycleState::close_reason() const {
    return close_reason_;
}

} // namespace pdi::windowing
