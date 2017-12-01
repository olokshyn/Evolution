//
// Created by Oleg on 5/22/16.
//

#ifndef EVOLUTION_ENTITY_H
#define EVOLUTION_ENTITY_H

#include <stdbool.h>

#include "3rdParty/CList/list.h"

typedef struct entity {
    double* chr;
    double fitness;
    bool old;
} Entity, *EntityPtr;

#ifndef LIST_DEFINED_ENTITYPTR
DEFINE_LIST(EntityPtr)
#define LIST_DEFINED_ENTITYPTR
#endif

#ifndef LIST_DEFINED_DOUBLE
DEFINE_LIST(double)
#define LIST_DEFINED_DOUBLE
#endif

int EntityAscendingComparator(const void* a, const void* b);

int EntityDescendingComparator(const void* a, const void* b);

Entity* CreateEntity(size_t chr_size);

void DestroyEntity(Entity* entity);

Entity* CopyEntity(const Entity* entity, size_t chr_size);

bool ClearEntitiesList(LIST_TYPE(EntityPtr) entities);

void DestroyEntitiesList(LIST_TYPE(EntityPtr) entities);

void SetEntitiesStatus(LIST_TYPE(EntityPtr) entities, bool old);

void GetFitnesses(LIST_TYPE(EntityPtr) entities,
                  double* mid_fitness,
                  double* min_fitness,
                  double* max_fitness);

LIST_TYPE(double) NormalizeEntitiesFitnesses(LIST_TYPE(EntityPtr) entities);

Entity** SortedEntitiesPointers(LIST_TYPE(EntityPtr) entities,
                                int (*comparator)(const void*, const void*));

#endif //EVOLUTION_ENTITY_H
