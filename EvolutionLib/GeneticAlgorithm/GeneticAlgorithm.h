#ifndef GENETICALGORITHMHEADER
#define GENETICALGORITHMHEADER

#include <stddef.h>

#include "GAFwd.h"
#include "Common.h"

typedef struct ga_result {
    double optimum;
    size_t iterations_made;
    double time_spent_per_iteration;
    int error;
} GAResult;

GAResult RunEvolution(const GAParameters* parameters,
                      const GAOperators* operators);

#endif
