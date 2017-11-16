#include "GeneticAlgorithm.h"

#include <math.h>
#include <time.h>

#include "World.h"
#include "GAParameters.h"
#include "GAOperators.h"

#include "Logging/Logging.h"
#include "DeathManager/DeathManager.h"

static int last_error = 0;

static double Iteration(World* world, size_t generation_number);
static double GetMaxFitness(World* world);
static void CountDiedSpecies(World* world);

static void SetError(int error_code);
static int GetLastError();
static int ResetLastError();

GAResult RunEvolution(const GAParameters* parameters,
                      const GAOperators* operators) {
    GAResult result = { 0 };

    World* world = CreateWorld(parameters, operators);
    if (!world) {
        Log(ERROR, "Error creating world");
        return result;
    }

    clock_t begin, end;
    double time_spent = 0.0;

    double max_fitness = GetMaxFitness(world);
    double prev_max_fitness = 0.0;

    size_t value_is_stable_count = 0;
    for (size_t i = 0; i != parameters->max_generations_count; ++i) {
        begin = clock();

        double cur_fitness = Iteration(world, i + 1);
        if (GetLastError()) {
            goto destroy_world;
        }

        end = clock();
        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

        if (cur_fitness > max_fitness) {
            max_fitness = cur_fitness;
        }

        if (!world->size || parameters->stable_value_iterations_count) {
            if (!world->size || fabs(max_fitness - prev_max_fitness)
                                < parameters->stable_value_eps) {

                ++value_is_stable_count;
                if (!world->size
                    || value_is_stable_count
                       >= parameters->stable_value_iterations_count) {

                    result.optimum = max_fitness;
                    result.iterations_made = i;
                    result.time_spent_per_iteration = time_spent / i;
                    goto exit;
                }
            }
            else {
                value_is_stable_count = 0;
            }
        }
        prev_max_fitness = max_fitness;
    }

    result.optimum = max_fitness;
    result.iterations_made = parameters->max_generations_count;
    result.time_spent_per_iteration = time_spent
                                      / parameters->max_generations_count;

exit:
    DestroyWorld(world);
    return result;

destroy_world:
    DestroyWorld(world);
    result.error = true;
    return result;
}

// Static methods section -----------

double Iteration(World* world, size_t generation_number) {
    ResetLastError();

    IterationStart();

    if (world->operators->mutation) {
        if (!world->operators->mutation(world, generation_number)) {
            goto error;
        }
    }

    LIST_TYPE(EntityPtr) new_entities = NULL;
    LIST_TYPE(SpeciesPtr) clustered_species = NULL;
    if (world->operators->crossover) {
        new_entities = world->operators->crossover(world, generation_number);
        if (!new_entities) {
            goto error;
        }

        if (world->operators->children_selection) {
            if (!world->operators->children_selection(world, &new_entities)) {
                goto destroy_new_entities;
            }
        }

        if (world->operators->clustering) {
            clustered_species =
                    world->operators->clustering(world, new_entities,
                                                 world->parameters->eps,
                                                 world->parameters->min_pts);
            if (!clustered_species) {
                goto destroy_new_entities;
            }

            if (!ClearPopulation(world->population)) {
                goto destroy_clustered_species;
            }
            if (!list_move(SpeciesPtr, clustered_species, world->population)) {
                goto destroy_clustered_species;
            }
            LOG_ASSERT(list_len(clustered_species) == 0);
            list_destroy(SpeciesPtr, clustered_species);
            clustered_species = NULL;
        }
        else {
            if (!list_move(EntityPtr, new_entities,
                           list_first(world->population)->entities)) {
                goto destroy_new_entities;
            }
        }
        DestroyEntitiesList(new_entities);
        new_entities = NULL;

        world->size = 0;
        list_for_each(SpeciesPtr, world->population, var) {
            world->size += list_len(list_var_value(var)->entities);
        }

        if (world->operators->selection && world->size) {
            if (!world->operators->selection(world)) {
                goto error;
            }
        }
    }

    Log(INFO, "World size: %zu", world->size);

    CountDiedSpecies(world);

    IterationEnd();

    double max_fitness = GetMaxFitness(world);
    Log(INFO, "Iteration: max fitness: %.3f", max_fitness);
    LogMaxFitness(max_fitness);
    return max_fitness;

destroy_clustered_species:
    DestroyPopulation(clustered_species);
destroy_new_entities:
    DestroyEntitiesList(new_entities);
error:
    SetError(1);
    return 0.0;
}

double GetMaxFitness(World* world) {
    Entity* max_fitness_entity = NULL;
    list_for_each(SpeciesPtr, world->population, species_var) {
        list_for_each(EntityPtr,
                      list_var_value(species_var)->entities,
                      entity_var) {
            if (!max_fitness_entity
                || list_var_value(entity_var)->fitness
                   > max_fitness_entity->fitness) {

                max_fitness_entity = list_var_value(entity_var);
            }
        }
    }
    return max_fitness_entity ? max_fitness_entity->fitness : 0.0;
}

static void CountDiedSpecies(World* world) {
    list_for_each(SpeciesPtr, world->population, species_var) {
        Species* species = list_var_value(species_var);
        LOG_RELEASE_ASSERT(species->initial_size != 0);
        if (species->died / (double)species->initial_size > EXTINCTION_BIAS) {
            species->died = 0;
            species->initial_size = list_len(species->entities);
            SetEntitiesStatus(species->entities, true);
            RegisterDeath();
        }
    }
}

static void SetError(int error_code) {
    last_error = error_code;
    Log(ERROR, "Error set: %d", last_error);
}

int GetLastError() {
    return last_error;
}

int ResetLastError() {
    int temp = last_error;
    last_error = 0;
    return temp;
}