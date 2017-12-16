//
// Created by oleg on 14.12.17.
//

#ifndef EVOLUTION_PLUGINMANAGER_H
#define EVOLUTION_PLUGINMANAGER_H

#include <stdbool.h>

#include "3rdParty/CList/list.h"

#include "GeneticAlgorithm/GAObjective.h"
#include "GeneticAlgorithm/GAOperators.h"

#ifndef LIST_DEFINED_CONSTOBJECTIVEPTR
typedef const Objective* ConstObjectivePtr;
DEFINE_LIST(ConstObjectivePtr)
#define LIST_DEFINED_CONSTOBJECTIVEPTR
#endif

#ifndef LIST_DEFINED_CONSTGAOPERATORSPTR
typedef const GAOperators* ConstGAOperatorsPtr;
DEFINE_LIST(ConstGAOperatorsPtr)
#define LIST_DEFINED_CONSTGAOPERATORSPTR
#endif

extern LIST_TYPE(ConstObjectivePtr) g_plugin_objectives;
extern LIST_TYPE(ConstGAOperatorsPtr) g_plugin_operators;

extern const char* g_PM_last_error;

bool load_plugins(const char* objectives_dir,
                  const char* operators_dir);

void unload_plugins();

#endif //EVOLUTION_PLUGINMANAGER_H
