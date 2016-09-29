//
// Created by Oleg on 3/13/16.
//

#include "AgglomerativeClustering.h"

#include <algorithm>
#include <vector>

#include "Cluster.hpp"
#include "DistanceManager.hpp"

extern "C" {
#include "Logging/Logging.h"
}

using namespace std;

#define DOUBLE_EPS 1e-5

template <typename T>
static bool IsInVector(const vector<T>& vec, const T& value);

static SpeciesList* ConvertToList(vector<Cluster>& level);

SpeciesList* AgglomerativeClustering(SpeciesList* clusters,
                                     EntitiesList* entities,
                                     size_t vector_length,
                                     double h) {
    LOG_FUNC_START("AgglomerativeClustering");

    try {
        LOG_ASSERT(h >= 0.0);
        Cluster::SetVectorLength(vector_length);
        size_t layer_size = clusters->length;
        if (entities) {
            layer_size += entities->length;
        }
        vector<Cluster> current_layer;
        current_layer.reserve(layer_size);
        for (ListIterator it = begin(clusters);
             !isIteratorExhausted(it);
             next(&it)) {
            current_layer.push_back(Cluster((Species*)it.current->value));
        }
        if (entities) {
            for (ListIterator it = begin(entities);
                 !isIteratorExhausted(it);
                 next(&it)) {
                current_layer.push_back(Cluster((Entity*)it.current->value));
            }
        }

        Log(INFO, "Before distance manager");
        DistanceManager distManager(current_layer);
        Log(INFO, "After distance manager");

        double max_distance = 0.0;
        while (layer_size > MAX_CLUSTER_COUNT
               && ((h > DOUBLE_EPS && max_distance <= h) || h < DOUBLE_EPS)) {
            LOG_ASSERT(layer_size == current_layer.size());
            vector<int> mergedClusters;
            vector<Cluster> next_layer;
            next_layer.reserve(layer_size / 2 + 1);

            Log(INFO, "Before layer loop");
            for (size_t i = 0; i < layer_size; ++i) {
                if (IsInVector<int>(mergedClusters, (int)i)) {
                    continue;
                }
                next_layer.push_back(current_layer[i]);
                mergedClusters.push_back((int)i);

                double min_distance = 0.0;
                int i_min_distance = -1;
                for (size_t j = i + 1; j < layer_size; ++j) {
                    double distance = distManager.GetDistance(i, j);
                    if (distance < min_distance || i_min_distance == -1) {
                        min_distance = distance;
                        i_min_distance = (int)j;
                    }
                }

                if (i_min_distance == -1
                    || IsInVector<int>(mergedClusters, i_min_distance)) {
                    continue;
                }
                next_layer.back().Add(current_layer[i_min_distance]);
                distManager.MergeClusters(i, (size_t)i_min_distance);
                mergedClusters.push_back(i_min_distance);
                if (min_distance > max_distance) {
                    max_distance = min_distance;
                }
            }
            Log(INFO, "After layer loop");
            distManager.CommitMerges();
            Log(INFO, "After merging");
            current_layer = move(next_layer);
            layer_size = current_layer.size();
        }

        LOG_FUNC_END("AgglomerativeClustering");
        return ConvertToList(current_layer);
    }
    catch (const std::bad_alloc&) {
        Log(ERROR, "AgglomerativeClustering allocation error");
        return NULL;
    }
}

template <typename T>
static bool IsInVector(const vector<T>& vec, const T& value) {
    return find(vec.begin(), vec.end(), value) != vec.end();
}

static SpeciesList* ConvertToList(vector<Cluster>& level) {
    SpeciesList* clusters = NULL;
    Species* species = NULL;

    clusters = CreateSpeciesList();
    if (!clusters) {
        goto error_ConvertToList;
    }

    for (size_t i = 0; i < level.size(); ++i) {
        species = level[i].Release();
        if (species->initial_size == 0) {
            species->initial_size = SPECIES_LENGTH(species);
            species->died = 0;
            MarkAllAsOld(species->entitiesList);
        }
#ifndef NDEBUG
        size_t old_count = 0;
        FOR_EACH_IN_SPECIES(species) {
            old_count += ENTITIES_IT_P->old;
        }
        LOG_ASSERT(species->initial_size == 0 || old_count != 0);
#endif
        if (!pushBack(clusters, species)) {
            goto error_ConvertToList;
        }
        species = NULL;
    }
    return clusters;

error_ConvertToList:
    DestroySpeciesList(clusters);
    DestroySpecies(species);
    return NULL;
}