//
// Created by Oleg on 7/27/16.
//

#ifndef EVOLUTION_EVOLUTIONLIB_H
#define EVOLUTION_EVOLUTIONLIB_H

#include <stdio.h>

#include "GeneticAlgorithm/GeneticAlgorithm.h"
#include "Functions/TestFunctions.h"
#include "Logging/Logging.h"


int StartEvolution(size_t max_iterations_count,
                   size_t stable_value_iterations_count,
                   double stable_value_eps,
                   size_t individuals_count,
                   size_t chromosome_size,
                   double mutation_probability,
                   size_t k_neighbour,
                   double cluster_height,
                   Objective objective,
                   size_t* iterations_made,
                   double* best_fitness,
                   short silent);

#endif //EVOLUTION_EVOLUTIONLIB_H
