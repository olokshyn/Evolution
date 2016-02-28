#ifndef GENETICALGORITHMHEADER
#define GENETICALGORITHMHEADER

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Lib.h"


#define DOUBLE_EPS 0.0001
#define CHROMOSOME_SIZE 2

#define ERROR_ALLOCATING_MEMORY -1
#define ERROR_INVALID_POINTER -2

typedef struct entity {
    double chr[CHROMOSOME_SIZE];
    double fitness;
} Entity;

typedef struct limit {
    double min;
    double max;
} Limit;

typedef double(*Objective)(double*, int);

typedef struct world {
    Entity* entities;
    Limit* limits;
    Objective Ofunc;
    int world_size;
    int chromosome_size;
} World;

void CreateWorld(World* world,
                 int world_size, 
                 double min_limit, 
                 double max_limit,
                 Objective Ofunc);

void ClearWorld(World* world);

void PerformMutation(World* world,
                     double mutation_prob);
void PerformCrossover(World* world);

double GetMaxFitness(World* world);

double Step(World* world, 
            double mutation_prob);

int GetLastError();

int ResetLastError();

#endif
