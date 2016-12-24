//
// Created by Oleg on 12/24/16.
//

#include "GAOperators.h"

#include <math.h>

#include "World.h"
#include "GAParameters.h"
#include "Entity/Crossovers.h"
#include "AgglomerativeClustering/AgglomerativeClustering.h"

#include "GALib.h"
#include "Logging/Logging.h"
#include "Common.h"

static int PerformSelectionInSpecies(World* world,
                                     Species* species,
                                     size_t alive_count);

static int PerformLimitedSelectionInSpecies(World* world,
                                            Species* species,
                                             double norm_fitness);

int GAO_DefaultMutation(World* world) {
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

Species* GAO_DefaultCrossover(World* world, size_t generation_number) {
    LOG_FUNC_START("GAO_DefaultCrossover");

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

    LOG_FUNC_END("GAO_DefaultCrossover");

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

SpeciesList* GAO_DefaultClustering(World* world, Species* new_species) {
    SpeciesList* clustered_species =
            AgglomerativeClustering(&world->species,
                                    new_species->entitiesList,
                                    world->chr_size,
                                    world->parameters->h);
    if (!clustered_species) {
        goto error_PerformClustering;
    }
    return clustered_species;

error_PerformClustering:
    return NULL;
}

int GAO_DefaultChildrenSelection(World* world, Species* new_species) {
    return PerformSelectionInSpecies(world, new_species, world->world_size);
}

size_t GAO_DefaultSelection(World* world) {
    LOG_FUNC_START("GAO_DefaultSelection");

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

    LOG_FUNC_END("GAO_DefaultSelection");

    return new_world_size;

error_PerformSelection:
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
          EntityComparator);

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