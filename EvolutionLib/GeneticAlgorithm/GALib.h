//
// Created by Oleg on 9/27/16.
//

#ifndef EVOLUTION_GALIB_H
#define EVOLUTION_GALIB_H

#include "GAFwd.h"
#include "List/List.h"

int CountSpeciesLinks(List* fitness_list);

double NonUniformMutationDelta(size_t t, double y,
                               const GAParameters* parameters);

#endif //EVOLUTION_GALIB_H
