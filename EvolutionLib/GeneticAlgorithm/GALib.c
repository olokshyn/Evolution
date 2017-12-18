//
// Created by Oleg on 9/27/16.
//

#include "GALib.h"

#include <math.h>

#include "GAParameters.h"
#include "Common.h"
#include "Entity/Crossovers.h"
#include "Logging/Logging.h"
#include "World.h"

double NonUniformMutationDelta(size_t t, double y,
                               const GAParameters* parameters) {
    double r = getRand(0.0, 1.0);
    return y * (1 - pow(r,
                        pow(1 - t / (double)parameters->max_generations_count,
                            parameters->mutation_on_iteration_dependence)));
}

bool CrossEntitiesWithProbability(World* world,
                                  LIST_TYPE(EntityPtr) entities,
                                  LIST_TYPE(EntityPtr) new_entities,
                                  double probability,
                                  size_t generation_number) {
    LOG_FUNC_START;
    LOG_ASSERT(entities);
    LOG_ASSERT(new_entities);

    Log(DEBUG, "Crossing %zu entities", list_len(entities));
    if (list_len(entities) <= 1) {
        Log(DEBUG, "One or less entities in species");
        goto exit;
    }

    bool* crossed_parents = (bool*)calloc(list_len(entities), sizeof(bool));
    if (!crossed_parents) {
        goto error;
    }

    LIST_TYPE(double) entities_fitnesses = NormalizeEntitiesFitnesses(entities);
    if (!entities_fitnesses) {
        goto destroy_crossed_parents;
    }

    LOG_RELEASE_ASSERT(list_len(entities) == list_len(entities_fitnesses));

    Entity* child1 = NULL;
    Entity* child2 = NULL;

    size_t i = 0;
    LIST_ITER_TYPE(EntityPtr) iter_i = list_begin(EntityPtr, entities);
    LIST_ITER_TYPE(double) ft_iter_i = list_begin(double, entities_fitnesses);
    for (; list_iter_valid(iter_i);
            list_next(EntityPtr, iter_i), list_next(double, ft_iter_i), ++i) {
        if (crossed_parents[i]) {
            continue;
        }
        size_t j = 0;
        LIST_ITER_TYPE(EntityPtr) iter_j = list_begin(EntityPtr, entities);
        LIST_ITER_TYPE(double) ft_iter_j =
                list_begin(double, entities_fitnesses);
        for (; list_iter_valid(iter_j) && !crossed_parents[i];
                list_next(EntityPtr, iter_j),
                        list_next(double, ft_iter_j),
                        ++j) {
            if (list_iter_eq(iter_i, iter_j)
                || crossed_parents[j]
                || !doWithProbability(probability)) {

                continue;
            }
            LOG_RELEASE_ASSERT(!crossed_parents[i] && !crossed_parents[j]);

            child1 = CreateEntity(world->chr_size);
            if (!child1) {
                goto destroy_entities_fitnesses;
            }

            child2 = CreateEntity(world->chr_size);
            if (!child2) {
                goto destroy_child1;
            }

            DHXCrossover(list_iter_value(iter_i),
                         list_iter_value(iter_j),
                         child1,
                         child2,
                         &world->parameters->objective,
                         list_iter_value(ft_iter_i),
                         list_iter_value(ft_iter_j),
                         world->chr_size,
                         generation_number,
                         world->parameters->max_generations_count);
            if (!list_push_back(EntityPtr, new_entities, child2)) {
                goto destroy_child2;
            }
            child2 = NULL;
            if (!list_push_back(EntityPtr, new_entities, child1)) {
                goto destroy_child1;
            }
            child1 = NULL;

            crossed_parents[i] = true;
            crossed_parents[j] = true;
        }
    }

    list_destroy(double, entities_fitnesses);
    free(crossed_parents);
exit:
    LOG_FUNC_SUCCESS;
    return true;

destroy_child2:
    DestroyEntity(child2);
destroy_child1:
    DestroyEntity(child1);
destroy_entities_fitnesses:
    list_destroy(double, entities_fitnesses);
destroy_crossed_parents:
    free(crossed_parents);
error:
    LOG_FUNC_ERROR;
    return false;
}

bool CountSpeciesLinks(LIST_TYPE(double) fitnesses) {
    LOG_FUNC_START;

    if (DOUBLE_EQ(SPECIES_LINK_PROBABILITY, 0.0)) {
        goto exit;
    }

    bool* counted = (bool*)calloc(list_len(fitnesses), sizeof(bool));
    if (!counted) {
        goto error;
    }

    size_t i = 0;
    for (LIST_ITER_TYPE(double) iter_i = list_begin(double, fitnesses);
            list_iter_valid(iter_i);
            list_next(double, iter_i), ++i) {
        if (counted[i]) {
            continue;
        }
        size_t j = 0;
        for (LIST_ITER_TYPE(double) iter_j = list_begin(double, fitnesses);
                list_iter_valid(iter_j) && !counted[i];
                list_next(double, iter_j), ++j) {
            if (list_iter_eq(iter_i, iter_j)
                || counted[j]
                || !doWithProbability(SPECIES_LINK_PROBABILITY)) {

                continue;
            }
            list_iter_value(iter_i) +=
                    getRand(SPECIES_LINK_MIN, SPECIES_LINK_MAX)
                    * list_iter_value(iter_j);
            counted[i] = true;
            counted[j] = true;
        }
        list_iter_value(iter_i) = MAX(list_iter_value(iter_i), 1.0);
    }
    free(counted);
    Normalize(fitnesses);
exit:
    LOG_FUNC_SUCCESS;
    return true;

error:
    LOG_FUNC_ERROR;
    return false;
}

bool FitnessBasedSelectionTemplate(
        World* world,
        bool (*selection)(World* world,
                          LIST_TYPE(EntityPtr)* entities_ptr,
                          size_t alive_count,
                          size_t* entities_died),
        bool (*adjust_fitnesses)(LIST_TYPE(double) fitnesses)) {

    LOG_FUNC_START;

    LIST_TYPE(double) fitnesses = NormalizePopulationFitnesses(world->population);
    if (!fitnesses) {
        goto error;
    }
    if (adjust_fitnesses && !adjust_fitnesses(fitnesses)) {
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
        if (!selection(world,
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

    LOG_ASSERT(world->size != 0);
    LOG_FUNC_SUCCESS;
    return true;

destroy_fitnesses:
    list_destroy(double, fitnesses);
error:
    LOG_FUNC_ERROR;
    return false;
}

bool LinearRankingSelection(World* world,
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

    DestroyEntitiesList(*entities_ptr);
    *entities_ptr = sorted_new_entities;
    sorted_new_entities = NULL;
    free(entities_p);
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

bool PerformSelectionInEntities(
        World* world,
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
    LOG_RELEASE_ASSERT(list_len(entities) > 1);

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
            goto destroy_entity;
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

double ScaleEps(World* world,
                LIST_TYPE(EntityPtr) new_entities,
                double eps) {
    LOG_FUNC_START;

    size_t entities_size = world->size + list_len(new_entities);
    if (!entities_size) {
        Log(WARNING, "%s: There are no entities", __FUNCTION__);
        goto error;
    }

    const Entity* * entities = calloc(entities_size, sizeof(Entity*));
    if (!entities) {
        Log(ERROR, "%s: Failed to allocate entities", __FUNCTION__);
        goto error;
    }

    size_t next_index = 0;
    list_for_each(SpeciesPtr, world->population, species_var) {
        list_for_each(EntityPtr,
                      list_var_value(species_var)->entities,
                      entity_var) {
            LOG_ASSERT(next_index < entities_size);
            entities[next_index++] = list_var_value(entity_var);
        }
    }
    list_for_each(EntityPtr, new_entities, var) {
        LOG_ASSERT(next_index < entities_size);
        entities[next_index++] = list_var_value(var);
    }
    LOG_RELEASE_ASSERT(entities_size == next_index);

    double min_distance = -1.0;
    double max_distance = 0.0;

    for (size_t i = 0; i != entities_size - 1; ++i) {
        for (size_t j = i + 1; j != entities_size; ++j) {
            double distance = EuclidMeasure(entities[i]->chr,
                                            entities[j]->chr,
                                            world->chr_size);
            if (min_distance < 0.0 || min_distance > distance) {
                min_distance = distance;
            }
            if (max_distance < distance)
            {
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
            double distance = EuclidMeasure(entities[i]->chr,
                                            entities[j]->chr,
                                            world->chr_size);
            if (distance <= mid_distance) {
                ++small_distances;
            }
            else {
                ++large_distances;
            }
        }
    }

    free(entities);
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

bool GaussSLE(double** matrix, size_t rows, size_t cols, double* solution) {
    if (rows + 1 != cols) {
        return false;
    }

    // forward
    for (size_t i = 0; i != rows; ++i) {
        // find the row with the biggest key
        size_t max_key_row = i;
        for (size_t j = i + 1; j != rows; ++j) {
            if (fabs(matrix[j][i]) > fabs(matrix[max_key_row][i])) {
                max_key_row = j;
            }
        }
        if (max_key_row != i) {
            double* temp = matrix[i];
            matrix[i] = matrix[max_key_row];
            matrix[max_key_row] = temp;
        }

        // Check for the incompatible matrix
        if (DOUBLE_EQ(matrix[i][i], 0.0)) {
            return false;
        }

        for (size_t j = i + 1; j != rows; ++j) {
            double delimiter = - matrix[j][i] / matrix[i][i];
            for (size_t c = i; c != cols; ++c) {
                matrix[j][c] += matrix[i][c] * delimiter;
            }
        }
    }

    // backward
    for (size_t i = 0; i != rows; ++i) {
        size_t index = rows - i - 1;
        solution[index] = matrix[index][rows];
        for (size_t j = index + 1; j != rows; ++j) {
            solution[index] -= matrix[index][j] * solution[j];
        }
        solution[index] /= matrix[index][index];
    }

    return true;
}
