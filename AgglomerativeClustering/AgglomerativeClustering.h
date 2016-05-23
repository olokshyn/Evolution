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

#define MAX_CLUSTER_COUNT 8

List* AgglomerativeClustering(List* clusters,
                              List* entities,
                              size_t vector_length,
                              double h);

#ifdef __cplusplus
}
#endif

#endif //WISHART_AGGLOMERATIVECLUSTERING_H
