//
// Created by oleg on 07.01.18.
//

#include "GeneticAlgorithm/GAOperators.h"

const GAOperators operators = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .clustering = GAO_Clustering,
        .selection = GAO_SpeciesSizePenaltySelection,
        .iteration_info_hook = GAO_ConvergenceStopIterationHook,
        .name = "Penalty Clustering"
};
