/**
 * @file interactive_state.cpp
 * @brief Implements GUI-independent interactive state.
 */

#include "pdi/ui/interactive_state.hpp"

#include <stdexcept>
#include <string>

namespace pdi::ui {

void InteractiveState::define_parameter(
    const std::string& name,
    int initial_value,
    int minimum_value,
    int maximum_value
) {
    if (name.empty()) {
        throw std::invalid_argument(
            "Interactive parameter name must not be empty."
        );
    }
    if (minimum_value > maximum_value) {
        throw std::invalid_argument(
            "Interactive parameter minimum must not exceed maximum."
        );
    }
    if (initial_value < minimum_value || initial_value > maximum_value) {
        throw std::invalid_argument(
            "Interactive parameter initial value is outside its limits."
        );
    }
    if (parameters_.contains(name)) {
        throw std::invalid_argument(
            "Interactive parameter is already defined: " + name
        );
    }

    parameters_.emplace(
        name,
        Parameter{
            .value = initial_value,
            .minimum = minimum_value,
            .maximum = maximum_value,
        }
    );
}

void InteractiveState::set_parameter(
    const std::string& name,
    int value
) {
    Parameter& target = mutable_parameter(name);

    if (value < target.minimum || value > target.maximum) {
        throw std::out_of_range(
            "Interactive parameter value is outside its limits: " + name
        );
    }

    target.value = value;
    request_reprocess();
}

void InteractiveState::set_ordered_pair(
    const std::string& lower_name,
    const std::string& upper_name,
    int lower_value,
    int upper_value
) {
    if (lower_value > upper_value) {
        throw std::invalid_argument(
            "Interactive ordered pair requires lower <= upper."
        );
    }

    Parameter& lower = mutable_parameter(lower_name);
    Parameter& upper = mutable_parameter(upper_name);

    if (lower_value < lower.minimum || lower_value > lower.maximum
        || upper_value < upper.minimum || upper_value > upper.maximum) {
        throw std::out_of_range(
            "Interactive ordered pair value is outside its limits."
        );
    }

    lower.value = lower_value;
    upper.value = upper_value;
    request_reprocess();
}

int InteractiveState::parameter(const std::string& name) const {
    return find_parameter(name).value;
}

void InteractiveState::request_reprocess() {
    reprocess_requested_ = true;
}

void InteractiveState::request_save() {
    save_requested_ = true;
}

void InteractiveState::request_close() {
    close_requested_ = true;
}

bool InteractiveState::consume_reprocess_request() {
    const bool requested = reprocess_requested_;
    reprocess_requested_ = false;
    return requested;
}

bool InteractiveState::consume_save_request() {
    const bool requested = save_requested_;
    save_requested_ = false;
    return requested;
}

bool InteractiveState::close_requested() const {
    return close_requested_;
}

InteractiveState::Parameter& InteractiveState::mutable_parameter(
    const std::string& name
) {
    const auto iterator = parameters_.find(name);

    if (iterator == parameters_.end()) {
        throw std::out_of_range(
            "Interactive parameter is not defined: " + name
        );
    }

    return iterator->second;
}

const InteractiveState::Parameter& InteractiveState::find_parameter(
    const std::string& name
) const {
    const auto iterator = parameters_.find(name);

    if (iterator == parameters_.end()) {
        throw std::out_of_range(
            "Interactive parameter is not defined: " + name
        );
    }

    return iterator->second;
}

} // namespace pdi::ui
