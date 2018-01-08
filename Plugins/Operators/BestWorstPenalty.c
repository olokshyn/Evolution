//
// Created by oleg on 08.01.18.
//

#include "GeneticAlgorithm/GAOperators.h"
#include "GeneticAlgorithm/Lib/Operators.h"
#include "GeneticAlgorithm/OperatorsSwitch.h"

const GAOperators operators = {
        .mutation = Mutation_NonUniform,
        .crossover = Crossover_DHX,
        .clustering = Clustering_OPTICS,
        .selection = Selection_Linear,
        .iteration_info_hook = GAO_SwitchIterationInfoHook,
        .name = "Best Worst Penalty"
};

const GAOperators clustering_best_worst_penalty = {
        .mutation = Mutation_NonUniform,
        .crossover = Crossover_DHX,
        .clustering = Clustering_OPTICS,
        .selection = Selection_Linear_BestWorstPenalty,
};

REGISTER_SWITCH_TO_OPERATORS(&clustering_best_worst_penalty)
#define STOP_ON_CONVERGENCE false

#include "GeneticAlgorithm/OperatorsSwitchDef.h"
