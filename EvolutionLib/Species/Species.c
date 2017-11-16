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
    species->entities = list_create(EntityPtr);
    if (!species->entities) {
        goto error;
    }
    species->died = 0;
    species->initial_size = initial_size;
    return species;

error:
    free(species);
    return NULL;
}

void DestroySpecies(Species* species) {
    if (species) {
        DestroyEntitiesList(species->entities);
        free(species);
    }
}

bool ClearPopulation(LIST_TYPE(SpeciesPtr) population) {
    if (!population) {
        return true;
    }
    list_for_each(SpeciesPtr, population, var) {
        DestroySpecies(list_var_value(var));
    }
    return list_clear(SpeciesPtr, population);
}

void DestroyPopulation(LIST_TYPE(SpeciesPtr) population) {
    if (!population) {
        return;
    }
    list_for_each(SpeciesPtr, population, var) {
        DestroySpecies(list_var_value(var));
    }
    list_destroy(SpeciesPtr, population);
}

double GetMidFitness(Species* species) {
    LOG_RELEASE_ASSERT(species);
    LOG_RELEASE_ASSERT(species->entities);
    // Species must not be empty
    LOG_RELEASE_ASSERT(list_len(species->entities));

    double fitness = 0.0;
    list_for_each(EntityPtr, species->entities, var) {
        fitness += list_var_value(var)->fitness;
    }
    return fitness / list_len(species->entities);
}

LIST_TYPE(double) NormalizePopulationFitnesses(
        LIST_TYPE(SpeciesPtr) population) {
    LIST_TYPE(double) fitnesses = list_create(double);
    if (!fitnesses) {
        return NULL;
    }

    list_for_each(SpeciesPtr, population, var) {
        if (!list_push_back(double, fitnesses,
                            GetMidFitness(list_var_value(var)))) {
            goto destroy_fitnesses;
        }
    }

    Normalize(fitnesses);

    return fitnesses;

destroy_fitnesses:
    list_destroy(double, fitnesses);
    return NULL;
}
