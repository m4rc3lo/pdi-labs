/**
 * @file keyboard_event_dispatcher.hpp
 * @brief Declares GUI-independent keyboard event dispatch.
 */

#pragma once

#include "pdi/ui/keyboard_event.hpp"
#include "pdi/windowing/window_lifecycle.hpp"

#include <functional>

namespace pdi::ui {

/**
 * @brief Applies universal close policy and invokes an optional callback.
 */
class KeyboardEventDispatcher {
public:
    using Callback = std::function<void(const KeyboardEvent&)>;

    /**
     * @brief Dispatches one raw HighGUI key code without calling HighGUI.
     *
     * @param raw_code Code returned by an event source; negative means no key.
     * @param lifecycle Lifecycle state updated before the callback.
     * @param callback Optional application callback.
     */
    static void dispatch(
        int raw_code,
        pdi::windowing::WindowLifecycleState& lifecycle,
        const Callback& callback = {}
    );
};

} // namespace pdi::ui
