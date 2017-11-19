//
// Created by Oleg on 9/27/16.
//

#include "GALib.h"

#include <math.h>

#include "GAParameters.h"
#include "Common.h"
#include "Logging/Logging.h"
#include "World.h"


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

double NonUniformMutationDelta(size_t t, double y,
                               const GAParameters* parameters) {
    double r = getRand(0.0, 1.0);
    return y * (1 - pow(r,
                        pow(1 - t / (double)parameters->max_generations_count,
                            parameters->mutation_on_iteration_dependence)));
}

bool LinearRankingSelection(World* world,
                            Species* species,
                            size_t alive_count) {
    LOG_FUNC_START;

    if (alive_count >= list_len(species->entities)) {
        Log(DEBUG, "There are less entities %zu that should left alive %zu",
            list_len(species->entities), alive_count);
        goto exit;
    }
    LOG_RELEASE_ASSERT(list_len(species->entities) > 1);

    double mu_minus = world->parameters->selection_worst_probability;
    double mu_plus = world->parameters->selection_best_probability;
    LOG_ASSERT(mu_minus >= 0 && mu_minus <= 1);
    LOG_ASSERT(mu_plus >= 1 && mu_plus <= 2);
    LOG_ASSERT(fabs(2 - mu_minus - mu_plus) < DOUBLE_EPS);
    // LOG_ASSERT(world->parameters->selection_elitists_count < alive_count);

    double* selection_probs = (double*)calloc(list_len(species->entities),
                                              sizeof(double));
    if (!selection_probs) {
        goto error;
    }

    size_t index = 0;
    double sum = 0.0;
    list_for_each(EntityPtr, species->entities, var) {
        sum += 1.0 / list_len(species->entities)
               * (mu_minus + (mu_plus - mu_minus)
                             * (index) / (list_len(species->entities) - 1));
        selection_probs[index++] = sum;
    }

    LIST_TYPE(EntityPtr) sorted_new_entities = list_create(EntityPtr);
    if (!sorted_new_entities) {
        goto destroy_selection_probs;
    }

    Entity** entities_p = SortedEntitiesPointers(species->entities,
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
        double r = getRand(0, selection_probs[list_len(species->entities) - 1]);
        for (size_t j = 0; j != list_len(species->entities); ++j) {
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

    for (size_t i = 0; i != list_len(species->entities); ++i) {
        if (entities_p[i]) {
            species->died += entities_p[i]->old;
        }
    }

    DestroyEntitiesList(species->entities);
    species->entities = sorted_new_entities;
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