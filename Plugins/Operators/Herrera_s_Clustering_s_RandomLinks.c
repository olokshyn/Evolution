//
// Created by oleg on 06.01.18.
//

#include "GeneticAlgorithm/GAOperators.h"
#include "GeneticAlgorithm/Lib/Operators.h"
#include "GeneticAlgorithm/OperatorsSwitch.h"

GAOperators operators = {
        .mutation = Mutation_NonUniform,
        .crossover = Crossover_DHX,
        .selection = Selection_LinearRanking,
        .iteration_info_hook = GAO_SwitchIterationInfoHook,
        .name = "Herrera_s_Clustering_s_RandomLinks"
};

const GAOperators clustering = {
        .mutation = Mutation_NonUniform,
        .crossover = Crossover_DHX,
        .clustering = Clustering_OPTICS,
        .selection = Selection_Linear,
};

const GAOperators random_links = {
        .mutation = Mutation_NonUniform,
        .crossover = Crossover_DHX,
        .clustering = Clustering_OPTICS,
        .selection = Selection_Linear_RandomSpeciesLinks,
};


REGISTER_SWITCH_TO_OPERATORS(&clustering, &random_links)
#define STOP_ON_CONVERGENCE false

#include "GeneticAlgorithm/OperatorsSwitchDef.h"
