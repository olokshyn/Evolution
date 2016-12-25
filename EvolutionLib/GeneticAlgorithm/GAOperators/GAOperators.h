//
// Created by Oleg on 12/24/16.
//

#ifndef EVOLUTION_GAOPERATORS_H
#define EVOLUTION_GAOPERATORS_H

#include "GeneticAlgorithm/GAFwd.h"
#include "Species/Species.h"

typedef struct ga_operators {
    int (*mutation)(World* world);
    Species* (*crossover)(World* world, size_t generation_number);
    SpeciesList* (*clustering)(World* world, Species* new_species);
    int (*children_selection)(World* world, Species* new_species);
    size_t (*selection)(World* world);
} GAOperators;

#endif //EVOLUTION_GAOPERATORS_H
