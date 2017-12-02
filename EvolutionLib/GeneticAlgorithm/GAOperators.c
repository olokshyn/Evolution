//
// Created by Oleg on 12/24/16.
//

#include "GAOperators.h"

#include <math.h>

#include "World.h"
#include "GAParameters.h"
#include "OPTICS/optics.h"

#include "GALib.h"
#include "Logging/Logging.h"
#include "Common.h"

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
    Species* noise_species = CreateSpecies(0);
    if (!noise_species) {
        goto error;
    }

    LIST_TYPE(SpeciesPtr) population = OPTICSClustering(
            world,
            new_entities,
            ScaleEps(world, new_entities, eps),
            min_pts,
            noise_species->entities);
    if (!population) {
        goto destroy_noise_species;
    }

    if (list_len(noise_species->entities) > 1) {
        noise_species->initial_size = list_len(noise_species->entities);
        SetEntitiesStatus(noise_species->entities, true);
        if (!list_push_back(SpeciesPtr, population, noise_species)) {
            goto destroy_population;
        }
    }
    else
    {
        DestroySpecies(noise_species);
    }
    noise_species = NULL;

    return population;

destroy_population:
    DestroyPopulation(population);
destroy_noise_species:
    DestroySpecies(noise_species);
error:
    return NULL;
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
    return FitnessBasedSelectionTemplate(world, LinearRankingSelection);
}

bool GAO_ElitistsSelection(World* world) {
    return FitnessBasedSelectionTemplate(world, PerformSelectionInEntities);
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
        .selection = GAO_ElitistsSelection
};

const GAOperators LokshynOperators = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .clustering = GAO_Clustering,
        .selection = GAO_SpeciesLinksSelection
};
