//
// Created by oleg on 14.12.17.
//

#include "PluginManager.h"

#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

#ifndef LIST_DEFINED_VOIDPTR
typedef void* VoidPtr;
DEFINE_LIST(VoidPtr)
#define LIST_DEFINED_VOIDPTR
#endif

LIST_TYPE(ConstObjectivePtr) g_plugin_objectives = NULL;
LIST_TYPE(ConstGAOperatorsPtr) g_plugin_operators = NULL;

const char* g_PM_last_error = NULL;

static LIST_TYPE(VoidPtr) g_library_handles = NULL;

static bool load_objective(const char* library_path);
static bool load_operator(const char* library_path);
static bool load_each_lib(const char* plugins_dir,
                          bool (*plugin_loader)(const char* library_path));

static bool verify_objective(const Objective* objective);
static bool verify_operators(const GAOperators* operators);

#define DEFINE_IS_LESS(TYPE) \
    static bool __is_less_##TYPE(TYPE* a, TYPE* b) { \
        return strcmp((*a)->name, (*b)->name) < 0; \
    }

#define is_less(TYPE) __is_less_##TYPE

DEFINE_IS_LESS(ConstObjectivePtr)
DEFINE_IS_LESS(ConstGAOperatorsPtr)

bool load_plugins(const char* objectives_dir,
                  const char* operators_dir) {
    g_plugin_objectives = list_create(ConstObjectivePtr);
    if (!g_plugin_objectives) {
        g_PM_last_error = "Failed to create g_plugin_objectives";
        goto error;
    }
    g_plugin_operators = list_create(ConstGAOperatorsPtr);
    if (!g_plugin_operators) {
        g_PM_last_error = "Failed to create g_plugin_operators";
        goto error;
    }
    g_library_handles = list_create(VoidPtr);
    if (!g_library_handles) {
        g_PM_last_error = "Failed to create g_library_handles";
        goto error;
    }

    if (!load_each_lib(objectives_dir, load_objective)) {
        goto error;
    }
    if (!load_each_lib(operators_dir, load_operator)) {
        goto error;
    }

    if (list_empty(g_plugin_objectives)
        || list_empty(g_plugin_operators)) {

        g_PM_last_error = "There are no plugins";
        goto error;
    }

    list_selection_sort_cmp(ConstObjectivePtr, g_plugin_objectives,
                            is_less(ConstObjectivePtr));
    list_selection_sort_cmp(ConstGAOperatorsPtr, g_plugin_operators,
                            is_less(ConstGAOperatorsPtr));

    return true;

error:
    unload_plugins();
    return false;
}

void unload_plugins() {
    if (g_plugin_objectives) {
        list_destroy(ConstObjectivePtr, g_plugin_objectives);
    }
    if (g_plugin_operators) {
        list_destroy(ConstGAOperatorsPtr, g_plugin_operators);
    }
    if (g_library_handles) {
        list_for_each(VoidPtr, g_library_handles, var) {
            dlclose(list_var_value(var));
        }
        list_destroy(VoidPtr, g_library_handles);
    }
}

static bool load_objective(const char* library_path) {
    void* lib_handle = dlopen(library_path, RTLD_LAZY);
    if (!lib_handle) {
        g_PM_last_error = dlerror();
        return false;
    }

    dlerror();

    const Objective* objective = (const Objective*)dlsym(lib_handle, "objective");
    g_PM_last_error = dlerror();
    if (g_PM_last_error) {
        goto destroy_lib_handle;
    }

    if (!verify_objective(objective)) {
        g_PM_last_error = "Objective is invalid";
        goto destroy_lib_handle;
    }

    if (!list_push_back(VoidPtr, g_library_handles, lib_handle)) {
        goto destroy_lib_handle;
    }
    if (!list_push_back(ConstObjectivePtr, g_plugin_objectives, objective)) {
        goto pop_lib_handle;
    }

    return true;

pop_lib_handle:
    list_pop_back(VoidPtr, g_library_handles);
destroy_lib_handle:
    dlclose(lib_handle);
    return false;
}

static bool load_operator(const char* library_path) {
    void* lib_handle = dlopen(library_path, RTLD_LAZY);
    if (!lib_handle) {
        g_PM_last_error = dlerror();
        return false;
    }

    dlerror();

    const GAOperators* operators = (const GAOperators*)dlsym(lib_handle, "operators");
    g_PM_last_error = dlerror();
    if (g_PM_last_error) {
        goto destroy_lib_handle;
    }

    if (!verify_operators(operators)) {
        g_PM_last_error = "Operators are invalid";
        goto destroy_lib_handle;
    }

    if (!list_push_back(VoidPtr, g_library_handles, lib_handle)) {
        goto destroy_lib_handle;
    }
    if (!list_push_back(ConstGAOperatorsPtr, g_plugin_operators, operators)) {
        goto pop_lib_handle;
    }

    return true;

pop_lib_handle:
    list_pop_back(VoidPtr, g_library_handles);
destroy_lib_handle:
    dlclose(lib_handle);
    return false;
}

static bool load_each_lib(const char* plugins_dir,
                          bool (*plugin_loader)(const char* library_path)) {
    DIR* dir = opendir(plugins_dir);
    if (!dir) {
        g_PM_last_error = "Failed to open plugins dir";
        return false;
    }
    char* buffer = NULL;
    struct dirent* entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        // Check that the file has .so extension
        char* so_pos = strstr(entry->d_name, ".so");
        if (so_pos && strlen(so_pos) == strlen(".so")) {
            size_t buffer_size = strlen(plugins_dir) + strlen(entry->d_name) + 2;
            buffer = (char*)malloc(sizeof(char) * buffer_size);
            if (!buffer) {
                goto close_dir;
            }
            if (snprintf(buffer, buffer_size, "%s/%s", plugins_dir, entry->d_name) < 0) {
                goto free_buffer;
            }
            if (!plugin_loader(buffer)) {
                goto free_buffer;
            }
            free(buffer);
        }
    }

    closedir(dir);
    return true;

free_buffer:
    free(buffer);
close_dir:
    closedir(dir);
    return false;
}

static bool verify_objective(const Objective* objective) {
    return objective->func && objective->name && strlen(objective->name) > 0;
}

static bool verify_operators(const GAOperators* operators) {
    return operators->name && strlen(operators->name) > 0;
}
