//
// Created by oleg on 08.01.18.
//

#include "Crossover.h"

#include "Common.h"
#include "GeneticAlgorithm/World.h"
#include "GeneticAlgorithm/GAParameters.h"
#include "Entity/Crossovers.h"
#include "Logging/Logging.h"

bool Crossover_Entities_Template_Probabilistic(
        const World* world,
        LIST_TYPE(EntityPtr) entities,
        LIST_TYPE(EntityPtr) new_entities,
        double probability,
        size_t generation_number,
        bool (* crossover)(const Entity* parent1,
                           const Entity* parent2,
                           Entity* child1,
                           Entity* child2,
                           const Objective* obj,
                           double fitness1,  // normalized
                           double fitness2,  // normalized
                           size_t chromosome_size,
                           size_t generation_number,
                           size_t max_generations_count)) {
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

            if (!crossover(list_iter_value(iter_i),
                           list_iter_value(iter_j),
                           child1,
                           child2,
                           world->parameters->objective,
                           list_iter_value(ft_iter_i),
                           list_iter_value(ft_iter_j),
                           world->chr_size,
                           generation_number,
                           world->parameters->max_generations_count)) {
                goto destroy_child2;
            }
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

bool Crossover_Entities_OnePoint(const World* world,
                                 LIST_TYPE(EntityPtr) entities,
                                 LIST_TYPE(EntityPtr) new_entities,
                                 double probability,
                                 size_t generation_number) {
    return Crossover_Entities_Template_Probabilistic(world,
                                                     entities,
                                                     new_entities,
                                                     probability,
                                                     generation_number,
                                                     OnePointCrossover);
}

bool Crossover_Entities_DHX(const World* world,
                            LIST_TYPE(EntityPtr) entities,
                            LIST_TYPE(EntityPtr) new_entities,
                            double probability,
                            size_t generation_number) {
    return Crossover_Entities_Template_Probabilistic(world,
                                                     entities,
                                                     new_entities,
                                                     probability,
                                                     generation_number,
                                                     DHXCrossover);
}

LIST_TYPE(EntityPtr) Crossover_Template(
        World* world,
        size_t generation_number,
        bool (*crossover_entities)(const World* world,
                                   LIST_TYPE(EntityPtr) entities,
                                   LIST_TYPE(EntityPtr) new_entities,
                                   double probability,
                                   size_t generation_number)) {
    LOG_FUNC_START;

    LIST_TYPE(EntityPtr) new_entities = list_create(EntityPtr);
    if (!new_entities) {
        goto error;
    }

    Log(DEBUG, "Perform crossover in %zu species", list_len(world->population));

    list_for_each(SpeciesPtr, world->population, species_var) {
        if (!crossover_entities(
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

LIST_TYPE(EntityPtr) Crossover_OnePoint(World* world,
                                        size_t generation_number) {
    return Crossover_Template(world, generation_number, Crossover_Entities_OnePoint);
}

LIST_TYPE(EntityPtr) Crossover_DHX(World* world,
                                   size_t generation_number) {
    return Crossover_Template(world, generation_number, Crossover_Entities_DHX);
}
