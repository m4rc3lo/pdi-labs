/**
 * @file qt_controls.cpp
 * @brief Implements optional Qt controls exposed through HighGUI.
 */

#include "pdi/ui/qt_controls.hpp"

#include "pdi/ui/ui_capabilities.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <utility>

namespace pdi::ui {

struct QtControls::ControlBinding {
    std::function<void(bool)> callback;
};

QtControls::QtControls() = default;

QtControls::~QtControls() = default;

bool QtControls::is_available() {
    return UiCapabilities::detect().qt_controls;
}

bool QtControls::add_push_button(
    const std::string& name,
    std::function<void()> callback
) {
    if (!callback) {
        return false;
    }

    return add_control(
        name,
        cv::QT_PUSH_BUTTON,
        false,
        [callback = std::move(callback)](bool) {
            callback();
        }
    );
}

bool QtControls::add_checkbox(
    const std::string& name,
    bool initial_state,
    std::function<void(bool)> callback
) {
    return add_control(
        name,
        cv::QT_CHECKBOX,
        initial_state,
        std::move(callback)
    );
}

bool QtControls::add_radio_button(
    const std::string& name,
    bool initial_state,
    std::function<void(bool)> callback
) {
    return add_control(
        name,
        cv::QT_RADIOBOX,
        initial_state,
        std::move(callback)
    );
}

bool QtControls::add_control(
    const std::string& name,
    int control_type,
    bool initial_state,
    std::function<void(bool)> callback
) {
    if (!is_available() || name.empty() || !callback) {
        return false;
    }

    auto binding = std::make_unique<ControlBinding>();
    binding->callback = std::move(callback);
    ControlBinding* binding_pointer = binding.get();

    try {
        cv::createButton(
            name,
            &QtControls::dispatch_control,
            binding_pointer,
            control_type,
            initial_state
        );
    } catch (const cv::Exception&) {
        return false;
    }

    controls_.push_back(std::move(binding));
    return true;
}

void QtControls::dispatch_control(
    int state,
    void* user_data
) noexcept {
    auto* binding = static_cast<ControlBinding*>(user_data);

    if (binding == nullptr || !binding->callback) {
        return;
    }

    try {
        binding->callback(state != 0);
    } catch (...) {
    }
}

} // namespace pdi::ui
