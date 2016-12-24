//
// Created by Oleg on 12/24/16.
//

#ifndef EVOLUTION_GAOPERATORS_H
#define EVOLUTION_GAOPERATORS_H

#include "GAFwd.h"
#include "Species/Species.h"

typedef struct ga_operators {
    int (*mutation)(World* world);
    Species* (*crossover)(World* world, size_t generation_number);
    SpeciesList* (*clustering)(World* world, Species* new_species);
    int (*children_selection)(World* world, Species* new_species);
    size_t (*selection)(World* world);
} GAOperators;

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

#endif //EVOLUTION_GAOPERATORS_H
