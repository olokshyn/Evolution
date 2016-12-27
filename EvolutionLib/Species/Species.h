//
// Created by Oleg on 6/1/16.
//

#ifndef EVOLUTION_SPECIES_H
#define EVOLUTION_SPECIES_H

#include <stdlib.h>

#include "List/List.h"
#include "Entity/Entity.h"

typedef struct species {
    EntitiesList* entitiesList;
    size_t died;
    size_t initial_size;
} Species;

typedef List SpeciesList;

Species* CreateSpecies(size_t initial_size);

void DestroySpecies(Species* species);

SpeciesList* CreateSpeciesList();

static inline int InitSpeciesList(SpeciesList* species) {
    return initList(species, NULL, (void (*)(void*))DestroySpecies);
}

static inline void DestroySpeciesList(SpeciesList* species) {
    destroyListPointer(species);
}

double GetMidFitness(Species* species);

List* NormalizeSpeciesFitnesses(SpeciesList* species);

List* NormalizeSpeciesFitnesses2(SpeciesList* species);

#define FOR_EACH_IN_SPECIES_N(SPECIES_P, LIST_IT) \
    FOR_EACH_IN_ENTITIES_N(((Species*)SPECIES_P)->entitiesList, LIST_IT)

#define FOR_EACH_IN_SPECIES(SPECIES_P) \
    FOR_EACH_IN_ENTITIES(((Species*)SPECIES_P)->entitiesList)

#define SPECIES_LENGTH(SPECIES_P) \
    (SPECIES_P ? ((Species*)SPECIES_P)->entitiesList->length : 0)

#define FOR_EACH_IN_SPECIES_LIST(SPECIES_LIST_P) \
    FOR_EACH_IN_LIST_N(SPECIES_LIST_P, species_list_it)

#define SPECIES_LIST_IT_P \
    LIST_IT_VALUE_P_N(species_list_it, Species)

#endif //EVOLUTION_SPECIES_H
