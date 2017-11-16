//
// Created by Oleg on 3/13/16.
//

#include "AgglomerativeClustering.h"

#include <cstdlib>
#include <cmath>

#include <algorithm>
#include <vector>
#include <memory>
#include <type_traits>

#include "Cluster.hpp"
#include "DistanceManager.hpp"

extern "C"
{
#include "Common.h"
#include "Logging/Logging.h"
}

using namespace std;

template <typename T>
static bool IsInVector(const vector<T>& vec, const T& value);

static LIST_TYPE(SpeciesPtr) ConvertToPopulation(vector<Cluster>& level);

LIST_TYPE(SpeciesPtr) AgglomerativeClustering(World* world,
                                              LIST_TYPE(EntityPtr) new_entities,
                                              double eps,
                                              size_t min_pts)
{
    LOG_FUNC_START;

    try
    {
        LOG_ASSERT(eps >= 0.0);
        Cluster::set_chr_size(world->chr_size);
        size_t layer_size = list_len(world->population);
        if (new_entities)
        {
            layer_size += list_len(new_entities);
        }
        vector<Cluster> current_layer;
        current_layer.reserve(layer_size);
        list_for_each(SpeciesPtr, world->population, var)
        {
            current_layer.emplace_back(list_var_value(var));
        }
        if (new_entities)
        {
            list_for_each(EntityPtr, new_entities, var)
            {
                current_layer.emplace_back(list_var_value(var));
            }
        }

        Log(INFO, "Before distance manager");
        DistanceManager distManager(current_layer);
        Log(INFO, "After distance manager");

        double max_distance = 0.0;
        while (layer_size > MAX_CLUSTER_COUNT
               && ((eps > DOUBLE_EPS && max_distance <= eps)
                   || eps < DOUBLE_EPS))
        {
            LOG_ASSERT(layer_size == current_layer.size());
            vector<size_t> mergedClusters;
            vector<Cluster> next_layer;
            next_layer.reserve(layer_size / 2 + 1);

            Log(INFO, "Before layer loop");
            for (size_t i = 0; i < layer_size; ++i)
            {
                if (IsInVector(mergedClusters, i))
                {
                    continue;
                }
                next_layer.push_back(std::move(current_layer[i]));
                mergedClusters.push_back(i);

                double min_distance = 0.0;
                int i_min_distance = -1;
                for (size_t j = i + 1; j < layer_size; ++j)
                {
                    double distance = distManager.GetDistance(i, j);
                    if (distance < min_distance || i_min_distance == -1)
                    {
                        min_distance = distance;
                        i_min_distance = static_cast<int>(j);
                    }
                }

                if (i_min_distance == -1
                    || IsInVector(mergedClusters,
                                  static_cast<size_t>(i_min_distance)))
                {
                    continue;
                }
                next_layer.back().add(current_layer[i_min_distance]);
                distManager.MergeClusters(i, (size_t)i_min_distance);
                mergedClusters.push_back(static_cast<size_t>(i_min_distance));
                if (min_distance > max_distance)
                {
                    max_distance = min_distance;
                }
            }
            Log(INFO, "After layer loop");
            distManager.CommitMerges();
            Log(INFO, "After merging");
            current_layer = move(next_layer);
            layer_size = current_layer.size();
        }

        LOG_FUNC_END;
        return ConvertToPopulation(current_layer);
    }
    catch (const std::bad_alloc&)
    {
        Log(ERROR, "AgglomerativeClustering allocation error");
        return nullptr;
    }
}

template <typename T>
static bool IsInVector(const vector<T>& vec, const T& value)
{
    return find(vec.begin(), vec.end(), value) != vec.end();
}

static LIST_TYPE(SpeciesPtr) ConvertToPopulation(vector<Cluster>& level)
{
    std::unique_ptr<
            std::remove_pointer<LIST_TYPE(SpeciesPtr)>::type,
            decltype(&DestroyPopulation)>
            population(list_create(SpeciesPtr), &DestroyPopulation);
    if (!population)
    {
        throw std::bad_alloc();
    }

    std::unique_ptr<Species, decltype(&DestroySpecies)>
            species(nullptr, &DestroySpecies);

    for (size_t i = 0; i != level.size(); ++i)
    {
        species.reset(level[i].release());
        if (species->initial_size == 0)
        {
            species->initial_size = list_len(species->entities);
            species->died = 0;
            SetEntitiesStatus(species->entities, true);
        }
#ifndef NDEBUG
        size_t old_count = 0;
        list_for_each(EntityPtr, species->entities, var)
        {
            old_count += list_var_value(var)->old;
        }
        LOG_ASSERT(species->initial_size == 0 || old_count != 0);
#endif
        if (!list_push_back(SpeciesPtr, population.get(), species.get()))
        {
            throw std::bad_alloc();
        }
        species.release();
    }
    return population.release();
}
