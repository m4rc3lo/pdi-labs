/**
 * @file keyboard_event_dispatcher.cpp
 * @brief Implements GUI-independent keyboard event dispatch.
 */

#include "pdi/ui/keyboard_event_dispatcher.hpp"

namespace pdi::ui {

void KeyboardEventDispatcher::dispatch(
    int raw_code,
    pdi::windowing::WindowLifecycleState& lifecycle,
    const Callback& callback
) {
    if (raw_code < 0) {
        return;
    }

    lifecycle.observe_key_code(raw_code);
    const KeyboardEvent event = KeyboardEvent::from_raw_code(raw_code);

    if (callback) {
        callback(event);
    }
}

} // namespace pdi::ui
