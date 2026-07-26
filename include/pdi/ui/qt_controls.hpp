/**
 * @file qt_controls.hpp
 * @brief Declares optional Qt controls exposed through OpenCV HighGUI.
 */

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace pdi::ui {

/**
 * @brief Adds Qt-only HighGUI controls with runtime capability checks.
 */
class QtControls {
public:
    QtControls();
    ~QtControls();

    QtControls(const QtControls&) = delete;
    QtControls& operator=(const QtControls&) = delete;
    QtControls(QtControls&&) = delete;
    QtControls& operator=(QtControls&&) = delete;

    /** @brief Returns whether the active HighGUI backend reports Qt support. */
    [[nodiscard]] static bool is_available();

    /** @brief Adds a push button when Qt controls are available. */
    [[nodiscard]] bool add_push_button(
        const std::string& name,
        std::function<void()> callback
    );

    /** @brief Adds a checkbox when Qt controls are available. */
    [[nodiscard]] bool add_checkbox(
        const std::string& name,
        bool initial_state,
        std::function<void(bool)> callback
    );

    /** @brief Adds a radio button when Qt controls are available. */
    [[nodiscard]] bool add_radio_button(
        const std::string& name,
        bool initial_state,
        std::function<void(bool)> callback
    );

private:
    struct ControlBinding;

    [[nodiscard]] bool add_control(
        const std::string& name,
        int control_type,
        bool initial_state,
        std::function<void(bool)> callback
    );

    static void dispatch_control(int state, void* user_data) noexcept;

    std::vector<std::unique_ptr<ControlBinding>> controls_;
};

} // namespace pdi::ui
