#include <stdio.h>

#include "GeneticAlgorithm.h"
#include "TestFunctions.h"

#define EPS 0.001

int RunSimulation(size_t max_iterations_count,
                  size_t stable_value_iterations_count,
                  double stable_value_eps,
                  size_t individuals_count,
                  size_t chromosome_size,
                  double mutation_probability,
                  size_t k_neighbour,
                  double cluster_height,
                  Objective objective,
                  size_t* iterations_made,
                  short silent);