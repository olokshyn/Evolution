//
// Created by Oleg on 12/25/16.
//

#ifndef EVOLUTION_GADEFAULTOPERATORS_H_H
#define EVOLUTION_GADEFAULTOPERATORS_H_H

#include "GAOperators.h"

int GAO_DefaultMutation(World* world);

Species* GAO_DefaultCrossover(World* world, size_t generation_number);

SpeciesList* GAO_DefaultClustering(World* world, Species* new_species);

int GAO_DefaultChildrenSelection(World* world, Species* new_species);

size_t GAO_DefaultSelection(World* world);

#define DEFAULT_GA_OPERATORS { \
    .mutation = GAO_DefaultMutation, \
    .crossover = GAO_DefaultCrossover, \
    .clustering = GAO_DefaultClustering, \
    .children_selection = GAO_DefaultChildrenSelection, \
    .selection = GAO_DefaultSelection \
}

#endif //EVOLUTION_GADEFAULTOPERATORS_H_H
