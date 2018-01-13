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

LIST_TYPE(double) NormalizePopulationFitnesses(
        LIST_TYPE(SpeciesPtr) population) {
    LIST_TYPE(double) fitnesses = list_create(double);
    if (!fitnesses) {
        return NULL;
    }

    list_for_each(SpeciesPtr, population, var) {
        double mid_fitness;
        GetFitnesses(list_var_value(var)->entities,
                     &mid_fitness, NULL, NULL);
        if (!list_push_back(double, fitnesses, mid_fitness)) {
            goto destroy_fitnesses;
        }
    }

    Normalize(fitnesses);

    return fitnesses;

destroy_fitnesses:
    list_destroy(double, fitnesses);
    return NULL;
}

double* NormalizePopulationFitnesses_array(
        LIST_TYPE(SpeciesPtr) population) {
    double* fitnesses = (double*)malloc(sizeof(double) * list_len(population));
    if (!fitnesses) {
        return NULL;
    }

    size_t index = 0;
    list_for_each(SpeciesPtr, population, var) {
        double mid_fitness;
        GetFitnesses(list_var_value(var)->entities, &mid_fitness, NULL, NULL);
        fitnesses[index++] = mid_fitness;
    }

    Normalize_array(fitnesses, list_len(population));

    return fitnesses;
}
