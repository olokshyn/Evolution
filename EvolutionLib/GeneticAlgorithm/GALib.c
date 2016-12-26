//
// Created by Oleg on 9/27/16.
//

#include "GALib.h"

#include <math.h>

#include "GAParameters.h"
#include "Common.h"
#include "Logging/Logging.h"
#include "World.h"


int CountSpeciesLinks(List* fitness_list) {
    if (SPECIES_LINK_PROBABILITY == 0.0) {
        return 1;
    }

    int* counted = (int*)calloc(fitness_list->length, sizeof(int));
    if (!counted) {
        return 0;
    }

    size_t i = 0;
    for (ListIterator it1 = begin(fitness_list);
            !isIteratorExhausted(it1);
            next(&it1), ++i) {
        if (counted[i]) {
            continue;
        }
        size_t j = 0;
        for (ListIterator it2 = begin(fitness_list);
                !isIteratorExhausted(it2) && !counted[i];
                next(&it2), ++j) {
            if (it1.current == it2.current
                    || counted[j]
                    || !doWithProbability(SPECIES_LINK_PROBABILITY)) {
                continue;
            }
            *((double*)it1.current->value) +=
                    getRand(SPECIES_LINK_MIN, SPECIES_LINK_MAX)
                    *  *((double*)it2.current->value);
            counted[i] = 1;
            counted[j] = 1;
        }
        *((double*)it1.current->value) = MAX(*((double*)it1.current->value),
                                             1.0);
    }
    free(counted);

    Normalize(fitness_list);
    return 1;
}

double NonUniformMutationDelta(size_t t, double y,
                               const GAParameters* parameters) {
    double r = getRand(0.0, 1.0);
    return y * (1 - pow(r,
                        pow(1 - t / (double)parameters->max_generations_count,
                            parameters->mutation_on_iteration_dependence)));
}

int LinearRankingSelection(World* world,
                           Species* species,
                           size_t alive_count) {
    if (alive_count >= SPECIES_LENGTH(species)) {
        return 1;
    }

    double mu_minus = world->parameters->selection_worst_probability;
    double mu_plus = world->parameters->selection_best_probability;
    LOG_ASSERT(mu_minus >= 0 && mu_minus <= 1);
    LOG_ASSERT(mu_plus >= 1 && mu_plus <= 2);
    LOG_ASSERT(fabs(2 - mu_minus - mu_plus) < DOUBLE_EPS);
    LOG_ASSERT(world->parameters->selection_elitists_count < alive_count);

    Entity** entities_p = NULL;
    double* selection_probs = NULL;
    EntitiesList* sorted_new_entities = NULL;
    Entity* new_entity = NULL;

    entities_p = (Entity**)malloc(sizeof(Entity*) * SPECIES_LENGTH(species));
    if (!entities_p) {
        goto error_LinearRankingSelection;
    }
    selection_probs = (double*)calloc(SPECIES_LENGTH(species), sizeof(double));
    if (!selection_probs) {
        goto error_LinearRankingSelection;
    }
    size_t index = 0;
    double sum = 0.0;
    FOR_EACH_IN_SPECIES(species) {
        entities_p[index++] = ENTITIES_IT_P;
        sum += 1.0 / SPECIES_LENGTH(species)
               * (mu_minus + (mu_plus - mu_minus)
                             * (index - 1) / (SPECIES_LENGTH(species) - 1));
        selection_probs[index - 1] = sum;
    }
    qsort(entities_p,
          SPECIES_LENGTH(species),
          sizeof(Entity*),
          EntityAscendingComparator);

    sorted_new_entities = CreateEntitiesList();
    if (!sorted_new_entities) {
        goto error_LinearRankingSelection;
    }

    if (world->parameters->selection_elitists_count) {
        for (size_t i = 0; i != alive_count; ++i) {
            if (i == world->parameters->selection_elitists_count) {
                break;
            }
            new_entity = CopyEntity(entities_p[SPECIES_LENGTH(species) - i - 1],
                                    world->chr_size);
            if (!new_entity) {
                goto error_LinearRankingSelection;
            }
            if (!pushBack(sorted_new_entities, new_entity)) {
                goto error_LinearRankingSelection;
            }
            new_entity = NULL;
            entities_p[SPECIES_LENGTH(species) - i - 1] = NULL;
        }
    }

    if (alive_count >= world->parameters->selection_elitists_count) {
        alive_count -= world->parameters->selection_elitists_count;
    }

    for (size_t i = 0; i != alive_count; ++i) {
        double r = getRand(0, selection_probs[SPECIES_LENGTH(species) - 1]);
        for (size_t j = 0; j != SPECIES_LENGTH(species); ++j) {
            if (entities_p[j] && selection_probs[j] > r) {
                new_entity = CopyEntity(entities_p[j], world->chr_size);
                if (!new_entity) {
                    goto error_LinearRankingSelection;
                }
                if (!pushBack(sorted_new_entities, new_entity)) {
                    goto error_LinearRankingSelection;
                }
                new_entity = NULL;
                entities_p[j] = NULL;
                break;
            }
        }
    }

    for (size_t i = 0; i != SPECIES_LENGTH(species); ++i) {
        if (entities_p[i]) {
            LOG_ASSERT(entities_p[i]->old == 0 || entities_p[i]->old == 1);
            species->died += entities_p[i]->old;
        }
    }

    destroyListPointer(species->entitiesList);
    species->entitiesList = sorted_new_entities;
    sorted_new_entities = NULL;
    free(entities_p);
    free(selection_probs);
    entities_p = NULL;

    return 1;

error_LinearRankingSelection:
    free(entities_p);
    free(selection_probs);
    DestroyEntity(new_entity);
    destroyListPointer(sorted_new_entities);
    return 0;
}