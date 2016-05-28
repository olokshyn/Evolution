#ifndef GENETICALGORITHMHEADER
#define GENETICALGORITHMHEADER

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../List/List.h"

#include "Lib.h"
#include "Entity.h"

#define ENABLE_THREADS 0
#if defined(ENABLE_THREADS) && ENABLE_THREADS
    #define THREADS_COUNT 4
#endif

#define ENABLE_CLUSTERING 1

#define COLORED_OUTPUT 0

#define DOUBLE_EPS 0.0001

#define SELECTION_MIN 0.1
#define SELECTION_MAX 1.0

#define ERROR_GENERIC -1
#define ERROR_ALLOCATING_MEMORY -2
#define ERROR_INVALID_POINTER -3

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
    size_t initial_world_size;
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

double Step(World* world);

double GetMaxFitness(World* world);

int GetLastError();

int ResetLastError();

#endif
