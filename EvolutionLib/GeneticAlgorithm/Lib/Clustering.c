//
// Created by oleg on 08.01.18.
//

#include "Clustering.h"

#include "GeneticAlgorithm/World.h"
#include "OPTICS/optics.h"
#include "Utils.h"

LIST_TYPE(SpeciesPtr) Clustering_Template(
        World* world,
        LIST_TYPE(EntityPtr) new_entities,
        double eps,
        size_t min_pts,
        LIST_TYPE(SpeciesPtr) (*clustering)(World* world,
                                            LIST_TYPE(EntityPtr) new_entities,
                                            double eps, size_t min_pts,
                                            LIST_TYPE(EntityPtr) noise_entities)) {
    Species* noise_species = CreateSpecies(0);
    if (!noise_species) {
        goto error;
    }

    LIST_TYPE(SpeciesPtr) population = clustering(
            world,
            new_entities,
            ScaleEpsToEntitiesNorms(world, new_entities, eps),
            min_pts,
            noise_species->entities);
    if (!population) {
        goto destroy_noise_species;
    }

    if (list_len(noise_species->entities) > 1) {
        noise_species->initial_size = list_len(noise_species->entities);
        SetEntitiesStatus(noise_species->entities, true);
        if (!list_push_back(SpeciesPtr, population, noise_species)) {
            goto destroy_population;
        }
    }
    else
    {
        DestroySpecies(noise_species);
    }
    noise_species = NULL;

    return population;

destroy_population:
    DestroyPopulation(population);
destroy_noise_species:
    DestroySpecies(noise_species);
error:
    return NULL;
}

LIST_TYPE(SpeciesPtr) Clustering_OPTICS(World* world,
                                        LIST_TYPE(EntityPtr) new_entities,
                                        double eps,
                                        size_t min_pts) {
    return Clustering_Template(world, new_entities, eps, min_pts, OPTICSClustering);
}
