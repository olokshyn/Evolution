//
// Created by Oleg on 12/24/16.
//

#ifndef EVOLUTION_WORLD_H
#define EVOLUTION_WORLD_H

#include <stddef.h>

#include "GAFwd.h"
#include "Species/Species.h"

typedef struct world {
    LIST_TYPE(SpeciesPtr) population;
    size_t size;
    size_t chr_size;
    GAParameters* parameters;
    GAOperators* operators;
    Journal* journal;
} World;


World* CreateWorld(const GAParameters* parameters,
                   const GAOperators* operators,
                   const Journal* journal);

void DestroyWorld(World* world);

#endif //EVOLUTION_WORLD_H
