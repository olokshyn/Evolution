//
// Created by Oleg on 10/22/17.
//

#include "gtest/gtest.h"

#include "TestsCommon.h"
#include "OPTICS/optics.h"

using namespace std;

namespace {

    std::vector<size_t> optics_clustering_wrapped(
            const std::vector< std::vector<double> >& points,
            double eps, size_t min_pts) {
        return optics_clustering(points, points[0].size(), eps, min_pts);
    }

    const auto run_FCPS_test_optics =
            run_FCPS_test<decltype(optics_clustering_wrapped),
                    optics_clustering_wrapped>;
}

TEST(OpticsTest, FCPS_Atom) {
    auto points = read_points("data/FCPS/01FCPSdata/Atom.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Atom.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.75);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 790);  // min erroneously clustered points
}

TEST(OpticsTest, FCPS_Chainlink) {
    auto points = read_points("data/FCPS/01FCPSdata/Chainlink.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Chainlink.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.15);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 0);  // min erroneously clustered points
}

// Disabled due to long runtime
TEST(OpticsTest, DISABLED_FCPS_EngyTime) {
    auto points = read_points("data/FCPS/01FCPSdata/EngyTime.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/EngyTime.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.1);  // best eps
    EXPECT_EQ(get<1>(result), 3);  // best min_pts
    EXPECT_EQ(get<2>(result), 2430);  // min erroneously clustered points
}

TEST(OpticsTest, FCPS_GolfBall) {
    auto points = read_points("data/FCPS/01FCPSdata/GolfBall.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/GolfBall.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.1);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 0);  // min erroneously clustered points
}

TEST(OpticsTest, FCPS_Hepta) {
    auto points = read_points("data/FCPS/01FCPSdata/Hepta.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Hepta.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.75);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 0);  // min erroneously clustered points
}

TEST(OpticsTest, FCPS_Lsun) {
    auto points = read_points("data/FCPS/01FCPSdata/Lsun.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Lsun.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.45);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 0);  // min erroneously clustered points
}

TEST(OpticsTest, FCPS_Target) {
    auto points = read_points("data/FCPS/01FCPSdata/Target.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Target.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.25);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 0);  // min erroneously clustered points
}

TEST(OpticsTest, FCPS_Tetra) {
    auto points = read_points("data/FCPS/01FCPSdata/Tetra.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Tetra.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.5);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 0);  // min erroneously clustered points
}

TEST(OpticsTest, FCPS_TwoDiamonds) {
    auto points = read_points("data/FCPS/01FCPSdata/TwoDiamonds.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/TwoDiamonds.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.15);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 0);  // min erroneously clustered points
}

TEST(OpticsTest, FCPS_WingNut) {
    auto points = read_points("data/FCPS/01FCPSdata/WingNut.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/WingNut.cls");

    tuple<double, size_t, size_t> result = run_FCPS_test_optics(points,
                                                                cluster_labels);
    EXPECT_DOUBLE_EQ(get<0>(result), 0.25);  // best eps
    EXPECT_EQ(get<1>(result), 1);  // best min_pts
    EXPECT_EQ(get<2>(result), 0);  // min erroneously clustered points
}