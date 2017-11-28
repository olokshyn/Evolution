//
// Created by Oleg on 11/16/17.
//

#include "Journal.h"

#include <stdio.h>
#include <string.h>

#include "Logging/Logging.h"
#include "Common.h"

static char* PopulationInfo(LIST_TYPE(SpeciesPtr) population);
static char* EntitiesInfo(LIST_TYPE(EntityPtr) entities);

_Thread_local size_t species_died_on_iteration = 0;
_Thread_local size_t new_entities_before_selection = 0;
_Thread_local size_t entities_before_selection = 0;

#define CHECK_STR(str) if (!(str)) { \
    Log(ERROR, "%s: Failed to allocate memory", __func__); \
    goto error; \
}

void RecordIterationStart(Journal* journal,
                          LIST_TYPE(SpeciesPtr) population,
                          size_t generation_number) {
    char* population_info = NULL;
    CHECK_STR(population_info = PopulationInfo(population))
    new_entities_before_selection = 0;
    entities_before_selection = 0;
    Log(INFO, "Iteration start: %s, gen number: %zu",
        population_info, generation_number);
    free(population_info);
    if (journal && journal->iteration_start) {
        journal->iteration_start(journal->data,
                                 population,
                                 generation_number);
    }
    species_died_on_iteration = 0;

error:
    return;
}

void RecordIterationEnd(Journal* journal,
                        LIST_TYPE(SpeciesPtr) population) {
    char* population_info = NULL;
    CHECK_STR(population_info = PopulationInfo(population))
    new_entities_before_selection = 0;
    entities_before_selection = 0;
    Log(INFO, "Iteration end: %s, species died: %zu",
        population_info, species_died_on_iteration);
    free(population_info);
    if (journal && journal->iteration_end) {
        journal->iteration_end(journal->data,
                               population,
                               species_died_on_iteration);
    }
    species_died_on_iteration = 0;

error:
    return;
}

void RecordCrossover(Journal* journal,
                     LIST_TYPE(SpeciesPtr) population,
                     LIST_TYPE(EntityPtr) new_entities) {
    char* population_info = NULL;
    char* entities_info = NULL;
    CHECK_STR(population_info = PopulationInfo(population))
    CHECK_STR(entities_info = EntitiesInfo(new_entities))
    new_entities_before_selection = list_len(new_entities);
    Log(INFO, "Crossover: population: %s; new entities: %s",
        population_info, entities_info);
    free(entities_info);
    free(population_info);
    if (journal && journal->crossover) {
        journal->crossover(journal->data, population, new_entities);
    }
    return;

error:
    free(population_info);
}

void RecordChildrenSelection(Journal* journal,
                             LIST_TYPE(SpeciesPtr) population,
                             LIST_TYPE(EntityPtr) new_entities) {
    char* population_info = NULL;
    char* entities_info = NULL;
    CHECK_STR(population_info = PopulationInfo(population))
    CHECK_STR(entities_info = EntitiesInfo(new_entities))
    size_t died_entities = 0;
    if (new_entities_before_selection > list_len(new_entities)) {
        died_entities = new_entities_before_selection - list_len(new_entities);
    }
    new_entities_before_selection = 0;
    Log(INFO, "Children selection: population: %s; "
                "new entities: %s, died entities: %zu",
        population_info, entities_info, died_entities);
    free(entities_info);
    free(population_info);
    if (journal && journal->children_selection) {
        journal->children_selection(journal->data, population, new_entities);
    }
    return;

error:
    free(population_info);
}

void RecordClustering(Journal* journal,
                      LIST_TYPE(SpeciesPtr) population,
                      LIST_TYPE(SpeciesPtr) new_population) {
    char* population_info = NULL;
    char* new_population_info = NULL;
    CHECK_STR(population_info = PopulationInfo(population))
    CHECK_STR(new_population_info = PopulationInfo(new_population))
    Log(INFO, "Clustering: old population: %s; new population: %s",
        population_info, new_population_info);
    free(new_population_info);
    free(population_info);
    if (journal && journal->clustering) {
        journal->clustering(journal->data, population, new_population);
    }
    return;

error:
    free(population_info);
}

void RecordBeforeSelection(Journal* journal,
                           LIST_TYPE(SpeciesPtr) population,
                           size_t world_size) {
    entities_before_selection = world_size;
    if (journal && journal->before_selection) {
        journal->before_selection(journal->data, population, world_size);
    }
}

void RecordSelection(Journal* journal,
                     LIST_TYPE(SpeciesPtr) population,
                     size_t world_size) {
    char* population_info = NULL;
    CHECK_STR(population_info = PopulationInfo(population))
    size_t died_entities = 0;
    if (entities_before_selection > world_size) {
        died_entities = entities_before_selection - world_size;
    }
    entities_before_selection = 0;
    Log(INFO, "Selection: %s, died entities: %zu",
        population_info, died_entities);
    free(population_info);
    if (journal && journal->selection) {
        journal->selection(journal->data, population, world_size);
    }

error:
    return;
}

void RecordSpeciesDeath(Journal* journal,
                        size_t initial_size) {
    Log(INFO, "Species died: size: %zu", initial_size);
    if (journal && journal->species_death) {
        journal->species_death(journal->data, initial_size);
    }
    ++species_died_on_iteration;
}

static char* PopulationInfo(LIST_TYPE(SpeciesPtr) population) {
    static const char* header = "[%3zu]{";
    static const char* footer = " }";
    static const size_t template_size = 9;
    static const size_t digits_per_number = 7;

    size_t total_size = sizeof(char)
                        * (list_len(population) * digits_per_number + template_size);
    char* buffer = (char*)malloc((size_t)total_size);
    if (!buffer) {
        return NULL;
    }

    char* s = buffer;
    size_t size = total_size - strlen(footer);
    int written = snprintf(s, size, header, list_len(population));
    if (written <= 0 || written >= size) {
        goto error;
    }
    s += written;
    total_size -= written;

    list_for_each(SpeciesPtr, population, var) {
        size = MAX(digits_per_number, total_size);
        written = snprintf(s, size, "%5zu, ",
                           list_len(list_var_value(var)->entities));
        if (written <= 0 || written >= size) {
            goto error;
        }
        s += written;
        total_size -= written;
    }

    size = MAX(strlen(footer) + 1, total_size);
    written = snprintf(s, size, footer);
    if (written <= 0 || written >= size) {
        goto error;
    }
    total_size -= written;
    LOG_RELEASE_ASSERT(total_size > 0);

    return buffer;

error:
    free(buffer);
    return NULL;
}

static char* EntitiesInfo(LIST_TYPE(EntityPtr) entities) {
    static const char* format = "[%3zu](mid ft: %8.3f, min ft: %8.3f, max ft: %8.3f)";
    static const size_t format_length = 60;

    char* buffer = (char*)malloc(sizeof(char) * format_length);
    if (!buffer) {
        return NULL;
    }

    double mid_fitness;
    double min_fitness;
    double max_fitness;
    GetFitnesses(entities, &mid_fitness, &min_fitness, &max_fitness);

    size_t size = format_length;
    int written = snprintf(buffer, size, format,
                           list_len(entities),
                           mid_fitness, min_fitness, max_fitness);
    if (written <= 0 || written >= size) {
        goto error;
    }
    return buffer;

error:
    free(buffer);
    return NULL;
}
