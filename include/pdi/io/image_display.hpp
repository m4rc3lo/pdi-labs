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
 * @details The component opens one named window per item, renders every image,
 * performs a single blocking `cv::waitKey`, and closes all windows afterward.
 * Automated tests do not call this component.
 */
class ImageDisplay {
public:
    /**
     * @brief Shows all named images and waits for one key press.
     *
     * @param images Non-empty collection of titled, non-empty images.
     *
     * @throws std::invalid_argument If the collection, a title, or an image is
     * empty.
     * @throws std::runtime_error If the graphical backend reports an error.
     */
    void show_all(const std::vector<WindowImage>& images) const;
};

} // namespace pdi::io
