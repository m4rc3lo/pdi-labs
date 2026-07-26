/**
 * @file ui_capabilities.cpp
 * @brief Implements HighGUI and Qt capability detection.
 */

#include "pdi/ui/ui_capabilities.hpp"

#include <opencv2/highgui.hpp>

#include <algorithm>
#include <cctype>
#include <string>

namespace pdi::ui {

UiCapabilities UiCapabilities::detect() {
    const std::string backend = cv::currentUIFramework();
    std::string normalized = backend;

    std::transform(
        normalized.begin(),
        normalized.end(),
        normalized.begin(),
        [](unsigned char character) {
            return static_cast<char>(std::toupper(character));
        }
    );

    const bool highgui_available = !backend.empty();
    const bool qt_available =
        normalized.find("QT") != std::string::npos;

    return {
        .windows = highgui_available,
        .trackbars = highgui_available,
        .mouse_callbacks = highgui_available,
        .keyboard_events = highgui_available,
        .qt_controls = qt_available,
        .backend_name = backend.empty() ? "NONE" : backend,
    };
}

} // namespace pdi::ui
