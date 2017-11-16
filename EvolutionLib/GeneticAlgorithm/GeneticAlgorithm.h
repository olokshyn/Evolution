#ifndef GENETICALGORITHMHEADER
#define GENETICALGORITHMHEADER

#include <stddef.h>
#include <stdbool.h>

#include "GAFwd.h"
#include "Common.h"

typedef struct ga_result {
    double optimum;
    size_t iterations_made;
    double time_spent_per_iteration;
    bool error;
} GAResult;

GAResult RunEvolution(const GAParameters* parameters,
                      const GAOperators* operators);

#endif
