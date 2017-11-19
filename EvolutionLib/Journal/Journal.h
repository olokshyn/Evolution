//
// Created by Oleg on 11/16/17.
//

#ifndef EVOLUTION_JOURNAL_H
#define EVOLUTION_JOURNAL_H

#include "Species/Species.h"

typedef struct journal {
    void (*iteration_start)(LIST_TYPE(SpeciesPtr) population);
    void (*iteration_end)(LIST_TYPE(SpeciesPtr) population,
                          size_t species_died_on_iteration);

    void (*crossover)(LIST_TYPE(SpeciesPtr) population,
                      LIST_TYPE(EntityPtr) new_entities);
    void (*children_selection)(LIST_TYPE(SpeciesPtr) population,
                               LIST_TYPE(EntityPtr) new_entities);
    void (*clustering)(LIST_TYPE(SpeciesPtr) population,
                       LIST_TYPE(SpeciesPtr) new_population);
    void (*before_selection)(LIST_TYPE(SpeciesPtr) population,
                             size_t world_size);
    void (*selection)(LIST_TYPE(SpeciesPtr) population,
                      size_t world_size);

    void (*species_death)(size_t initial_size);
} Journal;

void RecordIterationStart(Journal* journal,
                          LIST_TYPE(SpeciesPtr) population);

void RecordIterationEnd(Journal* journal,
                        LIST_TYPE(SpeciesPtr) population);

void RecordCrossover(Journal* journal,
                     LIST_TYPE(SpeciesPtr) population,
                     LIST_TYPE(EntityPtr) new_entities);

void RecordChildrenSelection(Journal* journal,
                             LIST_TYPE(SpeciesPtr) population,
                             LIST_TYPE(EntityPtr) new_entities);

void RecordClustering(Journal* journal,
                      LIST_TYPE(SpeciesPtr) population,
                      LIST_TYPE(SpeciesPtr) new_population);

void RecordBeforeSelection(Journal* journal,
                           LIST_TYPE(SpeciesPtr) population,
                           size_t world_size);

void RecordSelection(Journal* journal,
                     LIST_TYPE(SpeciesPtr) population,
                     size_t world_size);

void RecordSpeciesDeath(Journal* journal,
                        size_t initial_size);

#endif //EVOLUTION_JOURNAL_H
