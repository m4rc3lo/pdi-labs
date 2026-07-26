/**
 * @file interactive_state.hpp
 * @brief Declares GUI-independent state for interactive applications.
 */

#pragma once

#include <string>
#include <unordered_map>

namespace pdi::ui {

/**
 * @brief Stores bounded parameters and generic interaction requests.
 *
 * @details This class contains no HighGUI calls and can be tested without a
 * graphical environment.
 */
class InteractiveState {
public:
    /**
     * @brief Defines one bounded integer parameter.
     */
    void define_parameter(
        const std::string& name,
        int initial_value,
        int minimum_value,
        int maximum_value
    );

    /**
     * @brief Updates one previously defined parameter.
     */
    void set_parameter(const std::string& name, int value);

    /**
     * @brief Updates two parameters while enforcing lower <= upper.
     */
    void set_ordered_pair(
        const std::string& lower_name,
        const std::string& upper_name,
        int lower_value,
        int upper_value
    );

    /**
     * @brief Returns the current value of one parameter.
     */
    [[nodiscard]] int parameter(const std::string& name) const;

    /** @brief Marks the current state for reprocessing. */
    void request_reprocess();

    /** @brief Marks the current state for persistence. */
    void request_save();

    /** @brief Marks the current interaction for termination. */
    void request_close();

    /** @brief Returns and clears the reprocessing request. */
    [[nodiscard]] bool consume_reprocess_request();

    /** @brief Returns and clears the persistence request. */
    [[nodiscard]] bool consume_save_request();

    /** @brief Returns whether termination was requested. */
    [[nodiscard]] bool close_requested() const;

private:
    struct Parameter {
        int value;
        int minimum;
        int maximum;
    };

    [[nodiscard]] Parameter& mutable_parameter(const std::string& name);
    [[nodiscard]] const Parameter& find_parameter(
        const std::string& name
    ) const;

    std::unordered_map<std::string, Parameter> parameters_;
    bool reprocess_requested_ = false;
    bool save_requested_ = false;
    bool close_requested_ = false;
};

} // namespace pdi::ui
