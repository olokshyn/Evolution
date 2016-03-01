#include <stdio.h>

#include "GeneticAlgorithm.h"
#include "TestFunctions.h"

int RunSimulation(size_t iterations_count,
                  size_t individuals_count,
                  size_t chromosome_size,
                  double mutation_probability,
                  double min_limit,
                  double max_limit,
                  Objective Ofunc,
                  size_t k_neighbour,
                  double cluster_height);