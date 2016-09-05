//
// Created by Oleg on 5/22/16.
//

#include "Entity.h"

#include <math.h>

#define DOUBLE_EPS 0.0001

int EntityComparator(const void* a, const void* b) {
    double diff = (*(Entity**)b)->fitness
                  - (*(Entity**)a)->fitness;
    return (fabs(diff) < DOUBLE_EPS) ? 0 : (diff > 0) - (diff < 0);
}

Entity* CreateEntity(size_t chr_size) {
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    if (!entity) {
        goto  error_CreateEntity;
    }
    entity->chr = (double*)malloc(sizeof(double) * chr_size);
    if (!entity->chr) {
        goto error_CreateEntity;
    }

    return entity;

error_CreateEntity:
    free(entity);
    return NULL;
}

void EntityDestructor(Entity* entity) {
    if (entity) {
        free(entity->chr);
        free(entity);
    }
}

Entity* CopyEntity(Entity* entity, size_t chr_size) {
    Entity* new_entity = (Entity*)malloc(sizeof(Entity));
    if (!new_entity) {
        return NULL;
    }
    new_entity->chr = (double*)malloc(sizeof(double) * chr_size);
    if (!new_entity->chr) {
        free(new_entity);
        return NULL;
    }
    for (size_t i = 0; i < chr_size; ++i) {
        new_entity->chr[i] = entity->chr[i];
    }
    new_entity->fitness = entity->fitness;
    new_entity->old = entity->old;
    return new_entity;
}

List* CreateEntitiesList() {
    List* entities = (List*)malloc(sizeof(List));
    if (!entities) {
        return NULL;
    }
    initList(entities, EntityComparator, (void (*)(void*))EntityDestructor);
    return entities;
}

void MarkAllAsNew(List* entitiesList) {
    if (!entitiesList || !entitiesList->length) {
        return;
    }
    for (ListIterator it = begin(entitiesList);
            !isIteratorExhausted(it);
            next(&it)) {
        ((Entity*)it.current->value)->old = 0;
    }
}

void MarkAllAsOld(List* entitiesList) {
    if (!entitiesList || !entitiesList->length) {
        return;
    }
    for (ListIterator it = begin(entitiesList);
         !isIteratorExhausted(it);
         next(&it)) {
        ((Entity*)it.current->value)->old = 1;
    }
}

void CrossEntities(Entity* parent1,
                   Entity* parent2,
                   Entity* child,
                   ObjectiveFunc Ofunc,
                   size_t chromosome_size) {
    size_t crossover_point = (chromosome_size % 2 == 0) ?
                             (chromosome_size / 2) :
                             (chromosome_size / 2 + 1);
    for (size_t i = 0; i < crossover_point; ++i) {
        child->chr[i] = parent1->chr[i];
    }
    for (size_t i = crossover_point; i < chromosome_size; ++i) {
        child->chr[i] = parent2->chr[i];
    }
    child->fitness = Ofunc(child->chr, (int)chromosome_size);
    child->old = 0;
}