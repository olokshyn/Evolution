//
// Created by Oleg on 9/18/16.
//

#ifndef EVOLUTION_TESTSCOMMON_H
#define EVOLUTION_TESTSCOMMON_H

extern "C" {
#include "Common.h"
#include "Entity/Entity.h"
#include "Species/Species.h"
}

#include <string>
#include <vector>
#include <tuple>

extern const Objective random_objective;

const double g_FCPPS_min_eps = 0.1;
const double g_FCPS_max_eps = 0.8;
const double g_FCPS_step_eps = 0.05;
const size_t g_FCPS_min_min_pts = 1;
const size_t g_FCPS_max_min_pts = 3;

Entity* MockCreateEntity(size_t chr_size,
                         const Objective* obj = &random_objective);

Species* MockCreateSpecies(size_t size, size_t chr_size,
                           const Objective* obj = &random_objective);

std::vector< std::vector<double> > read_points(const std::string& filename);

std::vector<size_t> read_cluster_labels(const std::string& filename);


size_t count_erroneously_clustered_points(
        const std::vector<size_t>& found_cluster_labels,
        const std::vector<size_t>& cluster_labels);

template <typename Func, Func&& clustering_algorithm>
std::tuple<double, size_t, size_t> run_FCPS_test(
        const std::vector<std::vector<double> >& points,
        const std::vector<size_t>& cluster_labels) {
    double best_eps = 0.0;
    size_t best_min_pts = 0;
    size_t min_erroneously_clustered_points = cluster_labels.size();
    for (double eps = g_FCPPS_min_eps;
         eps <= g_FCPS_max_eps;
         eps += g_FCPS_step_eps) {
        for (size_t min_pts = g_FCPS_min_min_pts;
             min_pts <= g_FCPS_max_min_pts;
             ++min_pts) {
            auto found_cluster_labels = clustering_algorithm(points, eps, min_pts);

            size_t erroneously_clustered_points =
                    count_erroneously_clustered_points(found_cluster_labels,
                                                       cluster_labels);
            if (erroneously_clustered_points < min_erroneously_clustered_points) {
                min_erroneously_clustered_points = erroneously_clustered_points;
                best_eps = eps;
                best_min_pts = min_pts;
            }
            if (min_erroneously_clustered_points == 0) {
                break;
            }
        }
        if (min_erroneously_clustered_points == 0) {
            break;
        }
    }
    return std::make_tuple(best_eps, best_min_pts,
                           min_erroneously_clustered_points);
};

#endif //EVOLUTION_TESTSCOMMON_H
