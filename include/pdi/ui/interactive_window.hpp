/**
 * @file interactive_window.hpp
 * @brief Declares a reusable HighGUI interactive window.
 */

#pragma once

#include "pdi/ui/keyboard_event.hpp"
#include "pdi/ui/mouse_event.hpp"

#include <opencv2/core/mat.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace pdi::ui {

/**
 * @brief Encapsulates one responsive HighGUI window and its callbacks.
 *
 * @warning HighGUI callbacks execute on the thread that processes the event
 * loop. Callback objects and referenced state must outlive `run()`.
 */
class InteractiveWindow {
public:
    using TrackbarCallback = std::function<void(int)>;
    using MouseCallback = std::function<void(const MouseEvent&)>;
    using KeyboardCallback = std::function<void(const KeyboardEvent&)>;

    /**
     * @brief Creates a named HighGUI window.
     */
    explicit InteractiveWindow(std::string window_name);

    /**
     * @brief Destroys the owned HighGUI window.
     */
    ~InteractiveWindow();

    InteractiveWindow(const InteractiveWindow&) = delete;
    InteractiveWindow& operator=(const InteractiveWindow&) = delete;
    InteractiveWindow(InteractiveWindow&&) = delete;
    InteractiveWindow& operator=(InteractiveWindow&&) = delete;

    /**
     * @brief Replaces the image shown by the window.
     */
    void set_image(const cv::Mat& image);

    /**
     * @brief Adds a trackbar with an independent C++ callback.
     */
    void add_trackbar(
        const std::string& name,
        int initial_value,
        int maximum_value,
        TrackbarCallback callback
    );

    /**
     * @brief Programmatically changes one trackbar position.
     */
    void set_trackbar_position(
        const std::string& name,
        int value
    );

    /** @brief Registers the mouse callback. */
    void set_mouse_callback(MouseCallback callback);

    /** @brief Registers the keyboard callback. */
    void set_keyboard_callback(KeyboardCallback callback);

    /** @brief Requests termination of the event loop. */
    void request_close();

    /**
     * @brief Runs the responsive HighGUI event loop.
     *
     * @param delay_ms Delay passed to `cv::waitKey` on each iteration.
     */
    void run(int delay_ms = 20);

    /** @brief Returns the HighGUI window name. */
    [[nodiscard]] const std::string& window_name() const;

private:
    struct TrackbarBinding;

    static void dispatch_trackbar(int value, void* user_data) noexcept;
    static void dispatch_mouse(
        int event,
        int x,
        int y,
        int flags,
        void* user_data
    ) noexcept;

    std::string window_name_;
    cv::Mat current_image_;
    std::vector<std::unique_ptr<TrackbarBinding>> trackbars_;
    MouseCallback mouse_callback_;
    KeyboardCallback keyboard_callback_;
    bool close_requested_ = false;
};

} // namespace pdi::ui
