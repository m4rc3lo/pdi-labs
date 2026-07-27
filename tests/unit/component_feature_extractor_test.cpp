/**
 * @file component_feature_extractor_test.cpp
 * @brief Tests numeric connected-component feature extraction.
 */

#include "pdi/segmentation/component_feature_extractor.hpp"

#include "pdi/testing/mat_comparison.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <opencv2/core.hpp>

TEST_CASE(
    "ComponentFeatureExtractor computes area box and centroid",
    "[unit][segmentation][components]"
) {
    const cv::Mat labels = (
        cv::Mat_<int>(4, 5)
        << 1, 1, 0, 2, 0,
           1, 0, 0, 2, 2,
           0, 0, 3, 0, 0,
           0, 0, 3, 3, 0
    );

    const auto features =
        pdi::segmentation::ComponentFeatureExtractor{}.extract(labels);

    REQUIRE(features.size() == 3);

    REQUIRE(features[0].label == 1);
    REQUIRE(features[0].area == 3);
    REQUIRE(features[0].bounding_box == cv::Rect(0, 0, 2, 2));
    REQUIRE(features[0].centroid.x == Catch::Approx(1.0 / 3.0));
    REQUIRE(features[0].centroid.y == Catch::Approx(1.0 / 3.0));

    REQUIRE(features[1].label == 2);
    REQUIRE(features[1].area == 3);
    REQUIRE(features[1].bounding_box == cv::Rect(3, 0, 2, 2));
    REQUIRE(features[1].centroid.x == Catch::Approx(10.0 / 3.0));
    REQUIRE(features[1].centroid.y == Catch::Approx(2.0 / 3.0));

    REQUIRE(features[2].label == 3);
    REQUIRE(features[2].area == 3);
    REQUIRE(features[2].bounding_box == cv::Rect(2, 2, 2, 2));
    REQUIRE(features[2].centroid.x == Catch::Approx(7.0 / 3.0));
    REQUIRE(features[2].centroid.y == Catch::Approx(8.0 / 3.0));
}

TEST_CASE(
    "ComponentFeatureExtractor creates ProcessingRecord data",
    "[unit][segmentation][components][yaml]"
) {
    const cv::Mat labels = (
        cv::Mat_<int>(2, 3)
        << 1, 1, 0,
           0, 2, 2
    );

    const pdi::segmentation::ComponentFeatureExtractor extractor;
    const auto features = extractor.extract(labels);
    const auto record = extractor.make_processing_record(
        labels,
        features,
        {
            .project_version = "0.5.0",
            .input_path = "synthetic",
            .connectivity = "8",
        }
    );

    REQUIRE(record.parameters.size() == 7);
    REQUIRE(record.parameters[0].name == "connectivity");
    REQUIRE(record.parameters[0].value == "8");
    REQUIRE(record.parameters[1].name == "component_count");
    REQUIRE(record.parameters[1].value == "2");
    REQUIRE(
        record.parameters[2].name
        == "statistic.total_foreground_area"
    );
    REQUIRE(record.parameters[2].value == "4");
    REQUIRE(record.numeric_artifacts.size() == 1);
    REQUIRE(record.numeric_artifacts[0].name == "labels");

    pdi::testing::require_mat_exact(
        record.numeric_artifacts[0].value,
        labels
    );
}

TEST_CASE(
    "ComponentFeatureExtractor rejects invalid label matrices",
    "[unit][segmentation][components]"
) {
    REQUIRE_THROWS(
        pdi::segmentation::ComponentFeatureExtractor{}.extract(cv::Mat{})
    );

    const cv::Mat negative = (cv::Mat_<int>(1, 2) << 0, -1);

    REQUIRE_THROWS(
        pdi::segmentation::ComponentFeatureExtractor{}.extract(negative)
    );
}
