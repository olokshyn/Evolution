//
// Created by oleg on 06.01.18.
//

#include "GeneticAlgorithm/GAOperators.h"
#include "GeneticAlgorithm/OperatorsSwitch.h"

GAOperators operators = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .selection = GAO_LinearRankingSelection,
        .iteration_info_hook = GAO_SwitchIterationInfoHook,
        .name = "Herrera_s_Clustering_s_RandomLinks"
};

const GAOperators clustering = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .clustering = GAO_Clustering,
        .selection = GAO_ElitistsSelection,
};

const GAOperators random_links = {
        .mutation = GAO_NonUniformMutation,
        .crossover = GAO_UniformCrossover,
        .clustering = GAO_Clustering,
        .selection = GAO_SpeciesRandomLinksSelection,
};


REGISTER_SWITCH_TO_OPERATORS(&clustering, &random_links)
#define STOP_ON_CONVERGENCE false

#include "GeneticAlgorithm/OperatorsSwitchDef.h"
