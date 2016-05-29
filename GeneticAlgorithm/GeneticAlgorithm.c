#include "GeneticAlgorithm.h"
#include "../AgglomerativeClustering/AgglomerativeClustering.h"
#include "../Logging/Logging.h"

static int last_error = 0;

static void SetError(int error_code);
static void PerformMutation(World* world);
static List* PerformCrossover(World* world);
static double GetMidFitness(List* species);
static void PerformSelectionInSpecies(World* world,
                                      List** species,
                                      double norm_fitness);
static void PerformSelection(World* world,
                             List* clustered_species);
static void CrossEntities(Entity* parent1,
                          Entity* parent2,
                          Entity* child,
                          ObjectiveFunc Ofunc,
                          size_t chromosome_size);

void CreateWorld(World* world,
                 size_t world_size,
                 size_t chromosome_size,
                 double mutation_probability,
                 size_t k,
                 double h,
                 Objective objective) {
    LOG_FUNC_START("CreateWorld");

    List* new_entities = NULL;
    Entity* new_entity = NULL;

    if (world == NULL) {
        SetError(ERROR_INVALID_POINTER);
        return;
    }
    initList(&world->species, NULL, (void (*)(void*))clearListPointer);
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

    new_entities = CreateEntitiesList();

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
        if (!pushBack(new_entities, new_entity)) {
            goto error_CreateWorld;
        }
        new_entity = NULL;
    }
    if (!pushBack(&world->species, new_entities)) {
        goto error_CreateWorld;
    }
    new_entities = NULL;

    LOG_FUNC_END("CreateWorld");

    return;

error_CreateWorld:
    ClearWorld(world);
    clearListPointer(new_entities);
    EntityDestructor(new_entity);
    SetError(ERROR_ALLOCATING_MEMORY);
}

void ClearWorld(World* world) {
    clearList(&world->species);
}

double Step(World* world) {
    ResetLastError();

    PerformMutation(world);
    if (GetLastError()) {
        return 0.0;
    }

    List* clustered_species = PerformCrossover(world);
    if (GetLastError()) {
        clearListPointer(clustered_species);
        return 0.0;
    }

    PerformSelection(world, clustered_species);
    if (GetLastError()) {
        clearListPointer(clustered_species);
        return 0.0;
    }

    double max_fitness = GetMaxFitness(world);
    Log(INFO, "Step: max fitness: %.3f", max_fitness);
    return max_fitness;
}

double GetMaxFitness(World* world) {
    Entity* max_fitness_entity = NULL;
    for (ListIterator speciesIt = begin(&world->species);
            !isIteratorAtEnd(speciesIt);
            next(&speciesIt)) {
        for (ListIterator entityIt = begin(speciesIt.current->value);
                !isIteratorAtEnd(entityIt);
                next(&entityIt)) {
            if (!max_fitness_entity
                || ((Entity*)entityIt.current->value)->fitness
                   > max_fitness_entity->fitness) {
                max_fitness_entity = (Entity*)entityIt.current->value;
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

static void PerformMutation(World* world) {
    for (ListIterator speciesIt = begin(&world->species);
            !isIteratorAtEnd(speciesIt);
            next(&speciesIt)) {
        for (ListIterator entityIt = begin(speciesIt.current->value);
                !isIteratorAtEnd(entityIt);
                next(&entityIt)) {
            if (doWithProbability(world->mutation_prob)) {
                size_t i = (size_t)selectRandom(0, (int)world->chr_size - 1);
                ((Entity*)entityIt.current->value)->chr[i] =
                        getRand(world->obj.min, world->obj.max);
                ((Entity*)entityIt.current->value)->fitness =
                        world->obj.func(((Entity*)entityIt.current->value)->chr,
                                        (int)world->chr_size);
            }
        }
    }
}

static List* PerformCrossover(World* world) {
    LOG_FUNC_START("PerformCrossover");

    List* new_entities = NULL;
    Entity* new_entity = NULL;

    new_entities = CreateEntitiesList();
    if (!new_entities) {
        goto error_PerformCrossover;
    }

    Log(INFO, "Before crossover");
    for (ListIterator speciesIt = begin(&world->species);
            !isIteratorAtEnd(speciesIt);
            next(&speciesIt)) {
        List* speciesList = (List*)speciesIt.current->value;
        if (speciesList->length == 1) {
            continue;
        }

        for (size_t i = 0; i < speciesList->length; ++i) {
            for (size_t j = 0; j < speciesList->length; ++j) {
                if (i == j) {
                    continue;
                }
                new_entity = (Entity*)malloc(sizeof(Entity));
                if (!new_entity) {
                    goto error_PerformCrossover;
                }
                new_entity->chr = (double*)malloc(sizeof(double)
                                                  * world->chr_size);
                if (!new_entity->chr) {
                    goto error_PerformCrossover;
                }
                CrossEntities((Entity*)findByIndex(speciesList, i).current->value,
                              (Entity*)findByIndex(speciesList, j).current->value,
                              new_entity,
                              world->obj.func,
                              world->chr_size);
                if (!pushBack(new_entities, new_entity)) {
                    goto error_PerformCrossover;
                }
                new_entity = NULL;
            }
        }
    }
    Log(INFO, "After crossover");

    List* clustered_species = AgglomerativeClustering(&world->species,
                                                      new_entities,
                                                      world->chr_size,
                                                      world->h);
    if (!clustered_species) {
        goto error_PerformCrossover;
    }
    clearListPointer(new_entities);

    LOG_FUNC_END("PerformCrossover");

    return clustered_species;

error_PerformCrossover:
    clearListPointer(new_entities);
    EntityDestructor(new_entity);
    SetError(ERROR_ALLOCATING_MEMORY);
    return NULL;
}

static double GetMidFitness(List* species) {
    double fitness = 0.0;
    for (ListIterator it = begin(species);
            !isIteratorAtEnd(it);
            next(&it)) {
        fitness += ((Entity*)it.current->value)->fitness;
    }
    return fitness / species->length;
}

static void PerformSelectionInSpecies(World* world,
                                      List** species,
                                      double norm_fitness) {
    LOG_FUNC_START("PerformSelectionInSpecies");

    List* entities = *species;
    Entity** entities_p = NULL;
    List* sorted_new_entities = NULL;
    Entity* new_entity = NULL;

    double selection_part = MAX(norm_fitness, SELECTION_MIN);
    selection_part = MIN(selection_part, SELECTION_MAX);

    size_t species_size = (size_t)round(world->initial_world_size
                                        * selection_part);
    species_size = MIN(species_size, entities->length);

    entities_p = (Entity**)malloc(sizeof(Entity*) * entities->length);
    if (!entities_p) {
        goto error_PerformSelectionInSpecies;
    }
    size_t index = 0;
    for (ListIterator entityIt = begin(entities);
            !isIteratorAtEnd(entityIt);
            next(&entityIt), ++index) {
        entities_p[index] = (Entity*)entityIt.current->value;
    }
    qsort(entities_p,
          entities->length,
          sizeof(Entity*),
          EntityComparator);

    sorted_new_entities = CreateEntitiesList();
    if (!sorted_new_entities) {
        goto error_PerformSelectionInSpecies;
    }
    for (size_t i = 0; i < species_size; ++i) {
        new_entity = CopyEntity(entities_p[i], world->chr_size);
        if (!new_entity) {
            goto error_PerformSelectionInSpecies;
        }
        if (!pushBack(sorted_new_entities, new_entity)) {
            goto error_PerformSelectionInSpecies;
        }
        new_entity = NULL;
    }
    clearListPointer(*species);
    *species = sorted_new_entities;
    sorted_new_entities = NULL;
    free(entities_p);
    entities_p = NULL;

    LOG_FUNC_END("PerformSelectionInSpecies");

    return;

error_PerformSelectionInSpecies:
    free(entities_p);
    clearListPointer(sorted_new_entities);
    EntityDestructor(new_entity);
    SetError(ERROR_ALLOCATING_MEMORY);
}

static void PerformSelection(World* world, List* clustered_species) {
    LOG_FUNC_START("PerformSelection");

    List fitness_list;
    double* temp = NULL;
    initList(&fitness_list, NULL, free);
    for (ListIterator it = begin(clustered_species);
            !isIteratorAtEnd(it);
            next(&it)) {
        temp = (double*)malloc(sizeof(double));
        if (!temp) {
            goto error_PerformSelection;
        }
        *temp = GetMidFitness((List*)it.current->value);
        if (!pushBack(&fitness_list, temp)) {
            goto error_PerformSelection;
        }
        temp = NULL;
    }

    Normalize(&fitness_list);

    LOG_ASSERT(clustered_species->length == fitness_list.length);
    size_t new_world_size = 0;
    for (ListIterator it_cl = begin(clustered_species),
                 it_ft = begin(&fitness_list);
            !isIteratorAtEnd(it_cl) && !isIteratorAtEnd(it_ft);
            next(&it_cl), next(&it_ft)) {
        PerformSelectionInSpecies(world,
                                  (List**)(&it_cl.current->value),
                                  *((double*)it_ft.current->value));
        new_world_size += ((List*)it_cl.current->value)->length;
    }

    Log(DEBUG, "New world size: %d", (int)new_world_size);

    for (ListIterator it = begin(clustered_species);
                !isIteratorAtEnd(it);
                ) {
        if (((List*)it.current->value)->length == 0) {
            ListIterator temp_it = it;
            next(&it);
            removeFromList(temp_it);
        }
        else {
            next(&it);
        }
    }

    clearList(&fitness_list);
    clearList(&world->species);
    world->species = *clustered_species;
    free(clustered_species);
    world->world_size = new_world_size;

    LOG_ASSERT(world->world_size != 0);

    LOG_FUNC_END("PerformSelection");

    return;

error_PerformSelection:
    clearList(&fitness_list);
    free(temp);
    SetError(ERROR_ALLOCATING_MEMORY);
    return;
}

static void CrossEntities(Entity* parent1,
                          Entity* parent2, 
                          Entity* child,
                          ObjectiveFunc Ofunc,
                          size_t chromosome_size) {
    size_t crossover_point = (chromosome_size % 2 == 0) ?
                                (chromosome_size / 2) :
                                (chromosome_size / 2 + 1);
    for (size_t i = 0; i < crossover_point; ++i) {
        child->chr[i] = parent1->chr[i];
    }
    for (size_t i = crossover_point; i < chromosome_size; ++i) {
        child->chr[i] = parent2->chr[i];
    }
    child->fitness = Ofunc(child->chr, (int)chromosome_size);
}

static void SetError(int error_code) {
    last_error = error_code;
    Log(ERROR, "Error set: %d", last_error);
}