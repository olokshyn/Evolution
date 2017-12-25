//
// Created by Oleg on 12/24/16.
//

#include "World.h"

#include <stdbool.h>

#include "GAParameters.h"
#include "GAOperators.h"

#include "Logging/Logging.h"
#include "Journal/Journal.h"
#include "Common.h"

static bool InitPopulation(World* world);

World* CreateWorld(const GAParameters* parameters,
                   const GAOperators* operators,
                   const Journal* journal) {
    LOG_FUNC_START;

    if (!parameters) {
        Log(ERROR, "Cannot create world with NULL parameters");
        goto error;
    }
    if (!operators) {
        Log(ERROR, "Cannot create world with NULL operators");
        goto error;
    }

    World* world = (World*)malloc(sizeof(World));
    if (!world) {
        goto error;
    }

    world->population = list_create(SpeciesPtr);
    if (!world->population) {
        goto destroy_world;
    }

    world->size = parameters->initial_world_size;
    world->chr_size = get_chromosome_size(parameters);

    world->parameters = (GAParameters*)malloc(sizeof(GAParameters));
    if (!world->parameters) {
        goto destroy_population;
    }
    *world->parameters = *parameters;

    world->operators = (GAOperators*)malloc(sizeof(GAOperators));
    if (!world->operators) {
        goto destroy_parameters;
    }
    *world->operators = *operators;

    if (journal) {
        world->journal = (Journal*)malloc(sizeof(Journal));
        if (!world->journal) {
            goto destroy_operators;
        }
        *world->journal = *journal;
    }
    else {
        world->journal = NULL;
    }

    if (!InitPopulation(world)) {
        goto destroy_journal;
    }

    LOG_FUNC_SUCCESS;
    return world;

destroy_journal:
    free(world->journal);
destroy_operators:
    free(world->operators);
destroy_parameters:
    free(world->parameters);
destroy_population:
    DestroyPopulation(world->population);
destroy_world:
    free(world);
error:
    LOG_FUNC_ERROR;
    return NULL;
}

void DestroyWorld(World* world) {
    if (!world) {
        return;
    }
    free(world->journal);
    free(world->operators);
    free(world->parameters);
    DestroyPopulation(world->population);
    free(world);
}

static bool InitPopulation(World* world) {
    LOG_FUNC_START;

    if (!world || !world->population) {
        goto error;
    }

    Species* species = CreateSpecies(world->size);
    if (!species) {
        goto error;
    }

    Entity* entity = NULL;
    for (size_t i = 0; i != world->size; ++i) {
        entity = CreateEntity(world->chr_size);
        if (!entity) {
            goto destroy_species;
        }

        for (size_t j = 0; j != world->chr_size; ++j) {
            entity->chr[j] =
                    getRand(world->parameters->objective->min,
                            world->parameters->objective->max);
        }
        entity->fitness =
                world->parameters->objective->func(entity->chr,
                                                   (int)world->chr_size);
        entity->old = true;

        if (!list_push_back(EntityPtr, species->entities, entity)) {
            goto destroy_entity;
        }
        entity = NULL;
    }
    if (!list_push_back(SpeciesPtr, world->population, species)) {
        goto destroy_species;
    }

    LOG_FUNC_SUCCESS;
    return true;

destroy_entity:
    DestroyEntity(entity);
destroy_species:
    DestroySpecies(species);
error:
    LOG_FUNC_ERROR;
    return false;
}
