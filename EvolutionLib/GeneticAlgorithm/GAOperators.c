//
// Created by Oleg on 12/24/16.
//

#include "GAOperators.h"

#include <math.h>

#include "World.h"
#include "GAParameters.h"
#include "Entity/Crossovers.h"
#include "OPTICS/optics.h"

#include "GALib.h"
#include "Logging/Logging.h"
#include "Common.h"

static bool CrossEntitiesWithProbability(World* world,
                                         LIST_TYPE(EntityPtr) entities,
                                         LIST_TYPE(EntityPtr) new_entities,
                                         double probability,
                                         size_t generation_number);

static bool PerformSelectionInEntities(World* world,
                                       LIST_TYPE(EntityPtr)* entities_ptr,
                                       size_t alive_count,
                                       size_t* entities_died);

static bool PerformLimitedSelectionInSpecies(World* world,
                                             Species* species,
                                             double norm_fitness);

static double ScaleEps(World* world,
                       LIST_TYPE(EntityPtr) new_entities,
                       double eps);

bool GAO_UniformMutation(World* world, size_t generation_number) {
    LOG_FUNC_START;

    if (!world->population) {
        goto error;
    }
    list_for_each(SpeciesPtr, world->population, species_var) {
        list_for_each(EntityPtr,
                      list_var_value(species_var)->entities,
                      entity_var) {
            if (doWithProbability(world->parameters->mutation_probability)) {
                size_t i = (size_t)selectRandom(0, (int)world->chr_size - 1);
                list_var_value(entity_var)->chr[i] =
                        getRand(world->parameters->objective.min,
                                world->parameters->objective.max);
                list_var_value(entity_var)->fitness =
                        world->parameters->objective.func(
                                list_var_value(entity_var)->chr,
                                (int)world->chr_size);
            }
        }
    }
    LOG_FUNC_SUCCESS;
    return true;

error:
    LOG_FUNC_ERROR;
    return false;
}

bool GAO_NonUniformMutation(World* world, size_t generation_number) {
    LOG_FUNC_START;

    if (!world->population) {
        goto error;
    }
    list_for_each(SpeciesPtr, world->population, species_var) {
        list_for_each(EntityPtr,
                      list_var_value(species_var)->entities,
                      entity_var) {
            if (doWithProbability(world->parameters->mutation_probability)) {
                size_t i = (size_t)selectRandom(0, (int)world->chr_size - 1);
                if (doWithProbability(0.5)) {
                    list_var_value(entity_var)->chr[i] +=
                            NonUniformMutationDelta(
                                    generation_number,
                                    world->parameters->objective.max
                                    - list_var_value(entity_var)->chr[i],
                                    world->parameters);
                }
                else {
                    list_var_value(entity_var)->chr[i] -=
                            NonUniformMutationDelta(
                                    generation_number,
                                    list_var_value(entity_var)->chr[i]
                                    - world->parameters->objective.min,
                                    world->parameters);
                }
            }
        }
    }
    LOG_FUNC_SUCCESS;
    return true;

error:
    LOG_FUNC_ERROR;
    return false;
}

LIST_TYPE(EntityPtr) GAO_UniformCrossover(World* world,
                                          size_t generation_number) {
    LOG_FUNC_START;

    LIST_TYPE(EntityPtr) new_entities = list_create(EntityPtr);
    if (!new_entities) {
        goto error;
    }

    Log(DEBUG, "Perform crossover in %zu species", list_len(world->population));

    list_for_each(SpeciesPtr, world->population, species_var) {
        if (!CrossEntitiesWithProbability(
                world,
                list_var_value(species_var)->entities,
                new_entities,
                world->parameters->crossover_probability,
                generation_number)) {
            goto destroy_new_entities;
        }
    }

    Log(DEBUG, "New entities count: %zu", list_len(new_entities));
    LOG_FUNC_SUCCESS;
    return new_entities;

destroy_new_entities:
    DestroyEntitiesList(new_entities);
error:
    LOG_FUNC_ERROR;
    return NULL;
}

LIST_TYPE(EntityPtr) GAO_FitnessCrossover(World* world,
                                          size_t generation_number) {
    LOG_FUNC_START;

    LIST_TYPE(EntityPtr) new_entities = list_create(EntityPtr);
    if (!new_entities) {
        goto error;
    }

    LIST_TYPE(double) fitnesses = NormalizePopulationFitnesses(world->population);
    if (!fitnesses) {
        goto destroy_new_entities;
    }

    LOG_ASSERT(list_len(new_entities) == list_len(fitnesses));

    Log(DEBUG, "Perform crossover in %zu species", list_len(world->population));

    LIST_ITER_TYPE(SpeciesPtr) species_iter =
            list_begin(SpeciesPtr, world->population);
    LIST_ITER_TYPE(double) fitness_iter = list_begin(double, fitnesses);

    for (; list_iter_valid(species_iter);
            list_next(SpeciesPtr, species_iter),
                    list_next(double, fitness_iter)) {

        LIST_TYPE(EntityPtr) entities = list_iter_value(species_iter)->entities;

        Log(DEBUG, "Crossing %zu entities", list_len(entities));
        if (list_len(entities) <= 1) {
            Log(DEBUG, "One or less entities in species");
            continue;
        }

        double crossover_prob = list_iter_value(fitness_iter);
        if (list_len(world->population) == 1) {
            crossover_prob = CROSSOVER_FIRST_TIME_PROB;
        }
        else if (list_len(entities) <= CROSSOVER_EXTINCTION_BIAS) {
            crossover_prob = 1.0;
        }
        else {
            crossover_prob = MAX(MIN(CROSSOVER_ALPHA * crossover_prob,
                                     CROSSOVER_MAX_PROB),
                                 CROSSOVER_MIN_PROB);
        }

        if (!CrossEntitiesWithProbability(
                world,
                entities,
                new_entities,
                crossover_prob,
                generation_number)) {
            goto destroy_fitnesses;
        }
    }

    list_destroy(double, fitnesses);

    Log(DEBUG, "New entities count: %zu", list_len(new_entities));
    LOG_FUNC_SUCCESS;
    return new_entities;

destroy_fitnesses:
    list_destroy(double, fitnesses);
destroy_new_entities:
    DestroyEntitiesList(new_entities);
error:
    LOG_FUNC_ERROR;
    return NULL;
}

LIST_TYPE(SpeciesPtr) GAO_Clustering(World* world,
                                     LIST_TYPE(EntityPtr) new_entities,
                                     double eps,
                                     size_t min_pts) {
    return OPTICSClustering(world, new_entities,
                            ScaleEps(world, new_entities, eps), min_pts);
}

bool GAO_ChildrenSelection(World* world, LIST_TYPE(EntityPtr)* new_entities) {
    return PerformSelectionInEntities(world, new_entities, world->size, NULL);
}

bool GAO_SpeciesLinksSelection(World* world) {
    LOG_FUNC_START;

    Log(DEBUG, "Old world size: %zu", world->size);

    LIST_TYPE(double) fitnesses = NormalizePopulationFitnesses(world->population);
    if (!fitnesses) {
        goto error;
    }

    if (!CountSpeciesLinks(fitnesses)) {
        goto destroy_fitnesses;
    }

    LOG_RELEASE_ASSERT(list_len(world->population) == list_len(fitnesses));

    size_t new_world_size = 0;
    LIST_ITER_TYPE(SpeciesPtr) species_iter =
            list_begin(SpeciesPtr, world->population);
    LIST_ITER_TYPE(double) fitness_iter = list_begin(double, fitnesses);
    for (; list_iter_valid(species_iter);
            list_next(SpeciesPtr, species_iter),
                    list_next(double, fitness_iter)) {
        if (!PerformLimitedSelectionInSpecies(
                world,
                list_iter_value(species_iter),
                list_iter_value(fitness_iter))) {
            goto destroy_fitnesses;
        }
        new_world_size += list_len(list_iter_value(species_iter)->entities);
    }

    Log(DEBUG, "New world size: %zu", new_world_size);
    world->size = new_world_size;

    for (species_iter = list_begin(SpeciesPtr, world->population);
            list_iter_valid(species_iter);
            ) {
        if (!list_len(list_iter_value(species_iter)->entities)) {
            LIST_ITER_TYPE(SpeciesPtr) temp = species_iter;
            list_next(SpeciesPtr, species_iter);
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

bool GAO_LinearRankingSelection(World* world) {
    LOG_FUNC_START;

    LIST_TYPE(double) fitnesses = NormalizePopulationFitnesses(world->population);
    if (!fitnesses) {
        goto error;
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
        if (!alive_count) {
            alive_count = 1;
        }
        if (!LinearRankingSelection(world,
                                    list_iter_value(species_iter),
                                    alive_count)) {
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

const GAOperators HerreraOperators = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .selection = GAO_LinearRankingSelection
};

const GAOperators HerreraWithClusteringOperators = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .clustering = GAO_Clustering,
        .selection = GAO_LinearRankingSelection
};

const GAOperators LokshynOperators = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .clustering = GAO_Clustering,
        .selection = GAO_SpeciesLinksSelection
};

// Static methods section -----------

static bool CrossEntitiesWithProbability(World* world,
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

static bool PerformSelectionInEntities(
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

static bool PerformLimitedSelectionInSpecies(World* world,
                                             Species* species,
                                             double norm_fitness) {
    double selection_part = MAX(norm_fitness, SELECTION_MIN);
    selection_part = MIN(selection_part, SELECTION_MAX);

    size_t species_size = (size_t)round(world->parameters->initial_world_size
                                        * selection_part);
    species_size = MIN(species_size, list_len(species->entities));
    species_size = MAX(species_size, CROSSOVER_EXTINCTION_BIAS);
    return PerformSelectionInEntities(
            world, &species->entities, species_size, &species->died);
}

static double ScaleEps(World* world,
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
