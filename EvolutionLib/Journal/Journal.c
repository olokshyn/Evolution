//
// Created by Oleg on 11/16/17.
//

#include "Journal.h"

_Thread_local size_t species_died_on_iteration = 0;

bool EvolutionStopRequested(Journal* journal) {
    if (journal && journal->evolution_stop_requested) {
        return journal->evolution_stop_requested(journal->data);
    }
    return false;
}

void RecordIterationStart(Journal* journal,
                          LIST_TYPE(SpeciesPtr) population,
                          size_t generation_number) {
    if (journal && journal->iteration_start) {
        journal->iteration_start(journal->data,
                                 population,
                                 generation_number);
    }
    species_died_on_iteration = 0;
}

void RecordIterationEnd(Journal* journal,
                        LIST_TYPE(SpeciesPtr) population) {
    if (journal && journal->iteration_end) {
        journal->iteration_end(journal->data,
                               population,
                               species_died_on_iteration);
    }
    species_died_on_iteration = 0;
}

void RecordCrossover(Journal* journal,
                     LIST_TYPE(SpeciesPtr) population,
                     LIST_TYPE(EntityPtr) new_entities) {
    if (journal && journal->crossover) {
        journal->crossover(journal->data, population, new_entities);
    }
}

void RecordChildrenSelection(Journal* journal,
                             LIST_TYPE(SpeciesPtr) population,
                             LIST_TYPE(EntityPtr) new_entities) {
    if (journal && journal->children_selection) {
        journal->children_selection(journal->data, population, new_entities);
    }
}

void RecordClustering(Journal* journal,
                      LIST_TYPE(SpeciesPtr) population,
                      LIST_TYPE(SpeciesPtr) new_population) {
    if (journal && journal->clustering) {
        journal->clustering(journal->data, population, new_population);
    }
}

void RecordBeforeSelection(Journal* journal,
                           LIST_TYPE(SpeciesPtr) population,
                           size_t world_size) {
    if (journal && journal->before_selection) {
        journal->before_selection(journal->data, population, world_size);
    }
}

void RecordSelection(Journal* journal,
                     LIST_TYPE(SpeciesPtr) population,
                     size_t world_size) {
    if (journal && journal->selection) {
        journal->selection(journal->data, population, world_size);
    }
}

void RecordSpeciesDeath(Journal* journal,
                        size_t initial_size) {
    if (journal && journal->species_death) {
        journal->species_death(journal->data, initial_size);
    }
    ++species_died_on_iteration;
}
