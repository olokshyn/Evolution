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

typedef double(*Objective)(double*, int);

typedef struct world {
    List species;
    Limit* limits;
    Objective Ofunc;
    size_t world_size;
    size_t chr_size;
    double mutation_prob;
    size_t k;
    double h;
} World;

void CreateWorld(World* world,
                 size_t world_size,
                 size_t chromosome_size,
                 double mutation_probability,
                 double min_limit, 
                 double max_limit,
                 Objective Ofunc,
                 size_t k,
                 double h);

void ClearWorld(World* world);

void PerformClustering(World* world);

void PerformMutation(World* world);

void PerformCrossover(World* world);

double GetMaxFitness(World* world);

double Step(World* world);

int GetLastError();

int ResetLastError();

#endif
