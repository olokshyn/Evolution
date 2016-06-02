//
// Created by Oleg on 6/1/16.
//

#include "Species.h"


Species* CreateSpecies(size_t initial_size) {
    Species* species = (Species*)malloc(sizeof(Species));
    species->entitiesList = CreateEntitiesList();
    species->died = 0;
    species->initial_size = initial_size;
    return species;
}

void ClearSpecies(Species* species) {
    if (species) {
        clearListPointer(species->entitiesList);
        free(species);
    }
}