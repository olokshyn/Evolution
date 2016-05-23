//
// Created by Oleg on 4/21/16.
//

#ifndef WISHART_DISTANCEMANAGER_HPP
#define WISHART_DISTANCEMANAGER_HPP


#include <vector>
#include <exception>

#include "Cluster.hpp"


class IndexOutOfRangeException : std::exception {
};


class DistanceManager {

public:
    DistanceManager(const std::vector<Cluster>& clusters);

    double GetDistance(size_t i, size_t j) const;

    void MergeClusters(size_t i, size_t j);

    void CommitMerges();

private:
    size_t size;
    std::vector<std::vector<double>> distances;
    std::vector<std::vector<double>> new_distances;
    std::vector<size_t> sizes;
    std::vector<size_t> new_sizes;
    std::vector<size_t> indexes_to_delete;

    void DeleteIndexes();
    bool toDeleteIndex(size_t index);
};

#endif //WISHART_DISTANCEMANAGER_HPP
