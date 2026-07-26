/**
 * @file image_display.hpp
 * @brief Declares optional display of named images.
 */

#pragma once

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>

namespace pdi::io {

/**
 * @brief Associates an unambiguous window title with an image.
 */
struct WindowImage {
    std::string title;
    cv::Mat image;
};

/**
 * @brief Displays a collection of images when explicitly requested by a CLI.
 *
 * @details The component opens one named window per item and processes
 * HighGUI events periodically. The event loop ends with Esc, q, Q or after
 * the last window is closed through its X button. Automated tests do not call
 * this component.
 */
class ImageDisplay {
public:
    /**
     * @brief Shows all named images until a universal close condition occurs.
     *
     * @param images Non-empty collection of titled, non-empty images.
     *
     * @throws std::invalid_argument If the collection, a title, or an image is
     * empty.
     * @throws std::runtime_error If the graphical backend reports an error
     * unrelated to normal window closure.
     */
    void show_all(const std::vector<WindowImage>& images) const;
};

} // namespace pdi::io
