/**
 * @file ui_capabilities.hpp
 * @brief Declares runtime HighGUI and Qt capability information.
 */

#pragma once

#include <string>

namespace pdi::ui {

/**
 * @brief Describes capabilities reported by the active HighGUI backend.
 */
struct UiCapabilities {
    bool windows;
    bool trackbars;
    bool mouse_callbacks;
    bool keyboard_events;
    bool qt_controls;
    std::string backend_name;

    /**
     * @brief Detects available capabilities without creating a window.
     *
     * @return Capability record for the active OpenCV HighGUI backend.
     */
    [[nodiscard]] static UiCapabilities detect();
};

} // namespace pdi::ui
