//
// Created by Oleg on 6/1/16.
//

#include "Species.h"

Species* CreateSpecies(size_t initial_size) {
    Species* species = (Species*)malloc(sizeof(Species));
    if (!species) {
        return NULL;
    }
    species->entitiesList = CreateEntitiesList();
    species->died = 0;
    species->initial_size = initial_size;
    return species;
}

double GetMidFitness(Species* species) {
    double fitness = 0.0;
    FOR_EACH_IN_SPECIES(species) {
        fitness += ENTITY_SP_IT->fitness;
    }
    return fitness / SPECIES_LENGTH(species);
}

void ClearSpecies(Species* species) {
    if (species) {
        clearListPointer(species->entitiesList);
        free(species);
    }
}