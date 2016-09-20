#include "GeneticAlgorithm.h"

#include <math.h>

#include "Entity/Entity.h"
#include "AgglomerativeClustering/AgglomerativeClustering.h"
#include "Logging/Logging.h"
#include "DeathManager/DeathManager.h"

static int last_error = 0;

static void PerformSelectionInSpecies(World* world,
                                      Species* species,
                                      size_t alive_count);
static void PerformLimitedSelectionInSpecies(World* world,
                                             Species* species,
                                             double norm_fitness);
static List* CountFitnesses(List* species);
static void CountSpeciesLinks(World* world, List* fitness_list);
static void CountDiedSpecies(World* world);
static void SetError(int error_code);

void CreateWorld(World* world,
                 size_t world_size,
                 size_t chromosome_size,
                 double mutation_probability,
                 size_t k,
                 double h,
                 Objective objective) {
    LOG_FUNC_START("CreateWorld");

    if (!InitDeathManager("death.log")) {
        Log(ERROR, "Failed to init death manager");
        SetError(ERROR_INIT);
        return;
    }

    Species* new_species = NULL;
    Entity* new_entity = NULL;

    if (world == NULL) {
        SetError(ERROR_INVALID_POINTER);
        return;
    }
    initList(&world->species, NULL, (void (*)(void*))ClearSpecies);
    world->world_size = world_size;
    world->initial_world_size = world_size;
    if (objective.max_args_count > 0) {
        world->chr_size = chromosome_size <= objective.max_args_count
                            ? chromosome_size
                            : objective.max_args_count;
    }
    else {
        world->chr_size = chromosome_size;
    }
    world->mutation_prob = mutation_probability;
    world->k = k;
    world->h = h;
    world->obj = objective;

    new_species = CreateSpecies(world_size);

    for (size_t i = 0; i < world_size; ++i) {
        new_entity = (Entity*)malloc(sizeof(Entity));
        if (!new_entity) {
            goto error_CreateWorld;
        }
        new_entity->chr = (double*)malloc(sizeof(double)
                                          * world->chr_size);
        if (!new_entity->chr) {
            goto error_CreateWorld;
        }
        for (int j = 0; j < world->chr_size; ++j) {
            new_entity->chr[j] =
                getRand(world->obj.min, world->obj.max);
        }
        new_entity->fitness = world->obj.func(new_entity->chr,
                                              (int)world->chr_size);
        new_entity->old = 1;
        if (!pushBack(new_species->entitiesList, new_entity)) {
            goto error_CreateWorld;
        }
        new_entity = NULL;
    }
    if (!pushBack(&world->species, new_species)) {
        goto error_CreateWorld;
    }
    new_species = NULL;

    LOG_FUNC_END("CreateWorld");

    return;

error_CreateWorld:
    ReleaseDeathManager();
    ClearWorld(world);
    ClearSpecies(new_species);
    EntityDestructor(new_entity);
    SetError(ERROR_ALLOCATING_MEMORY);
}

void ClearWorld(World* world) {
    clearList(&world->species);
    ReleaseDeathManager();
}

void PerformMutation(World* world) {
    for (ListIterator speciesIt = begin(&world->species);
         !isIteratorExhausted(speciesIt);
         next(&speciesIt)) {
        FOR_EACH_IN_SPECIES(speciesIt.current->value) {
            if (doWithProbability(world->mutation_prob)) {
                size_t i = (size_t)selectRandom(0, (int)world->chr_size - 1);
                ENTITY_SP_IT->chr[i] = getRand(world->obj.min, world->obj.max);
                ENTITY_SP_IT->fitness =
                        world->obj.func(ENTITY_SP_IT->chr,
                                        (int)world->chr_size);
            }
        }
    }
}

Species* PerformCrossover(World* world) {
    LOG_FUNC_START("PerformCrossover");

    Species* new_species = NULL;
    List* fitness_list = NULL;
    Entity* new_entity1 = NULL;
    Entity* new_entity2 = NULL;
    short* crossed_parents = NULL;

    new_species = CreateSpecies(0);
    if (!new_species) {
        goto error_PerformCrossover;
    }

    fitness_list = CountFitnesses(&world->species);
    if (!fitness_list) {
        goto error_PerformCrossover;
    }

    for (ListIterator speciesIt = begin(&world->species),
                 fitnessIt = begin(fitness_list);
            !isIteratorExhausted(speciesIt);
            next(&speciesIt), next(&fitnessIt)) {
        if (SPECIES_LENGTH(speciesIt.current->value) == 1) {
            continue;
        }

        double crossover_prob = *(double*)fitnessIt.current->value;
        if (world->species.length == 1) {
            crossover_prob = CROSSOVER_FIRST_TIME_PROB;
        }
        else if (SPECIES_LENGTH(speciesIt.current->value)
                 <= CROSSOVER_EXTINCTION_BIAS) {
            crossover_prob = 1.0;
        }
        else {
            crossover_prob = MAX(MIN(CROSSOVER_ALPHA * crossover_prob,
                                     CROSSOVER_MAX_PROB),
                                 CROSSOVER_MIN_PROB);
        }

        crossed_parents = (short*)calloc(
                SPECIES_LENGTH(speciesIt.current->value),
                sizeof(short));
        if (!crossed_parents) {
            goto error_PerformCrossover;
        }
        size_t i = 0;
        for (ListIterator it1 = begin(speciesIt.current->value);
                !isIteratorExhausted(it1);
                next(&it1), ++i) {
            if (crossed_parents[i]) {
                continue;
            }
            size_t j = 0;
            for (ListIterator it2 = begin(speciesIt.current->value);
                    !isIteratorExhausted(it2) && !crossed_parents[i];
                    next(&it2), ++j) {
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
                CrossEntities(ENTITY_SP_IT_N(it1),
                              ENTITY_SP_IT_N(it2),
                              new_entity1,
                              new_entity2,
                              world->obj.func,
                              world->chr_size);
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
    }

    clearListPointer(fitness_list);

    Log(DEBUG, "New entities count: %d", (int)SPECIES_LENGTH(new_species));

    LOG_FUNC_END("PerformCrossover");

    return new_species;

error_PerformCrossover:
    ClearSpecies(new_species);
    clearListPointer(fitness_list);
    EntityDestructor(new_entity1);
    EntityDestructor(new_entity2);
    free(crossed_parents);
    SetError(ERROR_ALLOCATING_MEMORY);
    return NULL;
}

List* PerformClustering(World* world, Species* new_species) {
    List* clustered_species = AgglomerativeClustering(&world->species,
                                                      new_species->entitiesList,
                                                      world->chr_size,
                                                      world->h);
    if (!clustered_species) {
        goto error_PerformClustering;
    }
    return clustered_species;

error_PerformClustering:
    SetError(ERROR_ALLOCATING_MEMORY);
    return NULL;
}

void PerformChildrenSelection(World* world, Species* new_species) {
    PerformSelectionInSpecies(world, new_species, world->world_size);
}

size_t PerformSelection(World* world, List* clustered_species) {
    LOG_FUNC_START("PerformSelection");

    List* fitness_list = CountFitnesses(clustered_species);
    if (!fitness_list) {
        goto error_PerformSelection;
    }

    CountSpeciesLinks(world, fitness_list);

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

    clearListPointer(fitness_list);

    LOG_ASSERT(world->world_size != 0);

    LOG_FUNC_END("PerformSelection");

    return new_world_size;

error_PerformSelection:
    clearListPointer(fitness_list);
    SetError(ERROR_ALLOCATING_MEMORY);
    return 0;
}

double Iteration(World* world) {
    ResetLastError();

    IterationStart();

    Species* new_species = NULL;
    List* clustered_species = NULL;

    PerformMutation(world);
    if (GetLastError()) {
        goto error_Step;
    }

    new_species = PerformCrossover(world);
    if (GetLastError()) {
        goto error_Step;
    }

    PerformChildrenSelection(world, new_species);
    if (GetLastError()) {
        goto error_Step;
    }

    clustered_species = PerformClustering(world, new_species);
    if (GetLastError()) {
        goto error_Step;
    }
    ClearSpecies(new_species);
    new_species = NULL;

    size_t new_world_size = PerformSelection(world, clustered_species);
    if (GetLastError()) {
        clearListPointer(clustered_species);
        return 0.0;
    }

    emptyList(&world->species);
    moveList(&world->species, clustered_species);
    clearListPointer(clustered_species);
    clustered_species = NULL;
    world->world_size = new_world_size;

    CountDiedSpecies(world);

    IterationEnd();

    double max_fitness = GetMaxFitness(world);
    Log(INFO, "Iteration: max fitness: %.3f", max_fitness);
    LogMaxFitness(max_fitness);
    return max_fitness;

error_Step:
    ClearSpecies(new_species);
    clearListPointer(clustered_species);
    return 0.0;
}

double GetMaxFitness(World* world) {
    Entity* max_fitness_entity = NULL;
    for (ListIterator speciesIt = begin(&world->species);
            !isIteratorExhausted(speciesIt);
            next(&speciesIt)) {
        FOR_EACH_IN_SPECIES(speciesIt.current->value) {
            if (!max_fitness_entity
                    || ENTITY_SP_IT->fitness > max_fitness_entity->fitness) {
                max_fitness_entity = ENTITY_SP_IT;
            }
        }
    }
    return max_fitness_entity ? max_fitness_entity->fitness : 0.0;
}

int GetLastError() {
    return last_error;
}

int ResetLastError() {
    int temp = last_error;
    last_error = 0;
    return temp;
}

// Static methods section -----------

static void PerformSelectionInSpecies(World* world,
                                      Species* species,
                                      size_t alive_count) {
    if (alive_count >= SPECIES_LENGTH(species)) {
        return;
    }
    Entity** entities_p = NULL;
    List* sorted_new_entities = NULL;
    Entity* new_entity = NULL;

    entities_p = (Entity**)malloc(sizeof(Entity*) * SPECIES_LENGTH(species));
    if (!entities_p) {
        goto error_PerformSelectionInSpecies;
    }
    size_t index = 0;
    FOR_EACH_IN_SPECIES(species) {
        entities_p[index++] = ENTITY_SP_IT;
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

    clearListPointer(species->entitiesList);
    species->entitiesList = sorted_new_entities;
    sorted_new_entities = NULL;
    free(entities_p);
    entities_p = NULL;

    return;

error_PerformSelectionInSpecies:
    free(entities_p);
    clearListPointer(sorted_new_entities);
    EntityDestructor(new_entity);
    SetError(ERROR_ALLOCATING_MEMORY);
}

static void PerformLimitedSelectionInSpecies(World* world,
                                             Species* species,
                                             double norm_fitness) {
    double selection_part = MAX(norm_fitness, SELECTION_MIN);
    selection_part = MIN(selection_part, SELECTION_MAX);

    size_t species_size = (size_t)round(world->initial_world_size
                                        * selection_part);
    species_size = MIN(species_size, SPECIES_LENGTH(species));
    species_size = MAX(species_size, CROSSOVER_EXTINCTION_BIAS);
    PerformSelectionInSpecies(world, species, species_size);
}

static List* CountFitnesses(List* species) {
    List* fitness_list = NULL;
    double* temp = NULL;
    fitness_list = (List*)malloc(sizeof(List));
    if (!fitness_list) {
        goto error_CountFitnesses;
    }
    initList(fitness_list, NULL, free);
    for (ListIterator it = begin(species);
         !isIteratorExhausted(it);
         next(&it)) {
        temp = (double*)malloc(sizeof(double));
        if (!temp) {
            goto error_CountFitnesses;
        }
        *temp = GetMidFitness((Species*)it.current->value);
        if (!pushBack(fitness_list, temp)) {
            goto error_CountFitnesses;
        }
        temp = NULL;
    }

    Normalize(fitness_list);

    LOG_ASSERT(species->length == fitness_list->length);

    return fitness_list;

error_CountFitnesses:
    clearListPointer(fitness_list);
    free(temp);
    return NULL;
}

static void CountSpeciesLinks(World* world, List* fitness_list) {
    if (SPECIES_LINK_PROBABILITY == 0.0) {
        return;
    }

    for (ListIterator it1 = begin(fitness_list);
            !isIteratorExhausted(it1);
            next(&it1)) {
        for (ListIterator it2 = begin(fitness_list);
                !isIteratorExhausted(it2);
                next(&it2)) {
            if (it1.current == it2.current) {
                continue;
            }
            if (doWithProbability(SPECIES_LINK_PROBABILITY)) {
                *((double*)it1.current->value) +=
                        getRand(SPECIES_LINK_MIN, SPECIES_LINK_MAX)
                        *  *((double*)it2.current->value);
            }
        }
        *((double*)it1.current->value) = MAX(*((double*)it1.current->value),
                                             1.0);
    }

    Normalize(fitness_list);
}

static void CountDiedSpecies(World* world) {
    for (ListIterator clusterIt = begin(&world->species);
            !isIteratorExhausted(clusterIt);
            next(&clusterIt)) {
        Species* species = (Species*)clusterIt.current->value;
        LOG_ASSERT(species->initial_size != 0);
        if (species->died / (double)species->initial_size > EXTINCTION_BIAS) {
            species->died = 0;
            species->initial_size = SPECIES_LENGTH(species);
            MarkAllAsOld(species->entitiesList);
            RegisterDeath();
        }
    }
}

static void SetError(int error_code) {
    last_error = error_code;
    Log(ERROR, "Error set: %d", last_error);
}