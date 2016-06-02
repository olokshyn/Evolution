//
// Created by Oleg on 6/2/16.
//

#ifndef WISHART_DEATHMANAGER_H
#define WISHART_DEATHMANAGER_H

#include <stddef.h>

int InitDeathManager(const char* death_manager_file_name);

void ReleaseDeathManager();

void IterationStart();

void IterationEnd();

void RegisterDeath();

#endif //WISHART_DEATHMANAGER_H
