/**
 * @file main.cpp
 * @brief Manual diagnostic for closing a HighGUI window.
 */

#include "pdi/ui/interactive_window.hpp"

#include <opencv2/core.hpp>

#include <iostream>

namespace {

[[nodiscard]] const char* reason_name(
    pdi::windowing::WindowCloseReason reason
) {
    using pdi::windowing::WindowCloseReason;

    switch (reason) {
    case WindowCloseReason::Keyboard:
        return "keyboard";
    case WindowCloseReason::WindowClosed:
        return "window X";
    case WindowCloseReason::Requested:
        return "explicit request";
    case WindowCloseReason::BackendUnavailable:
        return "backend unavailable";
    case WindowCloseReason::None:
        return "none";
    }

    return "unknown";
}

} // namespace

int main() {
    std::cout
        << "Close the window with X, Esc, q or Q.\n"
        << "The process must terminate immediately after closing.\n";

    pdi::ui::InteractiveWindow window{"Window close diagnostic"};
    const cv::Mat image(240, 420, CV_8UC1, cv::Scalar{128});

    window.set_image(image);
    window.run();

    std::cout
        << "Event loop ended by: "
        << reason_name(window.close_reason())
        << '\n';

    pdi::ui::InteractiveWindow::destroy_all_windows();
    return 0;
}
