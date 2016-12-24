//
// Created by Oleg on 12/24/16.
//

#ifndef EVOLUTION_WORLD_H
#define EVOLUTION_WORLD_H

#include <stddef.h>

#include "GAFwd.h"
#include "Species/Species.h"

typedef struct world {
    SpeciesList species;
    size_t world_size;
    size_t chr_size;
    const GAParameters* parameters;
    const GAOperators* operators;
} World;


World* CreateWorld(const GAParameters* parameters,
                   const GAOperators* operators);

void ClearWorld(World** world);

#endif //EVOLUTION_WORLD_H
