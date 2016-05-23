//
// Created by Oleg on 5/14/16.
//

#include <algorithm>

#include "DistanceManager.hpp"


DistanceManager::DistanceManager(const std::vector<Cluster>& clusters)
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
                        clusters[i].GetNormSum(clusters[j]);
            }
            sizes[i] = new_sizes[i] = clusters[i].GetSize();
        }
}

double DistanceManager::GetDistance(size_t i, size_t j) const {
    return distances[i][j] / (double)sizes[i] / (double)sizes[j];
}

void DistanceManager::MergeClusters(size_t i, size_t j) {
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

void DistanceManager::CommitMerges() {
    DeleteIndexes();
    indexes_to_delete.clear();
    sizes = new_sizes;
    distances = new_distances;
}

void DistanceManager::DeleteIndexes() {
    std::sort(indexes_to_delete.begin(), indexes_to_delete.end());

    size_t move = 0;
    for (size_t i = 0; i < size; ++i) {
        if (toDeleteIndex(i)) {
            ++move;
            continue;
        }
        new_sizes[i - move] = new_sizes[i];
    }
    while (move-- > 0) {
        new_sizes.pop_back();
    }

    for (size_t i = 0; i < size; ++i) {
        move = 0;
        for (size_t j = 0; j < size; ++j) {
            if (toDeleteIndex(j)) {
                ++move;
                continue;
            }
            new_distances[i][j - move] = new_distances[i][j];
        }
        while (move-- > 0) {
            new_distances[i].pop_back();
        }
    }

    for (size_t j = 0; j < size - indexes_to_delete.size(); ++j) {
        move = 0;
        for (size_t i = 0; i < size; ++i) {
            if (toDeleteIndex(i)) {
                ++move;
                continue;
            }
            new_distances[i - move][j] = new_distances[i][j];
        }
    }
    for (size_t i = 0; i < indexes_to_delete.size(); ++i) {
        new_distances.pop_back();
    }

    size -= indexes_to_delete.size();
}

bool DistanceManager::toDeleteIndex(size_t index) {
    return std::find(indexes_to_delete.begin(),
                     indexes_to_delete.end(), index)
           != indexes_to_delete.end();
}