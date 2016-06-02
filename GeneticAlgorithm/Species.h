//
// Created by Oleg on 6/1/16.
//

#ifndef WISHART_SPECIES_H
#define WISHART_SPECIES_H

#include "Entity.h"

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

#define SPECIES_LENGTH(SPECIES) ((Species*)SPECIES)->entitiesList->length

Species* CreateSpecies(size_t initial_size);

void ClearSpecies(Species* species);

#endif //WISHART_SPECIES_H
