#ifndef GENETICALGORITHMHEADER
#define GENETICALGORITHMHEADER

#include <stdlib.h>

#include "Common.h"
#include "Species/Species.h"
#include "GAParameters.h"

#define ERROR_GENERIC -1
#define ERROR_ALLOCATING_MEMORY -2
#define ERROR_INVALID_POINTER -3
#define ERROR_INIT -4

typedef struct world {
    SpeciesList species;
    size_t world_size;
    size_t initial_world_size;
    size_t chr_size;
    double mutation_prob;
    size_t k;
    double h;
    Objective obj;
    size_t max_generations_count;
} World;

void CreateWorld(World* world,
                 size_t world_size,
                 size_t chromosome_size,
                 double mutation_probability,
                 size_t k,
                 double h,
                 Objective objective,
                 size_t max_generations_count);

void ClearWorld(World* world);

void PerformMutation(World* world);

Species* PerformCrossover(World* world, size_t generation_number);

SpeciesList* PerformClustering(World* world, Species* new_species);

void PerformChildrenSelection(World* world, Species* new_species);

size_t PerformSelection(World* world, SpeciesList* clustered_species);

double Iteration(World* world, size_t generation_number);

double GetMaxFitness(World* world);

int GetLastError();

int ResetLastError();

#endif
