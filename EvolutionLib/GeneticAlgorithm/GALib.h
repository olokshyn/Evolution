//
// Created by Oleg on 9/27/16.
//

#ifndef EVOLUTION_GALIB_H
#define EVOLUTION_GALIB_H

#include "GAFwd.h"
#include "List/List.h"
#include "Species/Species.h"

int CountSpeciesLinks(List* fitness_list);

double NonUniformMutationDelta(size_t t, double y,
                               const GAParameters* parameters);

int LinearRankingSelection(World* world, Species* species, size_t alive_count);

#endif //EVOLUTION_GALIB_H
