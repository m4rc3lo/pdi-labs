/**
 * @file mouse_event.hpp
 * @brief Declares typed mouse events for optional interactive interfaces.
 */

#pragma once

namespace pdi::ui {

/**
 * @brief Identifies a normalized mouse action.
 */
enum class MouseAction {
    Unknown,
    Move,
    LeftButtonDown,
    LeftButtonUp,
    RightButtonDown,
    RightButtonUp,
    MiddleButtonDown,
    MiddleButtonUp,
    MouseWheel,
    HorizontalWheel,
};

/**
 * @brief Represents one HighGUI mouse event.
 */
struct MouseEvent {
    MouseAction action;
    int x;
    int y;
    int flags;
    int raw_event;

    /**
     * @brief Converts raw HighGUI mouse data into a typed event.
     *
     * @param raw_event OpenCV mouse event code.
     * @param x Horizontal image coordinate.
     * @param y Vertical image coordinate.
     * @param flags OpenCV button and modifier flags.
     * @return Typed mouse event.
     */
    [[nodiscard]] static MouseEvent from_highgui(
        int raw_event,
        int x,
        int y,
        int flags
    );
};

} // namespace pdi::ui
