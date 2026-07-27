/**
 * @file component_features_csv_storage.cpp
 * @brief Implements CSV export for connected-component features.
 */

#include "pdi/io/component_features_csv_storage.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace pdi::io {

void ComponentFeaturesCsvStorage::save(
    const std::filesystem::path& output_path,
    const std::vector<pdi::segmentation::ComponentFeatures>& features
) const {
    if (output_path.extension() != ".csv") {
        throw std::invalid_argument(
            "Component feature export requires a .csv file."
        );
    }

    std::error_code error;
    const auto parent_path = output_path.parent_path();

    if (!parent_path.empty()) {
        std::filesystem::create_directories(parent_path, error);

        if (error) {
            throw std::runtime_error(
                "Could not create component CSV directory: "
                + parent_path.string() + ": " + error.message()
            );
        }
    }

    std::ofstream output(output_path, std::ios::binary);

    if (!output) {
        throw std::runtime_error(
            "Could not open component feature CSV: "
            + output_path.string()
        );
    }

    output
        << "label,area,x,y,width,height,centroid_x,centroid_y\n"
        << std::setprecision(17);

    for (const auto& feature : features) {
        output
            << feature.label << ','
            << feature.area << ','
            << feature.bounding_box.x << ','
            << feature.bounding_box.y << ','
            << feature.bounding_box.width << ','
            << feature.bounding_box.height << ','
            << feature.centroid.x << ','
            << feature.centroid.y << '\n';
    }

    if (!output) {
        throw std::runtime_error(
            "Could not finish component feature CSV: "
            + output_path.string()
        );
    }
}

} // namespace pdi::io
