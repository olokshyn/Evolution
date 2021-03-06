//
// Created by Oleg on 2/5/17.
//

/*! \brief Wishart tests covering all possible
 * clusterizations of two, three and four points.
 *
 * Conditions:
 * 0. One point is always in one class.
 * 1. Two points are always in one class.
 * 2. Three points will be:
 *    2.1 in one class, if (k = 1 and x2 ~ x1) or k = 2.
 *    2.2 in two classes, if k = 1 and x2 !~ x1
 *        2.2.1 [x1, x3], [x2], if x3 ~ x1.
 *        2.2.2 [x1], [x2, x3] if x3 ~ x2.
 *    2.3 never in three classes.
 * 3. Four points will be:
 *    3.1 in one class, if:
 *        3.1.1 k = 3.
 *        3.1.2 k = 2 and x2 ~ x1.
 *        3.1.3 k = 1 and x2 ~ x1 and (x3 ~ x1 or x3 ~ x2).
 *    3.2 in two classes [x1, x2, x4], [x3],
 *        if k = 1 and x2 ~ x1 and x3 !~ x1 and x3 !~ x2 and x4 !~ x3.
 *    3.3 in two classes [x1, x2], [x3, x4],
 *        if k = 1 and x2 ~ x1 and x3 !~ x1 and x3 !~ x2 and x4 ~ x3.
 *    3.4 in two classes [x1, x3, x4], [x2],
 *        if k = 1 and x2 !~ x1 and x3 ~ x1 and x4 !~ x2.
 *    3.5 in two classes [x1, x3], [x2, x4],
 *        if k = 1 and x2 !~ x1 and x3 ~ x1 and x4 ~ x2.
 *    3.6 in two classes [x1, x4], [x2, x3],
 *        if k = 1 and x2 !~ x1 and x3 ~ x2 and x4 ~ x1.
 *    3.7 in two classes [x1], [x2, x3, x4],
 *        if k = 1 and x2 !~ x1 and x3 ~ x2 and x4 !~ x1.
 *    3.8 never in four classes.
 *
 * The conditions above have been evaluated to get the distances
 * between points, under which the conditions are met.
 * All unmentioned distances should be strictly greater then
 * the mentioned ones.
 *
 * Condition in distances:
 * 0. One point.
 * 1. Two points, any distances.
 * 2. Three points:
 *    2.1 in one class: unspecified, refer to conditions
 *    2.2 in two classes:
 *        2.2.1 d(x1, x3) = d(x2, x3)
 *        2.2.2 d(x1, x3) = d(x2, x3)
 *    2.3 impossible
 * 3. Four points:
 *    3.1 in one class: unspecified, refer to conditions
 *    3.2 impossible
 *        (possible due to the order nearest neighbors are considered,
 *         but it contradicts the conditions for this case)
 *    3.3 k = 1
 *        1) d(x1, x2) <= d(x3, x4)
 *        2) d(x1, x4) = d(x3, x4) = d(x1, x2)
 *           (produces 3.2 due to the order nearest neighbors are considered)
 *    3.4 k = 1
 *        1) d(x1, x3) = d(x2, x3) <= d(x1, x4)
 *        2) d(x1, x4) = d(x2, x3) = d(x1, x3)
 *        3) d(x1, x3) = d(x2, x3) <= d(x3, x4)
 *        4) d(x1, x4) = d(x2, x3) = d(x1, x3) = d(x3, x4)
 *    3.5 k = 1
 *        1) d(x1, x3) = d(x2, x3) <= d(x2, x4)
 *        2) d(x1, x3) = d(x2, x4)
 *        3) d(x1, x4) = d(x2, x3) = d(x1, x3) = d(x2, x4)
 *           (produces 3.4 due to the order nearest neighbors are considered)
 *        4) d(x1, x4) = d(x2, x4) = d(x1, x3)
 *           (produces 3.4 due to the order nearest neighbors are considered)
 *    3.6 k = 1
 *        1) d(x1, x3) = d(x2, x3) <= d(x1, x4)
 *           (produces 3.4 due to the order nearest neighbors are considered)
 *        2) d(x1, x3) = d(x2, x4) = d(x2, x3) = d(x1, x4)
 *           (produces 3.4 due to the order nearest neighbors are considered)
 *        3) d(x1, x4) = d(x2, x3)
 *        4) d(x1, x4) = d(x2, x4) = d(x2, x3)
 *    3.7 k = 1
 *        1) d(x1, x3) = d(x2, x3) <= d(x2, x4)
 *           (produces 3.5 due to the order nearest neighbors are considered)
 *        2) d(x1, x3) = d(x2, x3) <= d(x3, x4)
 *           (produces 3.4 due to the order nearest neighbors are considered)
 *        3) d(x1, x3) = d(x2, x3) = d(x2, x4)
 *           (produces 3.5 due to the order nearest neighbors are considered)
 *        4) d(x1, x3) = d(x2, x3) = d(x2, x4) = d(x3, x4)
 *           (produces 3.5 due to the order nearest neighbors are considered)
 *    3.8 impossible
 */

#include <cmath>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <functional>
#include <memory>
#include <fstream>

#include "gtest/gtest.h"

#include "TestsCommon.h"

extern "C" {
#include "Wishart/Wishart.h"
}

namespace {

std::vector<const double*> to_point_pointers(
        const std::vector< std::vector<double> >& points) {
    std::vector<const double*> point_pointers(points.size());
    std::transform(points.begin(), points.end(), point_pointers.begin(),
                   [](const std::vector<double>& point) -> const double* {
                       return point.data();
                   });
    return point_pointers;
}

std::vector<size_t> to_cluster_numbers(
        std::unique_ptr<size_t> raw_cluster_numbers,
        size_t vectors_count) {
    return std::vector<size_t>(raw_cluster_numbers.get(),
                               raw_cluster_numbers.get() + vectors_count);
}

std::vector<size_t> run_wishart(const std::vector<std::vector<double> >& points,
                                double h, size_t k) {
    std::vector<const double*> point_pointers = to_point_pointers(points);

    return to_cluster_numbers(
            std::unique_ptr<size_t>(
                    Wishart(point_pointers.data(),
                            points.size(),
                            points[0].size(),
                            h, k)),
            points.size());
}

const auto run_FCPS_test_wishart =
        run_FCPS_test<decltype(run_wishart), run_wishart>;
}

TEST(WishartTest, one_positive) {
    std::vector< std::vector<double> > points = {
            { 1.0, 1.0 }
    };

    for (double h = 0; h <= 1.0; h += 0.1) {
        auto cluster_numbers = run_wishart(points, h, 1);

        ASSERT_EQ(1u, cluster_numbers.size());
        EXPECT_EQ(1u, cluster_numbers.at(0));
    }
}

TEST(WishartTest, two_positive) {
    std::vector< std::vector<double> > points = {
            { 0.0, 0.0 },
            { 0.5, 0.5 }
    };

    for (size_t k = 1; k != 3; ++k) {
        for (double h = 0; h <= 1.0; h += 0.1) {
            auto cluster_numbers = run_wishart(points, h, k);

            ASSERT_EQ(2u, cluster_numbers.size());
            EXPECT_EQ(1u, cluster_numbers[0]);
            EXPECT_EQ(1u, cluster_numbers[1]);
        }
    }
}

TEST(WishartTest, three_2_1_positive) {
    std::vector< std::vector<double> > points1 = {
            { 0.1, 0.1 },
            { 0.2, 0.2 },
            { 0.7, 0.7 }
    };

    for (double h = 0; h <= 1.0; h += 0.1) {
        auto cluster_numbers = run_wishart(points1, h, 1);

        ASSERT_EQ(3u, cluster_numbers.size());
        EXPECT_EQ(1u, std::accumulate(cluster_numbers.begin(),
                                      cluster_numbers.end(),
                                      1u, std::multiplies<size_t>()));
    }

    std::vector< std::vector<double> > points2 = {
            { 0.1, 0.1 },
            { 0.8, 0.8 },
            { 0.7, 0.7 }
    };

    for (double h = 0; h <= 1.0; h += 0.1) {
        auto cluster_numbers = run_wishart(points2, h, 2);

        ASSERT_EQ(3u, cluster_numbers.size());
        EXPECT_EQ(1u, std::accumulate(cluster_numbers.begin(),
                                      cluster_numbers.end(),
                                      1u, std::multiplies<size_t>()));
    }
}

TEST(WishartTest, three_2_2_positive) {
    double delta = 0.3;
    for (double x3 = 0.5, y3 = 0.5; x3 <= 1.0; x3 += 0.1, y3 += 0.1) {
        std::vector< std::vector<double> > points = { // d(x1, x3) = d(x2, x3)
                { x3 - delta, y3 - delta },
                { x3 + delta, y3 + delta },
                { x3,         y3 }
        };

        for (double h = 0; h <= 1.0; h += 0.1) {
            auto cluster_numbers = run_wishart(points, h, 1);
            ASSERT_EQ(3u, cluster_numbers.size());
            EXPECT_EQ(1u, cluster_numbers[0]);
            EXPECT_EQ(2u, cluster_numbers[1]);
            EXPECT_TRUE(cluster_numbers[2] == 1 || cluster_numbers[2] == 2);
        }
    }
}

TEST(WishartTest, four_3_2_positive) {
    // This test case contradicts the conditions for 3.2
    // but is legal due to the order nearest neighbors are considered

    // 3.3 2) d(x1, x4) = d(x3, x4) = d(x1, x2)
    const std::vector< std::vector<double> > points = {
            { 0.4, 0.4 },
            { 0.2, 0.2 },
            { 0.8, 0.8 },
            { 0.6, 0.6 }
    };

    auto cluster_numbers = run_wishart(points, 0.7, 1);
    ASSERT_EQ(4u, cluster_numbers.size());
    EXPECT_EQ(1u, cluster_numbers[0]);
    EXPECT_EQ(1u, cluster_numbers[1]);
    EXPECT_EQ(2u, cluster_numbers[2]);
    EXPECT_EQ(1u, cluster_numbers[3]);
}

TEST(WishartTest, four_3_3_positive) {
    for (const std::vector< std::vector<double> >& points : std::vector< std::vector< std::vector<double> > >(
            {
                    { // 1) d(x1, x2) <= d(x3, x4)
                            { 0.0, 0.0 },
                            { 0.2, 0.2 },
                            { 0.7, 0.7 },
                            { 1.0, 1.0 }
                    },

                    { // 1) d(x1, x2) <= d(x3, x4)
                            { 0.0, 0.0 },
                            { 0.2, 0.2 },
                            { 0.8, 0.8 },
                            { 1.0, 1.0 }
                    },

                    // The algorithm won`t produce expected results in this case
                    // due to the order nearest neighbors are considered:
                    // x1 ~ x2, x2 ~ x1, x3 ~ x4, x4 ~ x1
                    //                    { // 2) d(x1, x4) = d(x3, x4) = d(x1, x2)
                    //                            { 0.4, 0.4 },
                    //                            { 0.2, 0.2 },
                    //                            { 0.8, 0.8 },
                    //                            { 0.6, 0.6 }
                    //                    }
            })) {
        auto cluster_numbers = run_wishart(points, 0.2, 1);
        ASSERT_EQ(4u, cluster_numbers.size());
        EXPECT_EQ(1u, cluster_numbers[0]);
        EXPECT_EQ(1u, cluster_numbers[1]);
        EXPECT_EQ(2u, cluster_numbers[2]);
        EXPECT_EQ(2u, cluster_numbers[3]);
    }
}

TEST(WishartTest, four_3_4_positive) {
    for (const std::vector< std::vector<double> >& points : std::vector< std::vector< std::vector<double> > >(
            {
                    { // 1) d(x1, x3) = d(x2, x3) <= d(x1, x4)
                            { 0.2,  0.2 },
                            { 0.6,  0.6 },
                            { 0.4,  0.4 },
                            { -0.2, -0.2 }
                    },

                    { // 2) d(x1, x4) = d(x2, x3) = d(x1, x3)
                            { 0.2, 0.2 },
                            { 0.6, 0.6 },
                            { 0.4, 0.4 },
                            { 0.0, 0.0 }
                    },

                    { // 3) d(x1, x3) = d(x2, x3) <= d(x3, x4)
                            { 0.2, 0.2 },
                            { 0.6, 0.6 },
                            { 0.4, 0.4 },
                            { 0.0, 0.8 }
                    },

                    { // 4) d(x1, x4) = d(x2, x3) = d(x1, x3) = d(x3, x4)
                            { 0.2, 0.2 },
                            { 0.6, 0.6 },
                            { 0.4, 0.4 },
                            { 0.1 * sqrt(3) + 0.3, 0.3 - 0.1 * sqrt(3) }
                    },

                    { // 4) d(x1, x4) = d(x2, x3) = d(x1, x3) = d(x3, x4)
                            { 0.2, 0.2 },
                            { 0.6, 0.6 },
                            { 0.4, 0.4 },
                            { -0.1 * sqrt(3) + 0.3, 0.3 + 0.1 * sqrt(3) }
                    },

                    { // 3.5 3) d(x1, x4) = d(x2, x3) = d(x1, x3) = d(x2, x4)
                            { 0.0, 0.0 },
                            { 0.4, 0.4 },
                            { 0.0, 0.4 },
                            { 0.4, 0.0 }
                    },

                    { // 3.5 4) d(x1, x4) = d(x2, x4) = d(x1, x3)
                            { 0.0, 0.0 },
                            { 0.8, 0.0 },
                            { 0.0, 0.4 },
                            { 0.4, 0.0 }
                    },

                    { // 3.6 1) d(x1, x3) = d(x2, x3) <= d(x1, x4)
                            { 0.1, 0.1 },
                            { 0.4, 0.4 },
                            { 0.4, 0.1 },
                            { 0.1, -0.3 }
                    },

                    { // 3.6 2) d(x1, x3) = d(x2, x4) = d(x2, x3) = d(x1, x4)
                            { 0.0, 0.0 },
                            { 0.4, 0.4 },
                            { 0.0, 0.4 },
                            { 0.4, 0.0 }
                    },

                    { // 3.7 2) d(x1, x3) = d(x2, x3) <= d(x3, x4)
                            { 0.1, 0.1 },
                            { 0.4, 0.4 },
                            { 0.4, 0.1 },
                            { 0.9, 0.1 }
                    }
            })) {
        auto cluster_numbers = run_wishart(points, 0.2, 1);
        ASSERT_EQ(4u, cluster_numbers.size());
        EXPECT_EQ(1u, cluster_numbers[0]);
        EXPECT_EQ(2u, cluster_numbers[1]);
        EXPECT_EQ(1u, cluster_numbers[2]);
        EXPECT_EQ(1u, cluster_numbers[3]);
    }
}

TEST(WishartTest, four_3_5_positive) {
    for (const std::vector< std::vector<double> >& points : std::vector< std::vector< std::vector<double> > >(
            {
                    { // 1) d(x1, x3) = d(x2, x3) <= d(x2, x4)
                            { 0.2, 0.2 },
                            { 0.6, 0.6 },
                            { 0.4, 0.4 },
                            { 0.9, 0.9 }
                    },

                    { // 2) d(x1, x3) = d(x2, x4)
                            { 0.2, 0.2 },
                            { 0.7, 0.7 },
                            { 0.3, 0.3 },
                            { 0.8, 0.8 }
                    },

                    //                    The algorithm won`t produce expected results in thess cases
                    //                    due to the order nearest neighbors are considered
                    //
                    //                    { // 3) d(x1, x4) = d(x2, x3) = d(x1, x3) = d(x2, x4)
                    //                            { 0.0, 0.0 },
                    //                            { 0.4, 0.4 },
                    //                            { 0.0, 0.4 },
                    //                            { 0.4, 0.0 }
                    //                    },
                    //
                    //                    { // 4) d(x1, x4) = d(x2, x4) = d(x1, x3)
                    //                            { 0.0, 0.0 },
                    //                            { 0.8, 0.0 },
                    //                            { 0.0, 0.4 },
                    //                            { 0.4, 0.0 }
                    //                    }

                    { // 3.7 1) d(x1, x3) = d(x2, x3) <= d(x2, x4)
                            { 0.1, 0.1 },
                            { 0.4, 0.4 },
                            { 0.4, 0.1 },
                            { 0.9, 0.4 }
                    },

                    { // 3.7 3) d(x1, x3) = d(x2, x3) = d(x2, x4)
                            { 0.1, 0.1 },
                            { 0.4, 0.4 },
                            { 0.4, 0.1 },
                            { 0.7, 0.4 }
                    },

                    { // 3.7 4) d(x1, x3) = d(x2, x3) = d(x2, x4) = d(x3, x4)
                            { 0.0, 0.0 },
                            { 0.3 + 0.3 * sin(M_PI / 3), 0.3 / 2 },
                            { 0.3, 0.0 },
                            { 0.3 + 0.3 * sin(M_PI / 3), -0.3 / 2 }
                    }
            })) {
        auto cluster_numbers = run_wishart(points, 0.2, 1);
        ASSERT_EQ(4u, cluster_numbers.size());
        EXPECT_EQ(1u, cluster_numbers[0]);
        EXPECT_EQ(2u, cluster_numbers[1]);
        EXPECT_EQ(1u, cluster_numbers[2]);
        EXPECT_EQ(2u, cluster_numbers[3]);
    }
}

TEST(WishartTest, four_3_6_positive) {
    for (const std::vector< std::vector<double> >& points : std::vector< std::vector< std::vector<double> > >(
            {
                    //                    The algorithm won`t produce expected results in thess cases
                    //                    due to the order nearest neighbors are considered
                    //                    { // 1) d(x1, x3) = d(x2, x3) <= d(x1, x4)
                    //                            { 0.1, 0.1 },
                    //                            { 0.4, 0.4 },
                    //                            { 0.4, 0.1 },
                    //                            { 0.1, -0.3 }
                    //                    },
                    //
                    //                    { // 2) d(x1, x3) = d(x2, x4) = d(x2, x3) = d(x1, x4)
                    //                            { 0.0, 0.0 },
                    //                            { 0.4, 0.4 },
                    //                            { 0.0, 0.4 },
                    //                            { 0.4, 0.0 }
                    //                    },

                    { // 3) d(x1, x4) = d(x2, x3)
                            { 0.0, 0.0 },
                            { 0.5, 0.5 },
                            { 0.6, 0.6 },
                            { 0.1, 0.1 }
                    },

                    { // 4) d(x1, x4) = d(x2, x4) = d(x2, x3)
                            { 0.0, 0.0 },
                            { 0.4, 0.4 },
                            { 0.8, 0.4 },
                            { 0.4, 0.0 }
                    }
            })) {
        auto cluster_numbers = run_wishart(points, 0.2, 1);
        ASSERT_EQ(4u, cluster_numbers.size());
        EXPECT_EQ(1u, cluster_numbers[0]);
        EXPECT_EQ(2u, cluster_numbers[1]);
        EXPECT_EQ(2u, cluster_numbers[2]);
        EXPECT_EQ(1u, cluster_numbers[3]);
    }
}

TEST(WishartTest, four_3_7_positive) {
    for (const std::vector< std::vector<double> >& points : std::vector< std::vector< std::vector<double> > >(
            {
                    //                    The algorithm won`t produce expected results in thess cases
                    //                    due to the order nearest neighbors are considered
                    //                    { // 1) d(x1, x3) = d(x2, x3) <= d(x2, x4)
                    //                            { 0.1, 0.1 },
                    //                            { 0.4, 0.4 },
                    //                            { 0.4, 0.1 },
                    //                            { 0.9, 0.4 }
                    //                    },
                    //
                    //                    { // 2) d(x1, x3) = d(x2, x3) <= d(x3, x4)
                    //                            { 0.1, 0.1 },
                    //                            { 0.4, 0.4 },
                    //                            { 0.4, 0.1 },
                    //                            { 0.9, 0.1 }
                    //                    },
                    //
                    //                    { // 3) d(x1, x3) = d(x2, x3) = d(x2, x4)
                    //                            { 0.1, 0.1 },
                    //                            { 0.4, 0.4 },
                    //                            { 0.4, 0.1 },
                    //                            { 0.7, 0.4 }
                    //                    },
                    //
                    //                    { // 4) d(x1, x3) = d(x2, x3) = d(x2, x4) = d(x3, x4)
                    //                            { 0.0, 0.0 },
                    //                            { 0.3 + 0.3 * sin(M_PI / 3), 0.3 / 2 },
                    //                            { 0.3, 0.0 },
                    //                            { 0.3 + 0.3 * sin(M_PI / 3), -0.3 / 2 }
                    //                    }
            })) {
        auto cluster_numbers = run_wishart(points, 0.2, 1);
        ASSERT_EQ(4u, cluster_numbers.size());
        EXPECT_EQ(1u, cluster_numbers[0]);
        EXPECT_EQ(2u, cluster_numbers[1]);
        EXPECT_EQ(2u, cluster_numbers[2]);
        EXPECT_EQ(2u, cluster_numbers[3]);
    }
}

//TEST(WishartTest, 3_3_2_2) {
//    for (const std::vector< std::vector<double> >& points : std::vector< std::vector< std::vector<double> > >(
//            {
//                    {
//                            { 0.0, 0.0 },
//                            { 0.6, 0.6 },
//                            { 0.2, 0.2 },
//                            { 0.1, 0.1 }
//                    },
//
//                    {
//                            { 0.0, 0.0 },
//                            { 0.4, 0.4 },
//                            { 0.8, 0.4 },
//                            { 0.4, 0.0 }
//                    }
//            })) {
//        auto cluster_numbers = run_wishart(points, 2, 0.2);
//        ASSERT_EQ(4, cluster_numbers.size());
//        EXPECT_EQ(1, cluster_numbers[0]);
//        EXPECT_EQ(2, cluster_numbers[1]);
//        EXPECT_EQ(2, cluster_numbers[2]);
//        EXPECT_EQ(1, cluster_numbers[3]);
//    }
//}

TEST(WishartTest, FisherIris) {
    const size_t iris_dimensions = 4;
    const size_t iris_count = 150;
    const size_t iris_cluster_size = 50;
    const char* iris_filename = "data/fisher_iris.txt";

    std::vector< std::vector<double> > iris(iris_count);

    std::ifstream iris_file(iris_filename);
    if (!iris_file.is_open()) {
        throw std::runtime_error("Failed to open iris file");
    }

    for (size_t i = 0; i != iris_count; ++i) {
        iris[i] = std::vector<double>(iris_dimensions);
        for (size_t j = 0; j != iris_dimensions; ++j) {
            iris_file >> iris[i][j];
        }
        int cluster_number;
        iris_file >> cluster_number;
    }
    iris_file.close();

    auto cluster_numbers = run_wishart(iris, 0.14, 3);

    size_t correctly_clusterd_count = 0;
    ASSERT_EQ(iris_count, cluster_numbers.size());
    for (size_t i = 0; i != iris_count / iris_cluster_size; ++i) {
        size_t cluster_number = cluster_numbers[i * iris_cluster_size];
        for (size_t j = i * iris_cluster_size;
             j != (i + 1) * iris_cluster_size; ++j) {
            if (cluster_number == cluster_numbers[j]
                && (i == 0 || cluster_number
                              != cluster_numbers[(i - 1) * iris_cluster_size])) {
                ++correctly_clusterd_count;
            }
        }
    }
    EXPECT_GE(correctly_clusterd_count / (double)iris_count, 0.66);
}

TEST(WishartTest, FCPS_Atom) {
    auto points = read_points("data/FCPS/01FCPSdata/Atom.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Atom.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.15);  // best eps
    EXPECT_EQ(std::get<1>(result), 3u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 12u);  // min erroneously clustered points
}

TEST(WishartTest, FCPS_Chainlink) {
    auto points = read_points("data/FCPS/01FCPSdata/Chainlink.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Chainlink.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.1);  // best eps
    EXPECT_EQ(std::get<1>(result), 1u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 974u);  // min erroneously clustered points
}

// Disabled due to long runtime
TEST(WishartTest, DISABLED_FCPS_EngyTime) {
    auto points = read_points("data/FCPS/01FCPSdata/EngyTime.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/EngyTime.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.1);  // best eps
    EXPECT_EQ(std::get<1>(result), 3u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 2430u);  // min erroneously clustered points
}

TEST(WishartTest, FCPS_GolfBall) {
    auto points = read_points("data/FCPS/01FCPSdata/GolfBall.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/GolfBall.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.2);  // best eps
    EXPECT_EQ(std::get<1>(result), 3u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 0u);  // min erroneously clustered points
}

TEST(WishartTest, FCPS_Hepta) {
    auto points = read_points("data/FCPS/01FCPSdata/Hepta.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Hepta.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.1);  // best eps
    EXPECT_EQ(std::get<1>(result), 1u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 171u);  // min erroneously clustered points
}

TEST(WishartTest, FCPS_Lsun) {
    auto points = read_points("data/FCPS/01FCPSdata/Lsun.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Lsun.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.1);  // best eps
    EXPECT_EQ(std::get<1>(result), 3u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 34u);  // min erroneously clustered points
}

TEST(WishartTest, FCPS_Target) {
    auto points = read_points("data/FCPS/01FCPSdata/Target.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Target.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.1);  // best eps
    EXPECT_EQ(std::get<1>(result), 3u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 211u);  // min erroneously clustered points
}

TEST(WishartTest, FCPS_Tetra) {
    auto points = read_points("data/FCPS/01FCPSdata/Tetra.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/Tetra.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.1);  // best eps
    EXPECT_EQ(std::get<1>(result), 1u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 372u);  // min erroneously clustered points
}

TEST(WishartTest, FCPS_TwoDiamonds) {
    auto points = read_points("data/FCPS/01FCPSdata/TwoDiamonds.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/TwoDiamonds.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.1);  // best eps
    EXPECT_EQ(std::get<1>(result), 3u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 0u);  // min erroneously clustered points
}

TEST(WishartTest, FCPS_WingNut) {
    auto points = read_points("data/FCPS/01FCPSdata/WingNut.lrn");
    auto cluster_labels = read_cluster_labels("data/FCPS/01FCPSdata/WingNut.cls");

    std::tuple<double, size_t, size_t> result = run_FCPS_test_wishart(points,
                                                                      cluster_labels);
    EXPECT_DOUBLE_EQ(std::get<0>(result), 0.1);  // best eps
    EXPECT_EQ(std::get<1>(result), 3u);  // best min_pts
    EXPECT_EQ(std::get<2>(result), 10u);  // min erroneously clustered points
}
