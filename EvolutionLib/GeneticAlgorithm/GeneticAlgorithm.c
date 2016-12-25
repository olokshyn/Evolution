#include "GeneticAlgorithm.h"

#include <math.h>
#include <time.h>

#include "World.h"
#include "GAParameters.h"
#include "GAOperators/GAOperators.h"

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
        goto error_RunEvolution;
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
            goto error_RunEvolution;
        }

        end = clock();
        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

        if (cur_fitness > max_fitness) {
            max_fitness = cur_fitness;
        }

        if (fabs(max_fitness - prev_max_fitness)
            < parameters->stable_value_eps) {

            ++value_is_stable_count;
            if (parameters->stable_value_iterations_count
                && value_is_stable_count
                   >= parameters->stable_value_iterations_count) {

                result.optimum = max_fitness;
                result.iterations_made = i;
                result.time_spent_per_iteration = time_spent / i;
                goto exit_RunEvolution;
            }
        }
        else {
            value_is_stable_count = 0;
        }
        prev_max_fitness = max_fitness;
    }

    result.optimum = max_fitness;
    result.iterations_made = parameters->max_generations_count;
    result.time_spent_per_iteration = time_spent
                                      / parameters->max_generations_count;

exit_RunEvolution:
    ClearWorld(&world);
    return result;

error_RunEvolution:
    ClearWorld(&world);
    result.error = 1;
    return result;
}

// Static methods section -----------

double Iteration(World* world, size_t generation_number) {
    ResetLastError();

    IterationStart();

    Species* new_species = NULL;
    SpeciesList* clustered_species = NULL;

    if (world->operators->mutation) {
        if (!world->operators->mutation(world, generation_number)) {
            goto error_Iteration;
        }
    }

    if (world->operators->crossover) {
        new_species = world->operators->crossover(world, generation_number);
        if (!new_species) {
            goto error_Iteration;
        }

        if (world->operators->children_selection) {
            if (!world->operators->children_selection(world, new_species)) {
                goto error_Iteration;
            }
        }

        if (world->operators->clustering) {
            clustered_species =
                    world->operators->clustering(world, new_species);
            if (!clustered_species) {
                goto error_Iteration;
            }

            clearList(&world->species);
            moveList(&world->species, clustered_species);
            destroyListPointer(clustered_species);
            clustered_species = NULL;

            world->world_size = 0;
            FOR_EACH_IN_SPECIES_LIST(&world->species) {
                world->world_size += SPECIES_LENGTH(SPECIES_LIST_IT_P);
            }
        }
        else {
            if (!moveList(((Species*)world->species.head->value)->entitiesList,
                          new_species->entitiesList)) {
                goto error_Iteration;
            }

            world->world_size += SPECIES_LENGTH(new_species);
        }

        DestroySpecies(new_species);
        new_species = NULL;

        if (world->operators->selection) {
            world->world_size =
                    world->operators->selection(world);
            if (!world->world_size) {
                goto error_Iteration;
            }
        }
    }

    CountDiedSpecies(world);

    IterationEnd();

    double max_fitness = GetMaxFitness(world);
    Log(INFO, "Iteration: max fitness: %.3f", max_fitness);
    LogMaxFitness(max_fitness);
    return max_fitness;

error_Iteration:
    DestroySpecies(new_species);
    destroyListPointer(clustered_species);
    SetError(1);
    return 0.0;
}

double GetMaxFitness(World* world) {
    Entity* max_fitness_entity = NULL;
    FOR_EACH_IN_SPECIES_LIST(&world->species) {
        FOR_EACH_IN_SPECIES(SPECIES_LIST_IT_P) {
            if (!max_fitness_entity
                || ENTITIES_IT_P->fitness > max_fitness_entity->fitness) {
                max_fitness_entity = ENTITIES_IT_P;
            }
        }
    }
    return max_fitness_entity ? max_fitness_entity->fitness : 0.0;
}

static void CountDiedSpecies(World* world) {
    FOR_EACH_IN_SPECIES_LIST(&world->species) {
        LOG_ASSERT(SPECIES_LIST_IT_P->initial_size != 0);
        if (SPECIES_LIST_IT_P->died / (double)SPECIES_LIST_IT_P->initial_size
            > EXTINCTION_BIAS) {
            SPECIES_LIST_IT_P->died = 0;
            SPECIES_LIST_IT_P->initial_size = SPECIES_LENGTH(SPECIES_LIST_IT_P);
            MarkAllAsOld(SPECIES_LIST_IT_P->entitiesList);
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