//
// Created by Oleg on 6/1/16.
//

#include "Species.h"

#include "Common.h"
#include "Logging/Logging.h"

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

void DestroySpecies(Species* species) {
    if (species) {
        DestroyEntitiesList(species->entitiesList);
        free(species);
    }
}

SpeciesList* CreateSpeciesList() {
    SpeciesList* species = (SpeciesList*)malloc(sizeof(SpeciesList));
    if (!species) {
        return NULL;
    }
    InitSpeciesList(species);
    return species;
}

double GetMidFitness(Species* species) {
    double fitness = 0.0;
    FOR_EACH_IN_SPECIES(species) {
        LOG_RELEASE_ASSERT(ENTITIES_IT_P->fitness == ENTITIES_IT_P->fitness); // check for NaN
        fitness += ENTITIES_IT_P->fitness;
    }
    double res = fitness / SPECIES_LENGTH(species);
    LOG_RELEASE_ASSERT(res == res);  // check for NaN
    return res;
}

List* NormalizeSpeciesFitnesses(SpeciesList* species) {
    List* fitness_list = NULL;
    double* fitness = NULL;

    fitness_list = (List*)malloc(sizeof(List));
    if (!fitness_list) {
        goto error_NormalizeSpeciesFitnesses;
    }
    initList(fitness_list, NULL, free);

    FOR_EACH_IN_SPECIES_LIST(species) {
        if (!SPECIES_LENGTH(SPECIES_LIST_IT_P)) {
            continue;
        }
        fitness = (double*)malloc(sizeof(double));
        if (!fitness) {
            goto error_NormalizeSpeciesFitnesses;
        }
        *fitness = GetMidFitness(SPECIES_LIST_IT_P);
        if (!pushBack(fitness_list, fitness)) {
            goto error_NormalizeSpeciesFitnesses;
        }
        fitness = NULL;
    }

    Normalize(fitness_list);

    return fitness_list;

error_NormalizeSpeciesFitnesses:
    destroyListPointer(fitness_list);
    free(fitness);
    return NULL;
}