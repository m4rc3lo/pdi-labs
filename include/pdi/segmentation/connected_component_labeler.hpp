/**
 * @file connected_component_labeler.hpp
 * @brief Declares manual connected-component labeling for binary images.
 */

#pragma once

#include <opencv2/core/mat.hpp>

namespace pdi::segmentation {

/**
 * @brief Selects the neighborhood used during connected-component traversal.
 */
enum class Connectivity {
    Four = 4,
    Eight = 8,
};

/**
 * @brief Stores one labeling result independently of CLI or GUI controls.
 */
struct ConnectedComponentLabelingResult {
    cv::Mat labels;
    int component_count = 0;
};

/**
 * @brief Labels foreground components with an explicit breadth-first search.
 *
 * @details The input must be a strict binary image of type `CV_8UC1`, where
 * zero represents background and 255 represents foreground. Label zero remains
 * reserved for background. Foreground components receive consecutive positive
 * labels in row-major discovery order.
 *
 * Each pixel is inspected a constant number of times, so the time complexity is
 * `O(rows * cols)` for both connectivities. The label matrix and BFS queue use
 * `O(rows * cols)` auxiliary memory in the worst case.
 *
 * This implementation does not call `cv::connectedComponents` or related
 * OpenCV labeling functions.
 */
class ConnectedComponentLabeler {
public:
    /**
     * @brief Labels all foreground components using the selected connectivity.
     *
     * @param binary_image Strict binary image with type `CV_8UC1`.
     * @param connectivity Four- or eight-neighbor connectivity.
     * @return Integer label matrix (`CV_32SC1`) and component count.
     *
     * @throws std::invalid_argument If the image is empty, has an unsupported
     * type or contains a value different from 0 and 255.
     */
    [[nodiscard]] ConnectedComponentLabelingResult label(
        const cv::Mat& binary_image,
        Connectivity connectivity
    ) const;

    /**
     * @brief Creates a binary mask for one positive component label.
     *
     * @param labels Label matrix produced by `label`, with type `CV_32SC1`.
     * @param selected_label Positive component label to isolate.
     * @return Binary `CV_8UC1` mask containing 255 for the selected component.
     *
     * @throws std::invalid_argument If the matrix is invalid or the selected
     * label is not positive.
     */
    [[nodiscard]] cv::Mat select_component(
        const cv::Mat& labels,
        int selected_label
    ) const;
};

} // namespace pdi::segmentation
