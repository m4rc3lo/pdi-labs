/**
 * @file component_features_csv_storage.hpp
 * @brief Declares CSV export for connected-component features.
 */

#pragma once

#include "pdi/segmentation/component_feature_extractor.hpp"

#include <filesystem>
#include <vector>

namespace pdi::io {

/**
 * @brief Writes one CSV row for each connected component.
 */
class ComponentFeaturesCsvStorage {
public:
    /**
     * @brief Saves component features with a stable column order.
     *
     * @param output_path Destination with `.csv` extension.
     * @param features Component records to export.
     */
    void save(
        const std::filesystem::path& output_path,
        const std::vector<pdi::segmentation::ComponentFeatures>& features
    ) const;
};

} // namespace pdi::io
