//
// Created by oleg on 07.01.18.
//

#include "GeneticAlgorithm/GAOperators.h"
#include "GeneticAlgorithm/Lib/Operators.h"

const GAOperators operators = {
        .mutation = Mutation_NonUniform,
        .crossover = Crossover_DHX,
        .clustering = Clustering_OPTICS,
        .selection = Selection_Linear_SpeciesSizePenalty,
        .iteration_info_hook = GAO_ConvergenceStopIterationHook,
        .name = "Penalty Clustering"
};
