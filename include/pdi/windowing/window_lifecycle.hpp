/**
 * @file window_lifecycle.hpp
 * @brief Declares GUI-independent window lifecycle decisions.
 */

#pragma once

namespace pdi::windowing {

/**
 * @brief Identifies why an event loop stopped.
 */
enum class WindowCloseReason {
    None,
    Keyboard,
    WindowClosed,
    Requested,
    BackendUnavailable,
};

/**
 * @brief Identifies the interpreted state of one HighGUI window.
 */
enum class WindowVisibility {
    Visible,
    Closed,
    Unavailable,
};

/**
 * @brief Stores lifecycle decisions without calling HighGUI.
 */
class WindowLifecycleState {
public:
    /**
     * @brief Returns true for Esc, q or Q.
     */
    [[nodiscard]] static bool is_exit_key_code(int raw_code);

    /**
     * @brief Interprets a `WND_PROP_VISIBLE` result.
     */
    [[nodiscard]] static WindowVisibility interpret_visibility(
        double property_value
    );

    /**
     * @brief Observes one key code returned by an event loop.
     */
    void observe_key_code(int raw_code);

    /**
     * @brief Observes one interpreted window visibility state.
     */
    void observe_visibility(WindowVisibility visibility);

    /**
     * @brief Requests explicit event-loop termination.
     */
    void request_close(
        WindowCloseReason reason = WindowCloseReason::Requested
    );

    /**
     * @brief Returns whether the event loop should continue.
     */
    [[nodiscard]] bool should_continue() const;

    /**
     * @brief Returns the first reason that requested termination.
     */
    [[nodiscard]] WindowCloseReason close_reason() const;

private:
    bool close_requested_ = false;
    WindowCloseReason close_reason_ = WindowCloseReason::None;
};

} // namespace pdi::windowing
