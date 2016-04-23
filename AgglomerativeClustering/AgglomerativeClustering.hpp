//
// Created by Oleg on 3/13/16.
//

#ifndef WISHART_AGGLOMERATIVECLUSTERING_H
#define WISHART_AGGLOMERATIVECLUSTERING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "../List/List.h"

List* AgglomerativeClustering(const double* const* vectors,
                              size_t vectors_count,
                              size_t vector_length,
                              size_t k,
                              double h);

#ifdef __cplusplus
}
#endif

#endif //WISHART_AGGLOMERATIVECLUSTERING_H
