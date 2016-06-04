//
// Created by Oleg on 6/2/16.
//

#include "DeathManager.h"

#include <stdio.h>


static FILE* death_file = NULL;
static size_t died_on_iteration = 0;

int InitDeathManager(const char* death_manager_file_name) {
    if (!death_file) {
        ReleaseDeathManager();
    }
    death_file = fopen(death_manager_file_name, "w");
    if (!death_file) {
        return 0;
    }
    died_on_iteration = 0;
    return 1;
}

void ReleaseDeathManager() {
    if (death_file) {
        fclose(death_file);
    }
}

void IterationStart() {
    died_on_iteration = 0;
}

void IterationEnd() {
    fprintf(death_file, "%zu\n", died_on_iteration);
    died_on_iteration = 0;
    fflush(death_file);
}

void RegisterDeath() {
    ++died_on_iteration;
}