//
// Created by Oleg on 5/22/16.
//

#ifndef EVOLUTION_ENTITY_H
#define EVOLUTION_ENTITY_H

#include "List/List.h"

typedef struct entity {
    double* chr;
    double fitness;
    char old;
} Entity;

typedef List EntitiesList;

int EntityComparator(const void* a, const void* b);

Entity* CreateEntity(size_t chr_size);

void DestroyEntity(Entity* entity);

Entity* CopyEntity(Entity* entity, size_t chr_size);

EntitiesList* CreateEntitiesList();

static inline void DestroyEntitiesList(EntitiesList* entities) {
    destroyListPointer(entities);
}

void MarkAllAsNew(EntitiesList* entities);

void MarkAllAsOld(EntitiesList* entities);

List* NormalizeEntitiesFitnesses(EntitiesList* entities);

#define FOR_EACH_IN_ENTITIES_N(LIST_P, LIST_IT) \
    FOR_EACH_IN_LIST_N(LIST_P, LIST_IT)

#define FOR_EACH_IN_ENTITIES(LIST_P) \
    FOR_EACH_IN_LIST(LIST_P)

#define ENTITIES_IT_P_N(LIST_IT) \
    LIST_IT_VALUE_P_N(LIST_IT, Entity)

#define ENTITIES_IT_P \
    LIST_IT_VALUE_P(Entity)

#endif //EVOLUTION_ENTITY_H
