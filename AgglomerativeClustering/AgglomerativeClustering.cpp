//
// Created by Oleg on 3/13/16.
//

#include <algorithm>
#include <vector>

#include "AgglomerativeClustering.h"
#include "Cluster.hpp"

using namespace std;

#define DOUBLE_EPS 1e-5

template <typename T>
static bool IsInVector(const vector<T>& vec, const T& value);

static double EuclidMeasure(const vector<double>& vec1,
                            const vector<double>& vec2);

static List* ConvertToList(vector<Cluster<vector<double>>>& clusters);

List* AgglomerativeClustering(const double* const* vectors,
                              size_t vectors_count,
                              size_t vector_length,
                              size_t k,
                              double h) {
    assert(h >= 0.0);
    vector<vector<Cluster<vector<double>>>> levels(1);
    levels[0] = vector<Cluster<vector<double>>>(vectors_count);
    for (size_t i = 0; i < vectors_count; ++i) {
        vector<double> vec(vector_length);
        for (size_t j = 0; j < vector_length; ++j) {
            vec[j] = vectors[i][j];
        }
        levels[0][i].Add(vec);
    }

    size_t level = 0;
    double max_distance = 0.0;
    while (levels[level].size() > 1
           && ((h > DOUBLE_EPS && max_distance <= h) || h < DOUBLE_EPS)) {
        vector<int> mergedClusters;
        levels.push_back(vector<Cluster<vector<double>>>());
        for (size_t i = 0; i < levels[level].size(); ++i) {
            if (IsInVector<int>(mergedClusters, (int)i)) {
                continue;
            }
            levels[level + 1].push_back(levels[level][i]);
            mergedClusters.push_back((int)i);

            double min_distance = 0.0;
            int i_min_distance = -1;
            for (size_t j = i + 1; j < levels[level].size(); ++j) {
                double distance = levels[level][i].GetDistance(levels[level][j],
                                                               EuclidMeasure);
                if (distance < min_distance || i_min_distance == -1) {
                    min_distance = distance;
                    i_min_distance = (int)j;
                }
            }

            if (i_min_distance == -1
                || IsInVector<int>(mergedClusters, i_min_distance)) {
                continue;
            }
            levels[level + 1].back().Add(levels[level][i_min_distance]);
            mergedClusters.push_back(i_min_distance);
            if (min_distance > max_distance) {
                max_distance = min_distance;
            }
        }
        ++level;
    }

    size_t selected_level = (level > 2) ? level - 2 : level;
    return ConvertToList(levels[selected_level]);
}


template <typename T>
static bool IsInVector(const vector<T>& vec, const T& value) {
    return find(vec.begin(), vec.end(), value) != vec.end();
}


static double EuclidMeasure(const vector<double>& vec1,
                            const vector<double>& vec2) {
    assert(vec1.size() == vec2.size());
    double sum = 0;
    for (size_t i = 0; i < vec1.size(); ++i) {
        sum += pow(vec1[i] - vec2[i], 2.0);
    }
    return sqrt(sum);
}


static List* ConvertToList(vector<Cluster<vector<double>>>& clusters) {
    List* clustersList = (List*)malloc(sizeof(List));
    initList(clustersList, NULL, (void (*)(void*))clearListPointer);
    for (size_t i = 0; i < clusters.size(); ++i) {
        List* list = (List*)malloc(sizeof(List));
        initList(list, NULL, free);
        for (size_t j = 0; j < clusters[i].GetSize(); ++j) {
            double* vec = (double*)malloc(sizeof(double)
                                          * clusters[i][j].size());
            for (size_t k = 0; k < clusters[i][j].size(); ++k) {
                vec[k] = clusters[i][j][k];
            }
            pushBack(list, vec);
        }
        pushBack(clustersList, list);
    }
    return clustersList;
}