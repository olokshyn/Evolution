#include "GeneticAlgorithm.h"

static last_error = 0;

static int EntityComparator(const void* a, const void* b);
static void CrossEntities(Entity* parent1,
                          Entity* parent2,
                          Entity* child,
                          Objective Ofunc,
                          int chromosome_size);
static void SetError(int error_code);

void CreateWorld(World* world,
                 int world_size, 
                 double min_limit, 
                 double max_limit,
                 Objective Ofunc) {
    if (world == NULL) {
        SetError(ERROR_INVALID_POINTER);
        return;
    }
    world->entities = NULL;
    world->limits = NULL;
    world->Ofunc = Ofunc;
    world->world_size = world_size;
    world->chromosome_size = CHROMOSOME_SIZE;

    world->entities = (Entity*)malloc(sizeof(Entity) * world_size);
    if (world->entities == NULL) {
        SetError(ERROR_ALLOCATING_MEMORY);
        return;
    }

    world->limits = (Limit*)malloc(sizeof(Limit)* CHROMOSOME_SIZE);
    if (world->limits == NULL) {
        free(world->entities);
        world->entities = NULL;
        SetError(ERROR_ALLOCATING_MEMORY);
        return;
    }

    srand((unsigned int)time(NULL));
    for (int i = 0; i < CHROMOSOME_SIZE; i++) {
        world->limits[i].min = min_limit;
        world->limits[i].max = max_limit;
    }
    for (int i = 0; i < world_size; i++) {
        for (int j = 0; j < CHROMOSOME_SIZE; j++) {
            world->entities[i].chr[j] = 
                getRand(world->limits[j].min, world->limits[j].max);
        }
        world->entities[i].fitness = Ofunc(world->entities[i].chr,
                                           world->chromosome_size);
    }
}

void ClearWorld(World* world) {
    free(world->entities);
    free(world->limits);
}

void PerformMutation(World* world, double mutation_prob) {
    for (int i = 0; i < world->world_size; i++) {
        if (doWithProbability(mutation_prob)) {
            int j = selectRandom(0, world->chromosome_size - 1);
            world->entities[i].chr[j] = 
                getRand(world->limits[j].min, 
                        world->limits[j].max);
            world->entities[i].fitness = 
                world->Ofunc(world->entities[i].chr,
                             world->chromosome_size);
        }
    }
}

void PerformCrossover(World* world) {
    int new_world_size =
        world->world_size * (world->world_size - 1);
    Entity* new_entities = 
        (Entity*)malloc(sizeof(Entity) * new_world_size);
    if (new_entities == NULL) {
        SetError(ERROR_ALLOCATING_MEMORY);
        return;
    }

    int children_count = 0;
    for (int i = 0; i < world->world_size; i++) {
        for (int j = 0; j < world->world_size; j++) {
            if (i != j) {
                CrossEntities(world->entities + i,
                              world->entities + j,
                              new_entities + children_count++,
                              world->Ofunc,
                              world->chromosome_size);
            }
        }
    }
    Entity** entities_p = (Entity**)malloc(sizeof(Entity*)
                                           * new_world_size);
    if (entities_p == NULL) {
        free(new_entities);
        new_entities = NULL;
        SetError(ERROR_ALLOCATING_MEMORY);
        return;
    }
    for (int i = 0; i < new_world_size; i++) {
        entities_p[i] = new_entities + i;
    }
    qsort(entities_p,
          new_world_size,
          sizeof(Entity*),
          EntityComparator);
    for (int i = 0; i < world->world_size; i++) {
        world->entities[i] = *entities_p[i];
    }
    free(new_entities);
    free(entities_p);
}

double GetMaxFitness(World* world) {
    int max_i = 0;
    for (int i = 0; i < world->world_size; i++) {
        if (world->entities[i].fitness >
            world->entities[max_i].fitness) {
            max_i = i;
        }
    }
    return world->entities[max_i].fitness;
}

double Step(World* world, double mutation_prob) {
    PerformMutation(world, mutation_prob);
    PerformCrossover(world);
    if (GetLastError()) {
        return 0.0;
    }

    return world->entities[0].fitness;
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
    return (fabs(diff) < DOUBLE_EPS) ? 0 : (0 < diff) - (diff < 0);
}

static void CrossEntities(Entity* parent1,
                          Entity* parent2, 
                          Entity* child,
                          Objective Ofunc,
                          int chromosome_size) {
    int crossover_point = (chromosome_size % 2 == 0) ?
                            (chromosome_size / 2) :
                            (chromosome_size / 2 + 1);
    for (int i = 0; i < crossover_point; i++) {
        child->chr[i] = parent1->chr[i];
    }
    for (int i = crossover_point; i < chromosome_size; i++) {
        child->chr[i] = parent2->chr[i];
    }
    child->fitness = Ofunc(child->chr, chromosome_size);
}

static void SetError(int error_code) {
    last_error = error_code;
}