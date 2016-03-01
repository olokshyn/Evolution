#ifndef GENETICALGORITHMHEADER
#define GENETICALGORITHMHEADER

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../List/List.h"

#include "Lib.h"


#define DOUBLE_EPS 0.0001
#define CHROMOSOME_SIZE 2

#define ERROR_ALLOCATING_MEMORY -1
#define ERROR_INVALID_POINTER -2

typedef struct entity {
    double* chr;
    double fitness;
} Entity;

typedef struct limit {
    double min;
    double max;
} Limit;

typedef double(*ObjectiveFunc)(double*, int);

typedef struct objective {
    ObjectiveFunc func;
    double min;
    double max;
    size_t max_args_count;  // 0 if no limit on args count
    double optimum;  // not used
    size_t best_k;  // not used
    double best_h;  // not used
} Objective;

typedef struct world {
    List species;
    size_t world_size;
    size_t chr_size;
    double mutation_prob;
    size_t k;
    double h;
    Objective obj;
} World;

void CreateWorld(World* world,
                 size_t world_size,
                 size_t chromosome_size,
                 double mutation_probability,
                 size_t k,
                 double h,
                 Objective objective);

void ClearWorld(World* world);

void PerformClustering(World* world);

void PerformMutation(World* world);

void PerformCrossover(World* world);

double GetMaxFitness(World* world);

double Step(World* world);

int GetLastError();

int ResetLastError();

#endif
