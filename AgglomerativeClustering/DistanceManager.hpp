//
// Created by Oleg on 4/21/16.
//

#ifndef WISHART_DISTANCEMANAGER_HPP
#define WISHART_DISTANCEMANAGER_HPP

#include <memory>
#include <algorithm>

#include "Cluster.hpp"

template <typename T>
class DistanceManager {

public:
    static bool Comparator(size_t i, size_t j) {
        return i > j;
    }

    DistanceManager(const std::vector<Cluster<T>>& clusters,
                    typename Cluster<T>::measure_type measure)
            : size(clusters.size()),
              distances(size, std::vector<double>(size, 0.0)),
              new_distances(size, std::vector<double>(size, 0.0)),
              sizes(size, 0),
              new_sizes(size, 0) {
        for (size_t i = 0; i < size; ++i) {
            for (size_t j = i + 1; j < size; ++j) {
                distances[i][j] =
                    distances[j][i] =
                        new_distances[i][j] =
                            new_distances[j][i] =
                                clusters[i].GetNormSum(clusters[j], measure);
            }
            sizes[i] = new_sizes[i] = clusters[i].GetSize();
        }
    }

    double GetDistance(size_t i, size_t j) {
        return distances[i][j] / (double)sizes[i] / (double)sizes[j];
    }

    void MergeClusters(size_t i, size_t j) {
        if (i >= size || j >= size) {
            throw IndexOutOfRangeException();
        }

        if (i > j) {
            std::swap(i, j);
        }

        new_sizes[i] += new_sizes[j];

        for (size_t k = 0; k < size; ++k) {
            if (k != i) {
                new_distances[k][i] += new_distances[k][j];
                new_distances[i][k] += new_distances[j][k];
            }
            else {
                new_distances[k][i] = 0.0;
            }
        }

        indexes_to_delete.push_back(j);
    }

    void CommitMerges() {
        std::sort(indexes_to_delete.begin(),
                  indexes_to_delete.end(),
                  Comparator);
        for (size_t index : indexes_to_delete) {
            DeleteIndex(index);
        }
        indexes_to_delete.clear();
        sizes = new_sizes;
        distances = new_distances;
    }

private:
    size_t size;
    std::vector<std::vector<double>> distances;
    std::vector<std::vector<double>> new_distances;
    std::vector<size_t> sizes;
    std::vector<size_t> new_sizes;
    std::vector<size_t> indexes_to_delete;

    void DeleteIndex(size_t index) {
        for (size_t i = index + 1; i < size; ++i) {
            new_sizes[i - 1] = new_sizes[i];
        }
        new_sizes.pop_back();

        for (size_t i = 0; i < size; ++i) {
            for (size_t j = index + 1; j < size; ++j) {
                new_distances[i][j - 1] = new_distances[i][j];
            }
            new_distances[i].pop_back();
        }

        for (size_t j = 0; j < size - 1; ++j) {
            for (size_t i  = index + 1; i < size; ++i) {
                new_distances[i - 1][j] = new_distances[i][j];
            }
        }
        new_distances.pop_back();

        --size;
    }
};

#endif //WISHART_DISTANCEMANAGER_HPP
