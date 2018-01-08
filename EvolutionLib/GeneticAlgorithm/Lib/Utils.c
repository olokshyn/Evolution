//
// Created by oleg on 08.01.18.
//

#include "Utils.h"

#include "Common.h"
#include "GeneticAlgorithm/World.h"
#include "Logging/Logging.h"

double ScaleEpsToEntitiesNorms(const World* world,
                               LIST_TYPE(EntityPtr) new_entities,
                               double eps) {
    LOG_FUNC_START;

    size_t entities_size = world->size + list_len(new_entities);
    if (!entities_size) {
        Log(WARNING, "%s: There are no entities", __FUNCTION__);
        goto error;
    }

    const Entity* * entities_ptrs = calloc(entities_size, sizeof(Entity*));
    if (!entities_ptrs) {
        Log(ERROR, "%s: Failed to allocate entities", __FUNCTION__);
        goto error;
    }

    size_t next_index = 0;
    list_for_each(SpeciesPtr, world->population, species_var) {
        list_for_each(EntityPtr,
                      list_var_value(species_var)->entities,
                      entity_var) {
            LOG_ASSERT(next_index < entities_size);
            entities_ptrs[next_index++] = list_var_value(entity_var);
        }
    }
    list_for_each(EntityPtr, new_entities, var) {
        LOG_ASSERT(next_index < entities_size);
        entities_ptrs[next_index++] = list_var_value(var);
    }
    LOG_RELEASE_ASSERT(entities_size == next_index);

    double min_distance = -1.0;
    double max_distance = 0.0;

    for (size_t i = 0; i != entities_size - 1; ++i) {
        for (size_t j = i + 1; j != entities_size; ++j) {
            double distance = EuclidMeasure(entities_ptrs[i]->chr,
                                            entities_ptrs[j]->chr,
                                            world->chr_size);
            if (min_distance < 0.0 || min_distance > distance) {
                min_distance = distance;
            }
            if (max_distance < distance) {
                max_distance = distance;
            }
        }
    }
    if (min_distance < 0.0) {
        min_distance = 0.0;
    }

    double mid_distance = (min_distance + max_distance) / 2;
    size_t small_distances = 0;
    size_t large_distances = 0;
    for (size_t i = 0; i != entities_size - 1; ++i) {
        for (size_t j = i + 1; j != entities_size; ++j) {
            double distance = EuclidMeasure(entities_ptrs[i]->chr,
                                            entities_ptrs[j]->chr,
                                            world->chr_size);
            if (distance <= mid_distance) {
                ++small_distances;
            }
            else {
                ++large_distances;
            }
        }
    }

    free(entities_ptrs);
    // small_distances / distances * 2
    // [0, 2], 1 - equally distributed
    double scaling_multiplier = small_distances / 0.5
                                / (double)(small_distances
                                           + large_distances);
    // [0.5 eps, 1.5 eps]
    eps = MAX(MIN(eps / 2.0 * (1.0 + scaling_multiplier), 0.95), 0.05);

    LOG_FUNC_SUCCESS;
    return min_distance + (max_distance - min_distance) * eps;

error:
    LOG_FUNC_ERROR;
    return 0.0;
}
