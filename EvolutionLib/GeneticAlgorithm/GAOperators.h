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

bool GAO_ElitistsSelection(World* world);

extern const GAOperators HerreraOperators;

extern const GAOperators HerreraWithClusteringOperators;

extern const GAOperators LokshynOperators;

static const char* const OperatorsNames[] = {
        "Herrera",
        "Herrera with clustering",
        "Lokshyn"
};

static const GAOperators* const Operators[] = {
        &HerreraOperators,
        &HerreraWithClusteringOperators,
        &LokshynOperators
};

static_assert(sizeof(OperatorsNames) / sizeof(OperatorsNames[0])
              == sizeof(Operators) / sizeof(Operators[0]),
              "Operators names are ill-formed");
static const size_t Operators_count = sizeof(Operators) / sizeof(Operators[0]);

#endif //EVOLUTION_GAOPERATORS_H
