#include "GeneticAlgorithm.h"
#include "../Wishart/Wishart.h"
#include "../AgglomerativeClustering/AgglomerativeClustering.h"

#define CLUSTERING_ALGORITHM WishartWrapped

static int last_error = 0;

static int EntityComparator(const void* a, const void* b);
static void EntityDestructor(Entity* entity);
static Entity* CopyEntity(Entity* entity, size_t chr_size);
static List* CreateEntitiesList();
static void CrossEntities(Entity* parent1,
                          Entity* parent2,
                          Entity* child,
                          ObjectiveFunc Ofunc,
                          size_t chromosome_size);
static void SetError(int error_code);

void CreateWorld(World* world,
                 size_t world_size,
                 size_t chromosome_size,
                 double mutation_probability,
                 size_t k,
                 double h,
                 Objective objective) {
    List* new_entities = NULL;
    Entity* new_entity = NULL;

    if (world == NULL) {
        SetError(ERROR_INVALID_POINTER);
        return;
    }
    initList(&world->species, NULL, (void (*)(void*))clearListPointer);
    world->world_size = world_size;
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

//    srand((unsigned int)time(NULL));
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

void PerformClustering(World* world) {
    double** vectors = NULL;
    List* clustered_species = NULL;
    List* new_entities = NULL;
    Entity* new_entity = NULL;

    vectors = (double**)malloc(sizeof(double*) * world->world_size);
    if (!vectors) {
        goto error_PerformClustering;
    }
    for (size_t i = 0; i < world->world_size; ++i) {
        vectors[i] = (double*)malloc(sizeof(double) * world->chr_size);
        if (!vectors[i]) {
            goto error_PerformClustering;
        }
    }

    size_t index = 0;
    for (ListIterator speciesIt = begin(&world->species);
            !isIteratorAtEnd(speciesIt);
            next(&speciesIt)) {
        for (ListIterator entityIt = begin(speciesIt.current->value);
                !isIteratorAtEnd(entityIt);
                next(&entityIt), ++index) {
            for (size_t i = 0; i < world->chr_size; ++i) {
                vectors[index][i] =
                        ((Entity*)entityIt.current->value)->chr[i];
            }
        }
    }

    // TODO: add memory allocation checks
    clustered_species = CLUSTERING_ALGORITHM((const double* const*)vectors,
                                             world->world_size,
                                             world->chr_size,
                                             world->k,
                                             world->h);
    if (!clustered_species) {
        goto error_PerformClustering;
    }

    clearList(&world->species);
    initList(&world->species, NULL, (void (*)(void*))clearListPointer);
    for (ListIterator clusterIt = begin(clustered_species);
            !isIteratorAtEnd(clusterIt);
            next(&clusterIt)) {
        new_entities = CreateEntitiesList();
        if (!new_entities) {
            goto error_PerformClustering;
        }
        for (ListIterator vectorIt = begin(clusterIt.current->value);
                !isIteratorAtEnd(vectorIt);
                next(&vectorIt)) {
            new_entity = (Entity*)malloc(sizeof(Entity));
            if (!new_entity) {
                goto error_PerformClustering;
            }
            new_entity->chr = (double*)malloc(sizeof(double) * world->chr_size);
            if (!new_entity->chr) {
                goto error_PerformClustering;
            }
            for (size_t i = 0; i < world->chr_size; ++i) {
                new_entity->chr[i] = ((double*)vectorIt.current->value)[i];
            }
            new_entity->fitness = world->obj.func(new_entity->chr,
                                                  (int)world->chr_size);
            if (!pushBack(new_entities, new_entity)) {
                goto error_PerformClustering;
            }
            new_entity = NULL;
        }
        if (!pushBack(&world->species, new_entities)) {
            goto error_PerformClustering;
        }
        new_entities = NULL;
    }

    for (size_t i = 0; i < world->world_size; ++i) {
        free(vectors[i]);
    }
    free(vectors);
    clearListPointer(clustered_species);

    return;

error_PerformClustering:
    for (size_t i = 0;
            vectors && i < world->world_size && vectors[i];
            ++i) {
        free(vectors[i]);
    }
    free(vectors);
    clearListPointer(clustered_species);
    clearListPointer(new_entities);
    EntityDestructor(new_entity);
    SetError(ERROR_ALLOCATING_MEMORY);
}

void PerformMutation(World* world) {
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

void PerformCrossover(World* world) {
    List* new_entities = NULL;
    List* sorted_new_entities = NULL;
    Entity* new_entity = NULL;
    Entity** entities_p = NULL;

    for (ListIterator speciesIt = begin(&world->species);
            !isIteratorAtEnd(speciesIt);
            next(&speciesIt)) {
        List* speciesList = (List*)speciesIt.current->value;
        if (speciesList->length == 1) {
            continue;
        }
        new_entities = CreateEntitiesList();
        if (!new_entities) {
            goto error_PerformCrossover;
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

        entities_p = (Entity**)malloc(sizeof(Entity*) * new_entities->length);
        if (!entities_p) {
            goto error_PerformCrossover;
        }
        size_t index = 0;
        for (ListIterator entityIt = begin(new_entities);
                !isIteratorAtEnd(entityIt);
                next(&entityIt), ++index) {
            entities_p[index] = (Entity*)entityIt.current->value;
        }
        qsort(entities_p,
              new_entities->length,
              sizeof(Entity*),
              EntityComparator);

        sorted_new_entities = CreateEntitiesList();
        if (!sorted_new_entities) {
            goto error_PerformCrossover;
        }
        for (size_t i = 0; i < speciesList->length; ++i) {
            new_entity = CopyEntity(entities_p[i], world->chr_size);
            if (!new_entity) {
                goto error_PerformCrossover;
            }
            if (!pushBack(sorted_new_entities, new_entity)) {
                goto error_PerformCrossover;
            }
            new_entity = NULL;
        }
        clearListPointer(new_entities);
        new_entities = NULL;
        clearListPointer(speciesIt.current->value);
        speciesIt.current->value = sorted_new_entities;
        sorted_new_entities = NULL;
        free(entities_p);
        entities_p = NULL;
    }

    return;

error_PerformCrossover:
    clearListPointer(new_entities);
    clearListPointer(sorted_new_entities);
    EntityDestructor(new_entity);
    free(entities_p);
    SetError(ERROR_ALLOCATING_MEMORY);
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

double Step(World* world) {
    ResetLastError();
#if defined(ENABLE_CLUSTERING) && ENABLE_CLUSTERING
    PerformClustering(world);
    if (GetLastError()) {
        return 0.0;
    }
#endif
    PerformMutation(world);
    if (GetLastError()) {
        return 0.0;
    }
    PerformCrossover(world);
    if (GetLastError()) {
        return 0.0;
    }

    return GetMaxFitness(world);
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

static int EntityComparator(const void* a, const void* b) {
    double diff = (*(Entity**)b)->fitness 
                - (*(Entity**)a)->fitness;
    return (fabs(diff) < DOUBLE_EPS) ? 0 : (diff > 0) - (diff < 0);
}

static void EntityDestructor(Entity* entity) {
    if (entity) {
        free(entity->chr);
        free(entity);
    }
}

static Entity* CopyEntity(Entity* entity, size_t chr_size) {
    Entity* new_entity = (Entity*)malloc(sizeof(Entity));
    if (!new_entity) {
        return NULL;
    }
    new_entity->chr = (double*)malloc(sizeof(double) * chr_size);
    if (!new_entity->chr) {
        free(new_entity);
        return NULL;
    }
    for (size_t i = 0; i < chr_size; ++i) {
        new_entity->chr[i] = entity->chr[i];
    }
    new_entity->fitness = entity->fitness;
    return new_entity;
}

static List* CreateEntitiesList() {
    List* entities = (List*)malloc(sizeof(List));
    if (!entities) {
        return NULL;
    }
    initList(entities, EntityComparator, (void (*)(void*))EntityDestructor);
    return entities;
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
}