//
// Created by oleg on 08.01.18.
//

#include "Selection.h"

#include <math.h>

#include "Common.h"
#include "GeneticAlgorithm/World.h"
#include "GeneticAlgorithm/GAParameters.h"
#include "Logging/Logging.h"
#include "math_vector.h"
#include "Utils.h"

static _Thread_local double** g_centroids = NULL;
static _Thread_local double** g_matrix = NULL;
static _Thread_local double* g_species_size_ratio = NULL;

bool Selection_Template_FitnessBased(
        World* world,
        bool (*selection_entities)(const World* world,
                                   LIST_TYPE(EntityPtr)* entities_ptr,
                                   size_t alive_count,
                                   size_t* entities_died),
        bool (*adjust_fitnesses)(const World* world,
                                 LIST_TYPE(double) fitnesses)) {
    LOG_FUNC_START;

    LIST_TYPE(double) fitnesses = NormalizePopulationFitnesses(world->population);
    if (!fitnesses) {
        goto error;
    }
    if (adjust_fitnesses && !adjust_fitnesses(world, fitnesses)) {
        goto destroy_fitnesses;
    }
    ScaleSumToOne(fitnesses);

    LOG_RELEASE_ASSERT(list_len(world->population) == list_len(fitnesses));

    size_t new_world_size = 0;
    LIST_ITER_TYPE(SpeciesPtr) species_iter =
            list_begin(SpeciesPtr, world->population);
    LIST_ITER_TYPE(double) fitness_iter = list_begin(double, fitnesses);
    for (; list_iter_valid(species_iter);
           list_next(SpeciesPtr, species_iter),
                   list_next(double, fitness_iter)) {
        size_t alive_count = (size_t)round(world->parameters->initial_world_size
                                           * list_iter_value(fitness_iter));
        if (!selection_entities(world,
                                &list_iter_value(species_iter)->entities,
                                alive_count,
                                &list_iter_value(species_iter)->died)) {
            goto destroy_fitnesses;
        }
        new_world_size += list_len(list_iter_value(species_iter)->entities);
    }
    world->size = new_world_size;

    for (species_iter = list_begin(SpeciesPtr, world->population);
         list_iter_valid(species_iter);
            ) {
        if (!list_len(list_iter_value(species_iter)->entities)) {
            LIST_ITER_TYPE(SpeciesPtr) temp = species_iter;
            list_next(SpeciesPtr, species_iter);

            DestroySpecies(list_iter_value(temp));
            list_remove(SpeciesPtr, temp);
        }
        else {
            list_next(SpeciesPtr, species_iter);
        }
    }

    list_destroy(double, fitnesses);

    LOG_RELEASE_ASSERT(world->size != 0);
    LOG_FUNC_SUCCESS;
    return true;

destroy_fitnesses:
    list_destroy(double, fitnesses);
error:
    LOG_FUNC_ERROR;
    return false;
}

bool Selection_Entities_Linear(const World* world,
                               LIST_TYPE(EntityPtr)* entities_ptr,
                               size_t alive_count,
                               size_t* entities_died) {
    LOG_FUNC_START;
    LOG_RELEASE_ASSERT(entities_ptr);

    LIST_TYPE(EntityPtr) entities = *entities_ptr;

    if (alive_count >= list_len(entities)) {
        Log(DEBUG, "There are less entities %zu that should left alive %zu",
            list_len(entities), alive_count);
        goto exit;
    }
    if (alive_count < 2) {
        if (entities_died) {
            list_for_each(EntityPtr, entities, var) {
                *entities_died += list_var_value(var)->old;
            }
        }
        ClearEntitiesList(entities);
        goto exit;
    }
    LOG_RELEASE_ASSERT(list_len(entities) >= 2);

    Entity** entities_p =
            SortedEntitiesPointers(entities, EntityDescendingComparator);
    if (!entities_p) {
        goto error;
    }

    LIST_TYPE(EntityPtr) sorted_new_entities = list_create(EntityPtr);
    if (!sorted_new_entities) {
        goto destroy_entities_p;
    }

    Entity* new_entity = NULL;
    for (size_t i = 0; i != alive_count; ++i) {
        new_entity = CopyEntity(entities_p[i], world->chr_size);
        if (!new_entity) {
            goto destroy_sorted_new_entities;
        }
        if (!list_push_back(EntityPtr, sorted_new_entities, new_entity)) {
            goto destroy_new_entity;
        }
        new_entity = NULL;
    }

    if (entities_died) {
        for (size_t i = alive_count; i < list_len(entities); ++i) {
            *entities_died += entities_p[i]->old;
        }
    }

    DestroyEntitiesList(entities);
    *entities_ptr = sorted_new_entities;
    free(entities_p);

exit:
    LOG_FUNC_SUCCESS;
    return true;

destroy_new_entity:
    DestroyEntity(new_entity);
destroy_sorted_new_entities:
    DestroyEntitiesList(sorted_new_entities);
destroy_entities_p:
    free(entities_p);
error:
    LOG_FUNC_ERROR;
    return false;
}

// TODO: check this function
bool Selection_Entities_LinearRanking(const World* world,
                                      LIST_TYPE(EntityPtr)* entities_ptr,
                                      size_t alive_count,
                                      size_t* entities_died) {
    LOG_FUNC_START;

    LIST_TYPE(EntityPtr) entities = *entities_ptr;

    if (alive_count >= list_len(entities)) {
        Log(DEBUG, "There are less entities %zu that should left alive %zu",
            list_len(entities), alive_count);
        goto exit;
    }
    if (alive_count < 2) {
        if (entities_died) {
            list_for_each(EntityPtr, entities, var) {
                *entities_died += list_var_value(var)->old;
            }
        }
        ClearEntitiesList(entities);
        goto exit;
    }
    LOG_RELEASE_ASSERT(list_len(entities) > 1);

    double mu_minus = world->parameters->selection_worst_probability;
    double mu_plus = world->parameters->selection_best_probability;
    LOG_ASSERT(mu_minus >= 0 && mu_minus <= 1);
    LOG_ASSERT(mu_plus >= 1 && mu_plus <= 2);
    LOG_ASSERT(fabs(2 - mu_minus - mu_plus) < DOUBLE_EPS);
    // LOG_ASSERT(world->parameters->selection_elitists_count < alive_count);

    double* selection_probs = (double*)calloc(list_len(entities),
                                              sizeof(double));
    if (!selection_probs) {
        goto error;
    }

    size_t index = 0;
    double sum = 0.0;
    list_for_each(EntityPtr, entities, var) {
        sum += 1.0 / list_len(entities)
               * (mu_minus + (mu_plus - mu_minus)
                             * (index) / (list_len(entities) - 1));
        selection_probs[index++] = sum;
    }

    LIST_TYPE(EntityPtr) sorted_new_entities = list_create(EntityPtr);
    if (!sorted_new_entities) {
        goto destroy_selection_probs;
    }

    Entity** entities_p = SortedEntitiesPointers(entities,
                                                 EntityAscendingComparator);
    if (!entities_p) {
        goto destroy_sorted_new_entities;
    }

    Entity* new_entity = NULL;
    if (world->parameters->selection_elitists_count) {
        for (size_t i = 0;
             i != alive_count
             && i != world->parameters->selection_elitists_count;
             ++i) {
            new_entity = CopyEntity(entities_p[i], world->chr_size);
            if (!new_entity) {
                goto destroy_entities_p;
            }
            if (!list_push_back(EntityPtr, sorted_new_entities, new_entity)) {
                goto destroy_new_entity;
            }
            new_entity = NULL;
            entities_p[i] = NULL;
        }

        if (alive_count >= world->parameters->selection_elitists_count) {
            alive_count -= world->parameters->selection_elitists_count;
        }
    }

    for (size_t i = 0; i != alive_count; ++i) {
        double r = getRand(0, selection_probs[list_len(entities) - 1]);
        for (size_t j = 0; j != list_len(entities); ++j) {
            if (entities_p[j] && selection_probs[j] > r) {
                new_entity = CopyEntity(entities_p[j], world->chr_size);
                if (!new_entity) {
                    goto destroy_entities_p;
                }
                if (!list_push_back(EntityPtr, sorted_new_entities, new_entity)) {
                    goto destroy_new_entity;
                }
                new_entity = NULL;
                entities_p[j] = NULL;
                break;
            }
        }
    }

    if (entities_died) {
        for (size_t i = 0; i != list_len(entities); ++i) {
            if (entities_p[i]) {
                *entities_died += entities_p[i]->old;
            }
        }
    }

    free(entities_p);
    DestroyEntitiesList(*entities_ptr);
    *entities_ptr = sorted_new_entities;
    sorted_new_entities = NULL;
    free(selection_probs);
exit:
    LOG_FUNC_SUCCESS;
    return true;

destroy_new_entity:
    DestroyEntity(new_entity);
destroy_entities_p:
    free(entities_p);
destroy_sorted_new_entities:
    DestroyEntitiesList(sorted_new_entities);
destroy_selection_probs:
    free(selection_probs);
error:
    LOG_FUNC_ERROR;
    return false;
}

bool Selection_Entities_LinearBestWorst(const World* world,
                                        LIST_TYPE(EntityPtr)* entities_ptr,
                                        size_t alive_count,
                                        size_t* entities_died) {
    LOG_FUNC_START;
    LOG_RELEASE_ASSERT(entities_ptr);

    LIST_TYPE(EntityPtr) entities = *entities_ptr;

    if (alive_count >= list_len(entities)) {
        Log(DEBUG, "There are less entities %zu that should left alive %zu",
            list_len(entities), alive_count);
        goto exit;
    }
    if (alive_count < 2) {
        if (entities_died) {
            list_for_each(EntityPtr, entities, var) {
                *entities_died += list_var_value(var)->old;
            }
        }
        ClearEntitiesList(entities);
        goto exit;
    }
    LOG_RELEASE_ASSERT(list_len(entities) >= 2);

    Entity** entities_p =
            SortedEntitiesPointers(entities, EntityDescendingComparator);
    if (!entities_p) {
        goto error;
    }

    LIST_TYPE(EntityPtr) sorted_new_entities = list_create(EntityPtr);
    if (!sorted_new_entities) {
        goto destroy_entities_p;
    }

    size_t worst_alive_count = alive_count / 2;
    size_t best_alive_count = alive_count - worst_alive_count;

    Entity* new_entity = NULL;
    for (size_t i = 0; i != best_alive_count; ++i) {
        new_entity = CopyEntity(entities_p[i], world->chr_size);
        if (!new_entity) {
            goto destroy_sorted_new_entities;
        }
        if (!list_push_back(EntityPtr, sorted_new_entities, new_entity)) {
            goto destroy_entity;
        }
        new_entity = NULL;
        entities_p[i] = NULL;
    }
    for (size_t i = list_len(entities) - worst_alive_count; i != list_len(entities); ++i) {
        new_entity = CopyEntity(entities_p[i], world->chr_size);
        if (!new_entity) {
            goto destroy_sorted_new_entities;
        }
        if (!list_push_back(EntityPtr, sorted_new_entities, new_entity)) {
            goto destroy_entity;
        }
        new_entity = NULL;
        entities_p[i] = NULL;
    }

    if (entities_died) {
        for (size_t i = 0; i != list_len(entities); ++i) {
            if (entities_p[i]) {
                *entities_died += entities_p[i]->old;
            }
        }
    }

    free(entities_p);
    DestroyEntitiesList(entities);
    *entities_ptr = sorted_new_entities;

exit:
    LOG_FUNC_SUCCESS;
    return true;

destroy_entity:
    DestroyEntity(new_entity);
destroy_sorted_new_entities:
    DestroyEntitiesList(sorted_new_entities);
destroy_entities_p:
    free(entities_p);
error:
    LOG_FUNC_ERROR;
    return false;
}

struct __entity_desc {
    Entity* entity;
    double fitness;
};

static int __entity_desc_cmp(const void* a, const void* b) {
    // sort descending
    double diff = ((struct __entity_desc*)b)->fitness
                  - ((struct __entity_desc*)a)->fitness;
    return DOUBLE_EQ(diff, 0.0) ? 0 : SIGN(diff);
}

bool Selection_Entities_Scattering(const World* world,
                                   LIST_TYPE(EntityPtr)* entities_ptr,
                                   size_t alive_count,
                                   size_t* entities_died) {
    LOG_FUNC_START;

    const double scattering_power = 1.0;

    LIST_TYPE(EntityPtr) entities = *entities_ptr;

    if (alive_count >= list_len(entities)) {
        Log(DEBUG, "There are less entities %zu that should left alive %zu",
            list_len(entities), alive_count);
        goto exit;
    }
    if (alive_count < 2) {
        if (entities_died) {
            list_for_each(EntityPtr, entities, var) {
                *entities_died += list_var_value(var)->old;
            }
        }
        ClearEntitiesList(entities);
        goto exit;
    }
    LOG_RELEASE_ASSERT(list_len(entities) >= 2);

    size_t curr_centroid_index = 0;
#ifndef NDEBUG
    bool found = false;
#endif
    list_for_each(SpeciesPtr, world->population, var) {
        if (list_var_value(var)->entities == entities) {
#ifndef NDEBUG
            found = true;
#endif
            break;
        }
        ++curr_centroid_index;
    }
    LOG_ASSERT(found);
    size_t centroids_count = list_len(world->population);
    size_t closest_centroid_index = MinDistance(g_matrix, centroids_count, curr_centroid_index);

    const double* curr_centroid = g_centroids[curr_centroid_index];
    const double* closest_centroid = g_centroids[closest_centroid_index];

    double* direction = (double*)malloc(sizeof(double) * world->chr_size);
    if (!direction) {
        goto error;
    }
    math_vector_subtract(closest_centroid, curr_centroid, world->chr_size, direction);
    math_vector_normalize(direction, world->chr_size);
    math_vector_multiply(direction, -1.0, world->chr_size, direction);

    struct __entity_desc* descs =
            (struct __entity_desc*)malloc(sizeof(struct __entity_desc) * list_len(entities));
    if (!descs) {
        goto destroy_direction;
    }

    double* entity_direction = (double*)malloc(sizeof(double) * world->chr_size);
    if (!entity_direction) {
        goto destroy_descs;
    }

    // [0.0, 1.0]
    double distance_influence = (1.0 - g_matrix[curr_centroid_index][closest_centroid_index]);

    size_t index = 0;
    list_for_each(EntityPtr, entities, var) {
        descs[index].entity = list_var_value(var);

        math_vector_subtract(list_var_value(var)->chr, curr_centroid,
                             world->chr_size, entity_direction);
        math_vector_normalize(entity_direction, world->chr_size);

        // [-1.0, 1.0]
        double direction_influence =
                math_vector_dot_product(direction, entity_direction, world->chr_size);
        double influence = direction_influence
                           * distance_influence
                           * g_species_size_ratio[closest_centroid_index];

        descs[index].fitness = descs[index].entity->fitness
                               + fabs(descs[index].entity->fitness)
                                 * scattering_power
                                 * influence;

        ++index;
    }
    LOG_ASSERT(index == list_len(entities));
    free(entity_direction);

    qsort(descs, list_len(entities), sizeof(struct __entity_desc), __entity_desc_cmp);

    LIST_TYPE(EntityPtr) sorted_new_entities = list_create(EntityPtr);
    if (!sorted_new_entities) {
        goto destroy_descs;
    }

    Entity* new_entity = NULL;
    for (size_t i = 0; i != alive_count; ++i) {
        new_entity = CopyEntity(descs[i].entity, world->chr_size);
        if (!new_entity) {
            goto destroy_sorted_new_entities;
        }
        if (!list_push_back(EntityPtr, sorted_new_entities, new_entity)) {
            goto destroy_new_entity;
        }
        new_entity = NULL;
    }

    if (entities_died) {
        for (size_t i = alive_count; i < list_len(entities); ++i) {
            *entities_died += descs[i].entity->old;
        }
    }

    DestroyEntitiesList(entities);
    *entities_ptr = sorted_new_entities;
    free(descs);
    free(direction);

exit:
    LOG_FUNC_SUCCESS;
    return true;

destroy_new_entity:
    DestroyEntity(new_entity);
destroy_sorted_new_entities:
    DestroyEntitiesList(sorted_new_entities);
destroy_descs:
    free(descs);
destroy_direction:
    free(direction);
error:
    LOG_FUNC_ERROR;
    return false;
}

bool Selection_AdjustFitnesses_RandomSpeciesLinks(const World* world,
                                                  LIST_TYPE(double) fitnesses) {
    LOG_FUNC_START;

    if (DOUBLE_EQ(world->parameters->species_link_probability, 0.0)) {
        goto exit;
    }

    double** influence = (double**)calloc(list_len(fitnesses), sizeof(double*));
    if (!influence) {
        goto error;
    }
    for (size_t i = 0; i != list_len(fitnesses); ++i) {
        influence[i] = (double*)calloc(list_len(fitnesses), sizeof(double));
        if (!influence[i]) {
            goto destroy_influence;
        }
    }

    size_t i = 0;
    list_for_each(double, fitnesses, var_i) {
        size_t j = 0;
        list_for_each(double, fitnesses, var_j) {
            if (!list_var_eq(var_i, var_j)
                && doWithProbability(world->parameters->species_link_probability)) {

                influence[i][j] = getRand(world->parameters->species_link_min,
                                          world->parameters->species_link_max);
            }
            ++j;
        }
        LOG_ASSERT(j == list_len(fitnesses));
        ++i;
    }
    LOG_ASSERT(i == list_len(fitnesses));

    double* initial_fitnesses = (double*)malloc(sizeof(double) * list_len(fitnesses));
    if (!initial_fitnesses) {
        goto destroy_influence;
    }
    i = 0;
    list_for_each(double, fitnesses, var) {
        initial_fitnesses[i++] = list_var_value(var);
    }
    LOG_ASSERT(i == list_len(fitnesses));

    double* old_fitnesses = (double*)malloc(sizeof(double) * list_len(fitnesses));
    if (!old_fitnesses) {
        goto destroy_initial_fitnesses;
    }
    for (size_t k = 0; k != world->parameters->species_link_iterations_count; ++k) {
        i = 0;
        list_for_each(double, fitnesses, var) {
            old_fitnesses[i++] = list_var_value(var);
        }
        LOG_ASSERT(i == list_len(fitnesses));
        i = 0;
        list_for_each(double, fitnesses, var_i) {
            list_var_value(var_i) = initial_fitnesses[i];
            size_t j = 0;
            list_for_each(double, fitnesses, var_j) {
                if (!list_var_eq(var_i, var_j) && !DOUBLE_EQ(influence[i][j], 0.0)) {
                    list_var_value(var_i) += influence[i][j] * old_fitnesses[j];
                }
                ++j;
            }
            LOG_ASSERT(j == list_len(fitnesses));
            ++i;
        }
        LOG_ASSERT(i == list_len(fitnesses));
    }
    free(old_fitnesses);
    free(initial_fitnesses);
    for (size_t k = 0; k != list_len(fitnesses); ++k) {
        free(influence[k]);
    }
    free(influence);
    Normalize(fitnesses);
exit:
    LOG_FUNC_SUCCESS;
    return true;

destroy_initial_fitnesses:
    free(initial_fitnesses);
destroy_influence:
    for (size_t k = 0; k != list_len(fitnesses); ++k) {
        free(influence[k]);
    }
    free(influence);
error:
    LOG_FUNC_ERROR;
    return false;
}

bool Selection_AdjustFitnesses_SpeciesSizePenalty(const World* world,
                                                  LIST_TYPE(double) fitnesses) {
    LOG_FUNC_START;

    if (list_len(world->population) == 1) {
        Log(INFO, "%s: No penalty for one species", __FUNCTION__);
        goto exit;
    }

    const double penalty_power = 0.5;

    LIST_ITER_TYPE(SpeciesPtr) species_iter = list_begin(SpeciesPtr, world->population);
    LIST_ITER_TYPE(double) fitness_iter = list_begin(double, fitnesses);
    for (; list_iter_valid(species_iter) && list_iter_valid(fitness_iter);
           list_next(SpeciesPtr, species_iter), list_next(double, fitness_iter)) {
        double penalty = penalty_power
                         * list_len(list_iter_value(species_iter)->entities)
                         / (double)world->size;
        LOG_RELEASE_ASSERT(penalty < 1.0);
        if (penalty >= 0.5) {
            Log(DEBUG, "%s: penalty is more than 0.5 for species of size %zu",
                __FUNCTION__, list_len(list_iter_value(species_iter)->entities));
        }
        list_iter_value(fitness_iter) *= (1.0 - penalty);
    }
    LOG_ASSERT(!list_iter_valid(species_iter) && !list_iter_valid(fitness_iter));

exit:
    LOG_FUNC_SUCCESS;
    return true;
}

bool Selection_AdjustFitnesses_DamageTheBest(const World* world,
                                             LIST_TYPE(double) fitnesses) {
    LOG_FUNC_START;

    const double damaged_fitness = 0.75;

    LIST_ITER_TYPE(double) iter, max_iter;
    iter = max_iter = list_begin(double, fitnesses);
    while (list_iter_valid(iter)) {
        if (list_iter_value(iter) > list_iter_value(max_iter)) {
            max_iter = iter;
        }
        list_next(double, iter);
    }
    if (list_iter_valid(max_iter)) {
        list_iter_value(max_iter) = damaged_fitness;
    }

    LOG_FUNC_SUCCESS;
    return true;
}

bool Selection_Linear(World* world) {
    return Selection_Template_FitnessBased(world,
                                           Selection_Entities_Linear,
                                           NULL);
}

bool Selection_Linear_RandomSpeciesLinks(World* world) {
    return Selection_Template_FitnessBased(world,
                                           Selection_Entities_Linear,
                                           Selection_AdjustFitnesses_RandomSpeciesLinks);
}

bool Selection_Linear_SpeciesSizePenalty(World* world) {
    return Selection_Template_FitnessBased(world,
                                           Selection_Entities_Linear,
                                           Selection_AdjustFitnesses_SpeciesSizePenalty);
}

bool Selection_Linear_BestWorstPenalty(World* world) {
    return Selection_Template_FitnessBased(world,
                                           Selection_Entities_LinearBestWorst,
                                           Selection_AdjustFitnesses_SpeciesSizePenalty);
}

bool Selection_LinearRanking(World* world) {
    return Selection_Template_FitnessBased(world,
                                           Selection_Entities_LinearRanking,
                                           NULL);
}

bool Selection_Scattering(World* world) {
    LOG_FUNC_START;

    if (list_len(world->population) == 1) {
        if (!Selection_Linear(world)) {
            goto error;
        }
        goto exit;
    }

    size_t centroids_count = list_len(world->population);
    g_centroids = GetPopulationCentroids(world);
    if (!g_centroids) {
        goto error;
    }

    g_matrix = DistanceMatrix(g_centroids, centroids_count, world->chr_size);
    if (!g_matrix) {
        goto destroy_g_centroids;
    }
    NormalizeDistanceMatrix(g_matrix, centroids_count);

    g_species_size_ratio = (double*)malloc(sizeof(double) * centroids_count);
    if (!g_species_size_ratio) {
        goto destroy_g_matrix;
    }
    size_t index = 0;
    list_for_each(SpeciesPtr, world->population, var) {
        g_species_size_ratio[index++] = list_len(list_var_value(var)->entities)
                                        / (double)world->size;
    }
#ifndef NDEBUG
    double sum = 0.0;
    for (size_t i = 0; i != centroids_count; ++i) {
        sum += g_species_size_ratio[i];
    }
    LOG_ASSERT(DOUBLE_EQ(sum, 1.0));
#endif

    if (!Selection_Template_FitnessBased(world,
                                         Selection_Entities_Scattering,
                                         NULL)) {
        goto destroy_g_species_size_ratio;
    }

    free(g_species_size_ratio);
    g_species_size_ratio = NULL;
    DestroyDistanceMatrix(g_matrix, centroids_count);
    g_matrix = NULL;
    DestroyCentroids(g_centroids, centroids_count);
    g_centroids = NULL;

exit:
    LOG_FUNC_SUCCESS;
    return true;

destroy_g_species_size_ratio:
    free(g_species_size_ratio);
    g_species_size_ratio = NULL;
destroy_g_matrix:
    DestroyDistanceMatrix(g_matrix, centroids_count);
    g_matrix = NULL;
destroy_g_centroids:
    DestroyCentroids(g_centroids, centroids_count);
    g_centroids = NULL;
error:
    LOG_FUNC_ERROR;
    return false;
}

bool Selection_Children_Linear(World* world, LIST_TYPE(EntityPtr)* new_entities) {
    return Selection_Entities_Linear(world, new_entities, world->size, NULL);
}
