//
// Created by Oleg on 12/24/16.
//

#ifndef EVOLUTION_GAOPERATORS_H
#define EVOLUTION_GAOPERATORS_H

#include <stdbool.h>

#include "GAFwd.h"
#include "Species/Species.h"

typedef struct ga_operators {
    bool (*mutation)(World* world, size_t generation_number);
    LIST_TYPE(EntityPtr) (*crossover)(World* world, size_t generation_number);
    LIST_TYPE(SpeciesPtr) (*clustering)(World* world,
                                        LIST_TYPE(EntityPtr) new_entities,
                                        double eps,
                                        size_t min_pts);
    bool (*children_selection)(World* world, LIST_TYPE(EntityPtr)* new_entities);
    bool (*selection)(World* world);
} GAOperators;

bool GAO_UniformMutation(World* world, size_t generation_number);

bool GAO_NonUniformMutation(World* world, size_t generation_number);

LIST_TYPE(EntityPtr) GAO_UniformCrossover(World* world,
                                          size_t generation_number);

LIST_TYPE(EntityPtr) GAO_FitnessCrossover(World* world,
                                          size_t generation_number);

LIST_TYPE(SpeciesPtr) GAO_Clustering(World* world,
                                     LIST_TYPE(EntityPtr) new_entities,
                                     double eps,
                                     size_t min_pts);

bool GAO_ChildrenSelection(World* world, LIST_TYPE(EntityPtr)* new_entities);

bool GAO_SpeciesLinksSelection(World* world);

bool GAO_LinearRankingSelection(World* world);

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
