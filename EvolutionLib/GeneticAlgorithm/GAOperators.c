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

static int PerformSelectionInSpecies(World* world,
                                     Species* species,
                                     size_t alive_count);

static int PerformLimitedSelectionInSpecies(World* world,
                                            Species* species,
                                            double norm_fitness);

static double ScaleEps(World* world, Species* new_entities, double eps);

int GAO_UniformMutation(World* world, size_t generation_number) {
    FOR_EACH_IN_SPECIES_LIST(&world->species) {
        FOR_EACH_IN_SPECIES(SPECIES_LIST_IT_P) {
            if (doWithProbability(world->parameters->mutation_probability)) {
                size_t i = (size_t)selectRandom(0, (int)world->chr_size - 1);
                ENTITIES_IT_P->chr[i] = getRand(world->parameters->objective.min,
                                                world->parameters->objective.max);
                ENTITIES_IT_P->fitness =
                        world->parameters->objective.func(ENTITIES_IT_P->chr,
                                                          (int)world->chr_size);
            }
        }
    }
    return 1;
}

int GAO_NonUniformMutation(World* world, size_t generation_number) {
    FOR_EACH_IN_SPECIES_LIST(&world->species) {
        FOR_EACH_IN_SPECIES(SPECIES_LIST_IT_P) {
            if (doWithProbability(world->parameters->mutation_probability)) {
                size_t i = (size_t)selectRandom(0, (int)world->chr_size - 1);
                if (doWithProbability(0.5)) {
                    ENTITIES_IT_P->chr[i] +=
                            NonUniformMutationDelta(generation_number,
                                    world->parameters->objective.max
                                            - ENTITIES_IT_P->chr[i],
                                    world->parameters);
                }
                else {
                    ENTITIES_IT_P->chr[i] -=
                            NonUniformMutationDelta(generation_number,
                                     ENTITIES_IT_P->chr[i]
                                             - world->parameters->objective.min,
                                     world->parameters);
                }
            }
        }
    }
    return 1;
}

Species* GAO_UniformCrossover(World* world, size_t generation_number) {
    LOG_FUNC_START("GAO_UniformCrossover");

    Species* new_species = NULL;
    short* crossed_parents = NULL;
    List* en_ft_list = NULL;
    Entity* new_entity1 = NULL;
    Entity* new_entity2 = NULL;

    new_species = CreateSpecies(0);
    if (!new_species) {
        goto error_GAO_UniformCrossover;
    }

    // TODO: add %zu modifier to Log and use it instead of %d
    Log(DEBUG, "Perform crossover in %d species", (int)world->species.length);

    FOR_EACH_IN_SPECIES_LIST(&world->species) {
        Species* species = SPECIES_LIST_IT_P;
        Log(INFO, "Crossing %d entities", (int)SPECIES_LENGTH(species));
        if (SPECIES_LENGTH(species) <= 1) {
            Log(DEBUG, "One or less entities in species");
            continue;
        }

        crossed_parents = (short*)calloc(SPECIES_LENGTH(species),
                                         sizeof(short));
        if (!crossed_parents) {
            goto error_GAO_UniformCrossover;
        }

        en_ft_list = NormalizeEntitiesFitnesses(species->entitiesList);
        if (!en_ft_list) {
            goto error_GAO_UniformCrossover;
        }

        size_t i = 0;
        for (ListIterator it1 = begin(species->entitiesList),
                     ft_it1 = begin(en_ft_list);
                     !isIteratorExhausted(it1);
                     next(&it1), next(&ft_it1), ++i) {
            if (crossed_parents[i]) {
                continue;
            }
            size_t j = 0;
            for (ListIterator it2 = begin(species->entitiesList),
                         ft_it2 = begin(en_ft_list);
                         !isIteratorExhausted(it2) && !crossed_parents[i];
                         next(&it2), next(&ft_it2), ++j) {
                if (it1.current == it2.current
                            || crossed_parents[j]
                            || !doWithProbability(
                                world->parameters->crossover_probability)) {
                    continue;
                }
                LOG_ASSERT(!crossed_parents[i] && !crossed_parents[j]);

                new_entity1 = CreateEntity(world->chr_size);
                if (!new_entity1) {
                    goto error_GAO_UniformCrossover;
                }
                new_entity2 = CreateEntity(world->chr_size);
                if (!new_entity2) {
                    goto error_GAO_UniformCrossover;
                }
                DHXCrossover((Entity*)it1.current->value,
                             (Entity*)it2.current->value,
                             new_entity1,
                             new_entity2,
                             &world->parameters->objective,
                             *((double*)ft_it1.current->value),
                             *((double*)ft_it2.current->value),
                             world->chr_size,
                             generation_number,
                             world->parameters->max_generations_count);
                if (!pushBack(new_species->entitiesList, new_entity1)) {
                    goto error_GAO_UniformCrossover;
                }
                new_entity1 = NULL;
                if (!pushBack(new_species->entitiesList, new_entity2)) {
                    goto error_GAO_UniformCrossover;
                }
                new_entity2 = NULL;

                crossed_parents[i] = 1;
                crossed_parents[j] = 1;
            }
        }
        free(crossed_parents);
        crossed_parents = NULL;
        destroyListPointer(en_ft_list);
        en_ft_list = NULL;
    }

    Log(DEBUG, "New entities count: %d", (int)SPECIES_LENGTH(new_species));

    LOG_FUNC_END("GAO_UniformCrossover");

    return new_species;

error_GAO_UniformCrossover:
    DestroySpecies(new_species);
    free(crossed_parents);
    destroyListPointer(en_ft_list);
    DestroyEntity(new_entity1);
    DestroyEntity(new_entity2);
    return NULL;
}

Species* GAO_FitnessCrossover(World* world, size_t generation_number) {
    LOG_FUNC_START("GAO_FitnessCrossover");

    Species* new_species = NULL;
    List* fitness_list = NULL;
    List* en_ft_list = NULL;
    Entity* new_entity1 = NULL;
    Entity* new_entity2 = NULL;
    short* crossed_parents = NULL;

    new_species = CreateSpecies(0);
    if (!new_species) {
        goto error_PerformCrossover;
    }

    fitness_list = NormalizeSpeciesFitnesses(&world->species);
    if (!fitness_list) {
        goto error_PerformCrossover;
    }

    // TODO: add %zu modifier to Log and use it instead of %d
    Log(DEBUG, "Perform crossover in %d species", (int)world->species.length);

    for (ListIterator speciesIt = begin(&world->species),
                 fitnessIt = begin(fitness_list);
             !isIteratorExhausted(speciesIt);
             next(&speciesIt), next(&fitnessIt)) {
        Species* species = (Species*)speciesIt.current->value;
        Log(INFO, "Crossing %d entities", (int)SPECIES_LENGTH(species));
        if (SPECIES_LENGTH(species) <= 1) {
            Log(DEBUG, "One or less entities in species");
            continue;
        }

        double crossover_prob = *(double*)fitnessIt.current->value;
        if (world->species.length == 1) {
            crossover_prob = CROSSOVER_FIRST_TIME_PROB;
        }
        else if (SPECIES_LENGTH(species) <= CROSSOVER_EXTINCTION_BIAS) {
            crossover_prob = 1.0;
        }
        else {
            crossover_prob = MAX(MIN(CROSSOVER_ALPHA * crossover_prob,
                                     CROSSOVER_MAX_PROB),
                                 CROSSOVER_MIN_PROB);
        }

        crossed_parents = (short*)calloc(SPECIES_LENGTH(species),
                                         sizeof(short));
        if (!crossed_parents) {
            goto error_PerformCrossover;
        }

        en_ft_list = NormalizeEntitiesFitnesses(species->entitiesList);
        if (!en_ft_list) {
            goto error_PerformCrossover;
        }

        size_t i = 0;
        for (ListIterator it1 = begin(species->entitiesList),
                     ft_it1 = begin(en_ft_list);
             !isIteratorExhausted(it1);
             next(&it1), next(&ft_it1), ++i) {
            if (crossed_parents[i]) {
                continue;
            }
            size_t j = 0;
            for (ListIterator it2 = begin(species->entitiesList),
                         ft_it2 = begin(en_ft_list);
                 !isIteratorExhausted(it2) && !crossed_parents[i];
                 next(&it2), next(&ft_it2), ++j) {
                if (it1.current == it2.current
                        || crossed_parents[j]
                        || !doWithProbability(crossover_prob)) {
                    continue;
                }
                new_entity1 = CreateEntity(world->chr_size);
                if (!new_entity1) {
                    goto error_PerformCrossover;
                }
                new_entity2 = CreateEntity(world->chr_size);
                if (!new_entity2) {
                    goto error_PerformCrossover;
                }
                DHXCrossover((Entity*)it1.current->value,
                             (Entity*)it2.current->value,
                             new_entity1,
                             new_entity2,
                             &world->parameters->objective,
                             *((double*)ft_it1.current->value),
                             *((double*)ft_it2.current->value),
                             world->chr_size,
                             generation_number,
                             world->parameters->max_generations_count);
                if (!pushBack(new_species->entitiesList, new_entity1)) {
                    goto error_PerformCrossover;
                }
                new_entity1 = NULL;
                if (!pushBack(new_species->entitiesList, new_entity2)) {
                    goto error_PerformCrossover;
                }
                new_entity2 = NULL;
                LOG_ASSERT(!crossed_parents[i] && !crossed_parents[j]);
                crossed_parents[i] = 1;
                crossed_parents[j] = 1;
            }
        }
        free(crossed_parents);
        crossed_parents = NULL;
        destroyListPointer(en_ft_list);
        en_ft_list = NULL;
    }

    destroyListPointer(fitness_list);

    Log(DEBUG, "New entities count: %d", (int)SPECIES_LENGTH(new_species));

    LOG_FUNC_END("GAO_FitnessCrossover");

    return new_species;

error_PerformCrossover:
    DestroySpecies(new_species);
    destroyListPointer(fitness_list);
    destroyListPointer(en_ft_list);
    DestroyEntity(new_entity1);
    DestroyEntity(new_entity2);
    free(crossed_parents);
    return NULL;
}

SpeciesList* GAO_Clustering(World* world, Species* new_species,
                            double eps, size_t min_pts) {
    SpeciesList* clustered_species = OPTICSClustering(
            world, new_species,
            ScaleEps(world, new_species, world->parameters->eps),
            world->parameters->min_pts);
    if (!clustered_species) {
        goto error_PerformClustering;
    }
    return clustered_species;

error_PerformClustering:
    return NULL;
}

int GAO_ChildrenSelection(World* world, Species* new_species) {
    return PerformSelectionInSpecies(world, new_species, world->world_size);
}

int GAO_SpeciesLinksSelection(World* world) {
    LOG_FUNC_START("GAO_SpeciesLinksSelection");

    Log(DEBUG, "Old world size: %d", (int)world->world_size);

    SpeciesList* clustered_species = &world->species;

    List* fitness_list = NormalizeSpeciesFitnesses(clustered_species);
    if (!fitness_list) {
        goto error_PerformSelection;
    }

    if (!CountSpeciesLinks(fitness_list)) {
        goto error_PerformSelection;
    }

    size_t new_world_size = 0;

    Log(INFO, "Before selection in clustered_species");
    for (ListIterator it_cl = begin(clustered_species),
                it_ft = begin(fitness_list);
                !isIteratorExhausted(it_cl);
                next(&it_cl), next(&it_ft)) {
        PerformLimitedSelectionInSpecies(world,
                                         (Species*)(it_cl.current->value),
                                         *((double*)it_ft.current->value));
        new_world_size += SPECIES_LENGTH(it_cl.current->value);
    }
    Log(INFO, "After selection in clustered_species");

    Log(DEBUG, "New world size: %d", (int)new_world_size);
    world->world_size = new_world_size;

    for (ListIterator it = begin(clustered_species);
         !isIteratorExhausted(it);
            ) {
        if (SPECIES_LENGTH(it.current->value) == 0) {
            ListIterator temp_it = it;
            next(&it);
            removeFromList(temp_it);
        }
        else {
            next(&it);
        }
    }

    destroyListPointer(fitness_list);

    LOG_ASSERT(world->world_size != 0);

    LOG_FUNC_END("GAO_SpeciesLinksSelection");

    return 1;

error_PerformSelection:
    destroyListPointer(fitness_list);
    return 0;
}

int GAO_LinearRankingSelection(World* world) {

    List* fitness_list = NormalizeSpeciesFitnesses(&world->species);
    if (!fitness_list) {
        goto error_GAO_LinearRankingSelection;
    }

    size_t new_world_size = 0;
    for (ListIterator sp_it = begin(&world->species),
                 ft_it = begin(fitness_list);
            !isIteratorExhausted(sp_it);
            next(&sp_it), next(&ft_it)) {
        Species* species = (Species*)sp_it.current->value;
        size_t alive_count = (size_t)round(world->parameters->initial_world_size
                                           * *(double*)ft_it.current->value);
        if (!alive_count) {
            ++alive_count;
        }
        if (!LinearRankingSelection(world, species, alive_count)) {
            goto error_GAO_LinearRankingSelection;
        }
        new_world_size += SPECIES_LENGTH(species);
    }
    world->world_size = new_world_size;

    for (ListIterator it = begin(&world->species);
            !isIteratorExhausted(it);
            ) {
        if (SPECIES_LENGTH(it.current->value) == 0) {
            ListIterator temp_it = it;
            next(&it);
            removeFromList(temp_it);
        }
        else {
            next(&it);
        }
    }

    destroyListPointer(fitness_list);
    return 1;

error_GAO_LinearRankingSelection:
    destroyListPointer(fitness_list);
    return 0;
}

// Static methods section -----------

static int PerformSelectionInSpecies(World* world,
                                     Species* species,
                                     size_t alive_count) {
    if (alive_count >= SPECIES_LENGTH(species)) {
        return 1;
    }
    Entity** entities_p = NULL;
    EntitiesList* sorted_new_entities = NULL;
    Entity* new_entity = NULL;

    entities_p = (Entity**)malloc(sizeof(Entity*) * SPECIES_LENGTH(species));
    if (!entities_p) {
        goto error_PerformSelectionInSpecies;
    }
    size_t index = 0;
    FOR_EACH_IN_SPECIES(species) {
        entities_p[index++] = ENTITIES_IT_P;
    }
    qsort(entities_p,
          SPECIES_LENGTH(species),
          sizeof(Entity*),
          EntityDescendingComparator);

    sorted_new_entities = CreateEntitiesList();
    if (!sorted_new_entities) {
        goto error_PerformSelectionInSpecies;
    }
    for (size_t i = 0; i < alive_count; ++i) {
        new_entity = CopyEntity(entities_p[i], world->chr_size);
        if (!new_entity) {
            goto error_PerformSelectionInSpecies;
        }
        if (!pushBack(sorted_new_entities, new_entity)) {
            goto error_PerformSelectionInSpecies;
        }
        new_entity = NULL;
    }

    for (size_t i = alive_count; i < SPECIES_LENGTH(species); ++i) {
        LOG_ASSERT(entities_p[i]->old == 0 || entities_p[i]->old == 1);
        species->died += entities_p[i]->old;
    }

    destroyListPointer(species->entitiesList);
    species->entitiesList = sorted_new_entities;
    sorted_new_entities = NULL;
    free(entities_p);
    entities_p = NULL;

    return 1;

error_PerformSelectionInSpecies:
    free(entities_p);
    destroyListPointer(sorted_new_entities);
    DestroyEntity(new_entity);
    return 0;
}

static int PerformLimitedSelectionInSpecies(World* world,
                                            Species* species,
                                            double norm_fitness) {
    double selection_part = MAX(norm_fitness, SELECTION_MIN);
    selection_part = MIN(selection_part, SELECTION_MAX);

    size_t species_size = (size_t)round(world->parameters->initial_world_size
                                        * selection_part);
    species_size = MIN(species_size, SPECIES_LENGTH(species));
    species_size = MAX(species_size, CROSSOVER_EXTINCTION_BIAS);
    return PerformSelectionInSpecies(world, species, species_size);
}

static double ScaleEps(World* world, Species* new_entities, double eps) {
    size_t entities_size = world->world_size + new_entities->entitiesList->length;
    if (!entities_size) {
        Log(WARNING, "%s: There are no entities", __FUNCTION__);
        return 0.0;
    }

    const Entity* * entities = calloc(entities_size, sizeof(Entity*));
    if (!entities) {
        Log(ERROR, "%s: Failed to allocate entities", __FUNCTION__);
        return 0.0;
    }

    size_t next_index = 0;
    FOR_EACH_IN_SPECIES_LIST(&world->species) {
        FOR_EACH_IN_SPECIES(SPECIES_LIST_IT_P) {
            LOG_ASSERT(next_index < entities_size);
            entities[next_index++] = ENTITIES_IT_P;
        }
    }
    FOR_EACH_IN_SPECIES(new_entities) {
        LOG_ASSERT(next_index < entities_size);
        entities[next_index++] = ENTITIES_IT_P;
    }

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
    return min_distance + (max_distance - min_distance) * eps;
}
