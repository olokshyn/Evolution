//
// Created by Oleg on 6/1/16.
//

#ifndef EVOLUTION_SPECIES_H
#define EVOLUTION_SPECIES_H

#include <stdlib.h>

#include "3rdParty/CList/list.h"

#include "Entity/Entity.h"

typedef struct species {
    LIST_TYPE(EntityPtr) entities;
    size_t died;
    size_t initial_size;
} Species, *SpeciesPtr;

#ifndef LIST_DEFINED_SPECIESPTR
DEFINE_LIST(SpeciesPtr)
#define LIST_DEFINED_SPECIESPTR
#endif

#ifndef LIST_DEFINED_DOUBLE
DEFINE_LIST(double)
#define LIST_DEFINED_DOUBLE
#endif

Species* CreateSpecies(size_t initial_size);

void DestroySpecies(Species* species);

bool ClearPopulation(LIST_TYPE(SpeciesPtr) population);

void DestroyPopulation(LIST_TYPE(SpeciesPtr) population);

LIST_TYPE(double) NormalizePopulationFitnesses(
        LIST_TYPE(SpeciesPtr) population);

#endif //EVOLUTION_SPECIES_H
