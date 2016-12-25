//
// Created by Oleg on 12/24/16.
//

#ifndef EVOLUTION_GAOPERATORS_H
#define EVOLUTION_GAOPERATORS_H

#include "GeneticAlgorithm/GAFwd.h"
#include "Species/Species.h"

typedef struct ga_operators {
    int (*mutation)(World* world, size_t generation_number);
    Species* (*crossover)(World* world, size_t generation_number);
    SpeciesList* (*clustering)(World* world, Species* new_species);
    int (*children_selection)(World* world, Species* new_species);
    size_t (*selection)(World* world);
} GAOperators;

int GAO_UniformMutation(World* world, size_t generation_number);

int GAO_NonUniformMutation(World* world, size_t generation_number);

Species* GAO_Crossover(World* world, size_t generation_number);

SpeciesList* GAO_Clustering(World* world, Species* new_species);

int GAO_ChildrenSelection(World* world, Species* new_species);

size_t GAO_Selection(World* world);

#define DEFAULT_GA_OPERATORS { \
    .mutation = GAO_UniformMutation, \
    .crossover = GAO_Crossover, \
    .clustering = GAO_Clustering, \
    .children_selection = GAO_ChildrenSelection, \
    .selection = GAO_Selection \
}

#endif //EVOLUTION_GAOPERATORS_H
