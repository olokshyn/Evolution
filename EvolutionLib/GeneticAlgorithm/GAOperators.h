//
// Created by Oleg on 12/24/16.
//

#ifndef EVOLUTION_GAOPERATORS_H
#define EVOLUTION_GAOPERATORS_H

#include "GAFwd.h"
#include "Species/Species.h"

typedef struct ga_operators {
    int (*mutation)(World* world, size_t generation_number);
    Species* (*crossover)(World* world, size_t generation_number);
    SpeciesList* (*clustering)(World* world, Species* new_species,
                               double eps, size_t min_pts);
    int (*children_selection)(World* world, Species* new_species);
    int (*selection)(World* world);
} GAOperators;

int GAO_UniformMutation(World* world, size_t generation_number);

int GAO_NonUniformMutation(World* world, size_t generation_number);

Species* GAO_UniformCrossover(World* world, size_t generation_number);

Species* GAO_FitnessCrossover(World* world, size_t generation_number);

SpeciesList* GAO_Clustering(World* world, Species* new_species,
                            double eps, size_t min_pts);

int GAO_ChildrenSelection(World* world, Species* new_species);

int GAO_SpeciesLinksSelection(World* world);

int GAO_LinearRankingSelection(World* world);

#define DEFAULT_GA_OPERATORS { \
    .mutation = GAO_UniformMutation, \
    .crossover = GAO_FitnessCrossover, \
    .clustering = GAO_Clustering, \
    .children_selection = GAO_ChildrenSelection, \
    .selection = GAO_SpeciesLinksSelection \
}

#define HERRERA_GA_OPERATORS { \
    .mutation = GAO_NonUniformMutation, \
    .crossover = GAO_UniformCrossover, \
    .selection = GAO_LinearRankingSelection \
}

#define HERRERA_WITH_CLUSTERING_GA_OPERATORS { \
    .mutation = GAO_NonUniformMutation, \
    .crossover = GAO_UniformCrossover, \
    .clustering = GAO_Clustering, \
    .selection = GAO_LinearRankingSelection \
}

#define LOKSHYN_GA_OPERATORS { \
    .mutation = GAO_NonUniformMutation, \
    .crossover = GAO_UniformCrossover, \
    .clustering = GAO_Clustering, \
    .selection = GAO_SpeciesLinksSelection \
}

#endif //EVOLUTION_GAOPERATORS_H
