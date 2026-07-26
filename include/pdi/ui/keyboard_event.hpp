/**
 * @file keyboard_event.hpp
 * @brief Declares typed keyboard events for optional interactive interfaces.
 */

#pragma once

namespace pdi::ui {

/**
 * @brief Identifies generic commands derived from keyboard input.
 */
enum class KeyboardCommand {
    None,
    Exit,
    Save,
    Reset,
    ToggleMode,
};

/**
 * @brief Represents one normalized HighGUI keyboard event.
 */
struct KeyboardEvent {
    int raw_code;
    int normalized_code;
    KeyboardCommand command;

    /**
     * @brief Converts a HighGUI key code into a typed event.
     *
     * @param raw_code Value returned by `cv::waitKey`.
     * @return Normalized event and generic command.
     */
    [[nodiscard]] static KeyboardEvent from_raw_code(int raw_code);
};

} // namespace pdi::ui
