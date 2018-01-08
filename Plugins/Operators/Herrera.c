//
// Created by oleg on 16.12.17.
//

#include "GeneticAlgorithm/GAOperators.h"
#include "GeneticAlgorithm/Lib/Operators.h"

const GAOperators operators = {
        .mutation = Mutation_NonUniform,
        .crossover = Crossover_DHX,
        .selection = Selection_LinearRanking,
        .iteration_info_hook = GAO_ConvergenceStopIterationHook,
        .name = "Herrera"
};
