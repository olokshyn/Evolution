//
// Created by Oleg on 9/27/16.
//

#ifndef EVOLUTION_GALIB_H
#define EVOLUTION_GALIB_H

#include <stdbool.h>

#include "3rdParty/CList/list.h"

#include "GAFwd.h"
#include "Species/Species.h"

#ifndef LIST_DEFINED_DOUBLE
DEFINE_LIST(double)
#define LIST_DEFINED_DOUBLE
#endif

bool CountSpeciesLinks(LIST_TYPE(double) fitnesses);

double NonUniformMutationDelta(size_t t, double y,
                               const GAParameters* parameters);

bool LinearRankingSelection(World* world, Species* species, size_t alive_count);

#endif //EVOLUTION_GALIB_H
