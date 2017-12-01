//
// Created by Oleg on 5/22/16.
//

#include "Entity.h"

#include <math.h>

#include "Common.h"
#include "Logging/Logging.h"

#define ENTITY_ASCENDING_COMPARATOR(A, B) \
    double diff = (*(Entity**)(A))->fitness \
                   - (*(Entity**)(B))->fitness; \
    return (fabs(diff) < DOUBLE_EPS) ? 0 : SIGN(diff);

int EntityAscendingComparator(const void* a, const void* b) {
    ENTITY_ASCENDING_COMPARATOR(a, b)
}

int EntityDescendingComparator(const void* a, const void* b) {
    ENTITY_ASCENDING_COMPARATOR(b, a)
}

Entity* CreateEntity(size_t chr_size) {
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    if (!entity) {
        return NULL;
    }
    entity->chr = (double*)malloc(sizeof(double) * chr_size);
    if (!entity->chr) {
        goto error;
    }

    return entity;

error:
    free(entity);
    return NULL;
}

void DestroyEntity(Entity* entity) {
    if (entity) {
        free(entity->chr);
        free(entity);
    }
}

Entity* CopyEntity(const Entity* entity, size_t chr_size) {
    if (!entity) {
        return NULL;
    }
    Entity* new_entity = (Entity*)malloc(sizeof(Entity));
    if (!new_entity) {
        return NULL;
    }
    new_entity->chr = (double*)malloc(sizeof(double) * chr_size);
    if (!new_entity->chr) {
        goto error;
    }
    for (size_t i = 0; i != chr_size; ++i) {
        new_entity->chr[i] = entity->chr[i];
    }
    new_entity->fitness = entity->fitness;
    new_entity->old = entity->old;

    return new_entity;

error:
    free(new_entity);
    return NULL;
}

bool ClearEntitiesList(LIST_TYPE(EntityPtr) entities) {
    if (!entities) {
        return true;
    }
    list_for_each(EntityPtr, entities, var) {
        DestroyEntity(list_var_value(var));
    }
    return list_clear(EntityPtr, entities);
}

void DestroyEntitiesList(LIST_TYPE(EntityPtr) entities) {
    if (!entities) {
        return;
    }
    list_for_each(EntityPtr, entities, var) {
        DestroyEntity(list_var_value(var));
    }
    list_destroy(EntityPtr, entities);
}

void SetEntitiesStatus(LIST_TYPE(EntityPtr) entities, bool old) {
    if (!entities) {
        return;
    }
    list_for_each(EntityPtr, entities, var) {
        list_var_value(var)->old = old;
    }
}

void GetFitnesses(LIST_TYPE(EntityPtr) entities,
                  double* mid_fitness,
                  double* min_fitness,
                  double* max_fitness) {
    LOG_RELEASE_ASSERT(entities);
    // Entities list must not be empty
    LOG_RELEASE_ASSERT(list_len(entities));

    double _mid_fitness = 0.0;
    double _min_fitness = list_first(entities)->fitness;
    double _max_fitness = list_first(entities)->fitness;
    list_for_each(EntityPtr, entities, var) {
        _mid_fitness += list_var_value(var)->fitness;
        if (list_var_value(var)->fitness < _min_fitness) {
            _min_fitness = list_var_value(var)->fitness;
        }
        else if (list_var_value(var)->fitness > _max_fitness) {
            _max_fitness = list_var_value(var)->fitness;
        }
    }

    if (mid_fitness) {
        *mid_fitness = _mid_fitness / list_len(entities);
    }
    if (min_fitness) {
        *min_fitness = _min_fitness;
    }
    if (max_fitness) {
        *max_fitness = _max_fitness;
    }
}

LIST_TYPE(double) NormalizeEntitiesFitnesses(LIST_TYPE(EntityPtr) entities) {
    LOG_RELEASE_ASSERT(entities);

    LIST_TYPE(double) fitnesses = list_create(double);
    if (!fitnesses) {
        return NULL;
    }

    list_for_each(EntityPtr, entities, var) {
        if (!list_push_back(double, fitnesses, list_var_value(var)->fitness)) {
            goto destroy_fitnesses;
        }
    }

    Normalize(fitnesses);

    return fitnesses;

destroy_fitnesses:
    list_destroy(double, fitnesses);
    return NULL;
}

Entity** SortedEntitiesPointers(LIST_TYPE(EntityPtr) entities,
                                int (*comparator)(const void*, const void*)) {
    LOG_RELEASE_ASSERT(list_len(entities));

    Entity** entities_p =
            (Entity**)malloc(sizeof(Entity*) * list_len(entities));
    if (!entities_p) {
        return NULL;
    }

    size_t index = 0;
    list_for_each(EntityPtr, entities, var) {
        entities_p[index++] = list_var_value(var);
    }

    qsort(entities_p, list_len(entities), sizeof(Entity*), comparator);

    return entities_p;
}
