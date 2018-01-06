//
// Created by oleg on 16.12.17.
//

#include "GeneticAlgorithm/GAOperators.h"

const GAOperators operators = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .clustering = GAO_Clustering,
        .selection = GAO_ElitistsSelection,
        .iteration_info_hook = GAO_ConvergenceStopIterationHook,
        .name = "Herrera With Clustering"
};
