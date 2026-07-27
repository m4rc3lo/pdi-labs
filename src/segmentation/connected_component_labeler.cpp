/**
 * @file connected_component_labeler.cpp
 * @brief Implements manual connected-component labeling with BFS.
 */

#include "pdi/segmentation/connected_component_labeler.hpp"

#include <opencv2/core.hpp>

#include <array>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <utility>

namespace pdi::segmentation {
namespace {

using Offset = std::pair<int, int>;

constexpr std::array<Offset, 4> four_neighbors{{
    {-1, 0},
    {0, -1},
    {0, 1},
    {1, 0},
}};

constexpr std::array<Offset, 8> eight_neighbors{{
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, -1},
    {0, 1},
    {1, -1},
    {1, 0},
    {1, 1},
}};

void validate_binary_image(const cv::Mat& binary_image) {
    if (binary_image.empty()) {
        throw std::invalid_argument("Binary image must not be empty.");
    }

    if (binary_image.type() != CV_8UC1) {
        throw std::invalid_argument("Binary image must have type CV_8UC1.");
    }

    for (int row = 0; row < binary_image.rows; ++row) {
        const auto* binary_row = binary_image.ptr<std::uint8_t>(row);

        for (int column = 0; column < binary_image.cols; ++column) {
            if (binary_row[column] != 0U && binary_row[column] != 255U) {
                throw std::invalid_argument(
                    "Binary image must contain only 0 and 255."
                );
            }
        }
    }
}

template<std::size_t NeighborCount>
void label_component_bfs(
    const cv::Mat& binary_image,
    cv::Mat& labels,
    int start_row,
    int start_column,
    int component_label,
    const std::array<Offset, NeighborCount>& neighbors
) {
    std::queue<cv::Point> pending;
    pending.emplace(start_column, start_row);
    labels.ptr<std::int32_t>(start_row)[start_column] = component_label;

    while (!pending.empty()) {
        const cv::Point current = pending.front();
        pending.pop();

        for (const auto& [row_offset, column_offset] : neighbors) {
            const int neighbor_row = current.y + row_offset;
            const int neighbor_column = current.x + column_offset;

            if (neighbor_row < 0 || neighbor_row >= binary_image.rows
                || neighbor_column < 0
                || neighbor_column >= binary_image.cols) {
                continue;
            }

            const auto* binary_row =
                binary_image.ptr<std::uint8_t>(neighbor_row);
            auto* label_row = labels.ptr<std::int32_t>(neighbor_row);

            if (binary_row[neighbor_column] != 255U
                || label_row[neighbor_column] != 0) {
                continue;
            }

            label_row[neighbor_column] = component_label;
            pending.emplace(neighbor_column, neighbor_row);
        }
    }
}

} // namespace

ConnectedComponentLabelingResult ConnectedComponentLabeler::label(
    const cv::Mat& binary_image,
    Connectivity connectivity
) const {
    validate_binary_image(binary_image);

    if (connectivity != Connectivity::Four
        && connectivity != Connectivity::Eight) {
        throw std::invalid_argument(
            "Connectivity must be Four or Eight."
        );
    }

    cv::Mat labels = cv::Mat::zeros(binary_image.size(), CV_32SC1);
    int component_count = 0;

    for (int row = 0; row < binary_image.rows; ++row) {
        const auto* binary_row = binary_image.ptr<std::uint8_t>(row);
        auto* label_row = labels.ptr<std::int32_t>(row);

        for (int column = 0; column < binary_image.cols; ++column) {
            if (binary_row[column] != 255U || label_row[column] != 0) {
                continue;
            }

            ++component_count;

            switch (connectivity) {
            case Connectivity::Four:
                label_component_bfs(
                    binary_image,
                    labels,
                    row,
                    column,
                    component_count,
                    four_neighbors
                );
                break;

            case Connectivity::Eight:
                label_component_bfs(
                    binary_image,
                    labels,
                    row,
                    column,
                    component_count,
                    eight_neighbors
                );
                break;
            }
        }
    }

    return {
        .labels = std::move(labels),
        .component_count = component_count,
    };
}

cv::Mat ConnectedComponentLabeler::select_component(
    const cv::Mat& labels,
    int selected_label
) const {
    if (labels.empty()) {
        throw std::invalid_argument("Label matrix must not be empty.");
    }

    if (labels.type() != CV_32SC1) {
        throw std::invalid_argument("Label matrix must have type CV_32SC1.");
    }

    if (selected_label <= 0) {
        throw std::invalid_argument("Selected component label must be positive.");
    }

    cv::Mat mask = cv::Mat::zeros(labels.size(), CV_8UC1);

    for (int row = 0; row < labels.rows; ++row) {
        const auto* label_row = labels.ptr<std::int32_t>(row);
        auto* mask_row = mask.ptr<std::uint8_t>(row);

        for (int column = 0; column < labels.cols; ++column) {
            mask_row[column] = label_row[column] == selected_label
                ? static_cast<std::uint8_t>(255)
                : static_cast<std::uint8_t>(0);
        }
    }

    return mask;
}

} // namespace pdi::segmentation
