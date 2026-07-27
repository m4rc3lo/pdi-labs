/**
 * @file component_features_csv_storage_test.cpp
 * @brief Tests CSV export for connected-component features.
 */

#include "pdi/io/component_features_csv_storage.hpp"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

TEST_CASE(
    "ComponentFeaturesCsvStorage writes stable header and rows",
    "[unit][io][csv][components]"
) {
    const auto directory =
        std::filesystem::temp_directory_path()
        / "pdi_labs_component_features_csv";
    const auto output_path = directory / "components.csv";
    std::filesystem::remove_all(directory);

    pdi::io::ComponentFeaturesCsvStorage{}.save(
        output_path,
        {
            {
                .label = 2,
                .area = 4,
                .bounding_box = {3, 5, 2, 3},
                .centroid = {3.5, 6.25},
            },
        }
    );

    std::string content;

    {
        std::ifstream input(output_path);
        content.assign(
            std::istreambuf_iterator<char>{input},
            std::istreambuf_iterator<char>{}
        );
    }

    REQUIRE(
        content
        == "label,area,x,y,width,height,centroid_x,centroid_y\n"
           "2,4,3,5,2,3,3.5,6.25\n"
    );

    std::filesystem::remove_all(directory);
}

TEST_CASE(
    "ComponentFeaturesCsvStorage requires CSV extension",
    "[unit][io][csv][components]"
) {
    REQUIRE_THROWS(
        pdi::io::ComponentFeaturesCsvStorage{}.save(
            "components.txt",
            {}
        )
    );
}
