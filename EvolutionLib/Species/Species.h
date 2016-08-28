//
// Created by Oleg on 6/1/16.
//

#ifndef EVOLUTION_SPECIES_H
#define EVOLUTION_SPECIES_H

#include <stdlib.h>

#include "List/List.h"
#include "Entity/Entity.h"

typedef struct species {
    List* entitiesList;
    size_t died;
    size_t initial_size;
} Species;

#define FOR_EACH_IN_SPECIES_N(SPECIES, IN_ENTITY_SP_IT) \
for (ListIterator IN_ENTITY_SP_IT = begin(((Species*)(SPECIES))->entitiesList); \
        !isIteratorAtEnd(IN_ENTITY_SP_IT); \
        next(&IN_ENTITY_SP_IT))

#define ENTITY_SP_IT_N(IN_ENTITY_SP_IT) ((Entity*)IN_ENTITY_SP_IT.current->value)

#define FOR_EACH_IN_SPECIES(SPECIES) FOR_EACH_IN_SPECIES_N((SPECIES), sp_in_it)

#define ENTITY_SP_IT ENTITY_SP_IT_N(sp_in_it)

#define SPECIES_LENGTH(SPECIES) (SPECIES ? ((Species*)SPECIES)->entitiesList->length : 0)

Species* CreateSpecies(size_t initial_size);

double GetMidFitness(Species* species);

void ClearSpecies(Species* species);

#endif //EVOLUTION_SPECIES_H
