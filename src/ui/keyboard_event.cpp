/**
 * @file keyboard_event.cpp
 * @brief Implements typed keyboard event translation.
 */

#include "pdi/ui/keyboard_event.hpp"

namespace pdi::ui {

KeyboardEvent KeyboardEvent::from_raw_code(int raw_code) {
    const int normalized_code = raw_code < 0 ? -1 : raw_code & 0xff;

    KeyboardCommand command = KeyboardCommand::None;

    switch (normalized_code) {
    case 27:
    case 'q':
    case 'Q':
        command = KeyboardCommand::Exit;
        break;

    case 's':
    case 'S':
        command = KeyboardCommand::Save;
        break;

    case 'r':
    case 'R':
        command = KeyboardCommand::Reset;
        break;

    case 'm':
    case 'M':
        command = KeyboardCommand::ToggleMode;
        break;

    default:
        break;
    }

    return {
        .raw_code = raw_code,
        .normalized_code = normalized_code,
        .command = command,
    };
}

} // namespace pdi::ui
