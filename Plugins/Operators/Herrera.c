//
// Created by oleg on 16.12.17.
//

#include "GeneticAlgorithm/GAOperators.h"

const GAOperators operators = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .selection = GAO_LinearRankingSelection,
        .iteration_info_hook = GAO_ConvergenceStopIterationHook,
        .name = "Herrera"
};
