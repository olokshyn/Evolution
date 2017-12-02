//
// Created by Oleg on 11/16/17.
//

#ifndef EVOLUTION_JOURNAL_H
#define EVOLUTION_JOURNAL_H

#include "Species/Species.h"

typedef struct journal {
    void* data;
    bool (*evolution_stop_requested)(void* data);
    void (*iteration_start)(void* data,
                            LIST_TYPE(SpeciesPtr) population,
                            size_t generation_number);
    void (*iteration_end)(void* data,
                          LIST_TYPE(SpeciesPtr) population,
                          size_t species_died_on_iteration);

    void (*crossover)(void* data,
                      LIST_TYPE(SpeciesPtr) population,
                      LIST_TYPE(EntityPtr) new_entities);
    void (*children_selection)(void* data,
                               LIST_TYPE(SpeciesPtr) population,
                               LIST_TYPE(EntityPtr) new_entities);
    void (*clustering)(void* data,
                       LIST_TYPE(SpeciesPtr) population,
                       LIST_TYPE(SpeciesPtr) new_population);
    void (*before_selection)(void* data,
                             LIST_TYPE(SpeciesPtr) population,
                             size_t world_size);
    void (*selection)(void* data,
                      LIST_TYPE(SpeciesPtr) population,
                      size_t world_size);

    void (*species_death)(void* data,
                          size_t initial_size);
} Journal;

bool EvolutionStopRequested(Journal* journal);

void RecordIterationStart(Journal* journal,
                          LIST_TYPE(SpeciesPtr) population,
                          size_t generation_number);

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
