//
// Created by Oleg on 12/4/15.
//

#include "Wishart.h"

typedef struct graph {
    size_t n;
    short** edges;
} Graph;

static unsigned long factorial(unsigned long n) {
    unsigned long f = 1;
    for (unsigned long i = 2; i <= n; ++i) {
        f *= i;
    }
    return f;
}

static double _getC(size_t n) {
    static size_t c_n = 0;
    static double C = 0.0;
    if (n != c_n) {
        c_n = n;
        size_t k;
        if (n % 2 == 0) {
            k = n / 2;
            C = pow(M_PI, k) / factorial(k);
        }
        else {
            k = (n - 1) / 2;
            C = pow(2, k + 1) * pow(M_PI, k)
                    / factorial(factorial(2 * k + 1));
        }
    }
    return C;
}

static double _getV(double R, size_t n) {
    return _getC(n) * R;
}

static int _intCmp(const void* p1, const void* p2) {
    return *(int*)p1 - *(int*)p2;
}

static void _intDst(void* value) {
    free(value);
}

static int _doubleCmp(const void* p1, const void* p2) {
    double diff = *(double*)p1 - *(double*)p2;
    return (diff > 0) - (diff < 0);
}

static int _VectorCmp(const void* p1, const void* p2) {
    double diff = ((Vector*)p1)->kDistance
                  - ((Vector*)p2)->kDistance;
    return (diff > 0) - (diff < 0);
}

static int _ClusterCmp(const void* p1, const void* p2) {
    // To allow find cluster by number
    return (int)((Cluster*)p1)->number - *(int*)p2;
}

static void _ClusterDst(void* value) {
    free(value);
}

static Vectors* createVectors(const double* const* dVectors,
                              size_t vectors_count,
                              size_t vector_length) {
    size_t i;
    Vectors* vectors = (Vectors*)malloc(sizeof(Vectors));
    vectors->v = (Vector*)malloc(vectors_count * sizeof(Vector));
    for (i = 0; i < vectors_count; ++i) {
        vectors->v[i].x = dVectors[i];
        vectors->v[i].kDistance = 0.0;
        vectors->v[i].density = 0.0;
    }
    vectors->count = vectors_count;
    vectors->vector_length = vector_length;
    vectors->distances = (double**)malloc(vectors_count * sizeof(double*));
    for (i = 0; i < vectors_count; ++i) {
        vectors->distances[i] = (double*)calloc(vectors_count, sizeof(double));
    }
    return vectors;
}

static void releaseVectors(Vectors* vectors) {
    if (!vectors) {
        return;
    }
    if (vectors->distances) {
        for (size_t i = 0; i < vectors->count; ++i) {
            free(vectors->distances[i]);
        }
    }
    free(vectors->v);
    free(vectors->distances);
    free(vectors);
}

static void countDistances(Vectors* vectors) {
    size_t i, j, k;
    for (i = 0; i < vectors->count - 1; ++i) {
        for (j = i + 1; j < vectors->count; ++j) {
            vectors->distances[i][j] = 0;
            for (k = 0; k < vectors->vector_length; ++k) {
                vectors->distances[i][j] +=
                        pow(vectors->v[i].x[k] - vectors->v[j].x[k], 2);
            }
            vectors->distances[i][j] = sqrt(vectors->distances[i][j]);
            vectors->distances[j][i] = vectors->distances[i][j];
        }
    }
}

static void countKDistances(Vectors* vectors, size_t k) {
    double* point_distances;
    point_distances = (double*)malloc(vectors->count
                                      * sizeof(double));
    size_t i, j;
    for (i = 0; i < vectors->count; ++i) {
        for (j = 0; j < vectors->count; ++j) {
            point_distances[j] = vectors->distances[i][j];
        }
        qsort((void*)point_distances,
              vectors->count,
              sizeof(double),
              _doubleCmp);
        vectors->v[i].kDistance = point_distances[k];
    }
    free(point_distances);
}

static void countDensities(Vectors* vectors, size_t k) {
    for (size_t i = 0; i < vectors->count; ++i) {
        if (vectors->v[i].kDistance != 0.0) {
            vectors->v[i].density =
                    k / (_getV(vectors->v[i].kDistance,
                               vectors->vector_length)
                         * vectors->count);
        }
    }
}

static void sortVectorsByKDistance(Vectors* vectors) {
    qsort(vectors->v,
          vectors->count,
          sizeof(Vector),
          _VectorCmp);
    countDistances(vectors);
}


static Graph* _initGraph(size_t n) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->n = n;
    graph->edges = (short**)malloc(n * sizeof(short*));
    size_t i;
    for (i = 0; i < n; ++i) {
        graph->edges[i] = (short*)calloc(n, sizeof(short));
    }
    return graph;
}

static void releaseGraph(Graph* graph) {
    for (size_t i = 0; i < graph->n; ++i) {
        free(graph->edges[i]);
    }
    free(graph->edges);
    free(graph);
}

static Graph* buildGraph(Vectors* vectors) {
    Graph* graph = _initGraph(vectors->count);
    size_t i, j;
    for (i = 0; i < vectors->count; ++i) {
        for (j = 0; j < vectors->count; ++j) {
            if (i != j
                && vectors->distances[i][j] <= vectors->v[i].kDistance) {
                graph->edges[i][j] = 1;
            }
        }
    }
    return graph;
}

static Graph buildSubGraph(Graph* graph, size_t n) {
    if (n >= graph->n) {
        return *graph;
    }
    Graph sub_graph = *graph;
    sub_graph.n = n;
    return sub_graph;
}

static List getConnectedClustersNumbers(Graph* graph,
                                        size_t vector_index,
                                        size_t* w) {
    List clusters_numbers;  // List of size_t
    initList(&clusters_numbers, _intCmp, _intDst);
    ListIterator it;
    size_t* temp;
    for (size_t i = 0; i < graph->n; ++i) {
        if (graph->edges[vector_index][i]) {
            it = findByVal(&clusters_numbers, w + i);
            if (!it.current) {
                temp = (size_t*)malloc(sizeof(size_t));
                *temp = w[i];
                pushBack(&clusters_numbers, temp);
            }
        }
    }
    return clusters_numbers;
}

//double getClusterHeight(Vectors* vectors,
//                        size_t cluster_number,
//                        size_t* w) {
//    long i_max = -1, i_min = -1;
//    for (size_t i = 0; i < vectors->count; ++i) {
//        if (w[i] == cluster_number) {
//            if (i_max == -1
//                || vectors->v[i].density > vectors->v[i_max].density) {
//                i_max = i;
//            }
//            if (i_min == -1
//                || vectors->v[i].density < vectors->v[i_min].density) {
//                i_min = i;
//            }
//        }
//    }
//    if (i_max == -1 || i_min == -1) {  // TODO: error handling
//        return 0.0;
//    }
//    return fabs(vectors->v[i_max].density - vectors->v[i_min].density);
//}

short isSignificantCluster(Vectors* vectors,
                           size_t cluster_number,
                           size_t* w, double h) {
    for (size_t i = 0; i < vectors->count - 1; ++i) {
        for (size_t j = i + 1; j < vectors->count; ++j) {
            if (w[i] == cluster_number && w[j] == cluster_number
                && fabs(vectors->v[i].density - vectors->v[j].density) >= h) {
                return 1;
            }
        }
    }
    return 0;
}

size_t* Wishart(const double* const* vectors,
                size_t vectors_count,
                size_t vector_length,
                size_t k,
                double h) {
    size_t i, j;

    Vectors* wVectors = createVectors(vectors,
                                      vectors_count,
                                      vector_length);
    // step 1
    countDistances(wVectors);
    countKDistances(wVectors, k);
    countDensities(wVectors, k);

    sortVectorsByKDistance(wVectors);

    Graph* graph_n = buildGraph(wVectors);
    // end of step 1

    // step 2
    // this buffer is returned, and must be freed by calling side
    size_t* w = (size_t*)calloc(vectors_count, sizeof(size_t));
    // end of step 2

    size_t* connected_clusters_numbers;
    size_t* temp;
    size_t all_clusters_formed;
    size_t next_cluster_number = 1;
    Graph graph_i;
    List clusters_numbers;  // List of size_t
    List significant_clusters_numbers;  // List of size_t
    initList(&significant_clusters_numbers, _intCmp, _intDst);

    List Clusters;  // List of Cluster
    initList(&Clusters, _ClusterCmp, _ClusterDst);  // See weird _ClusterCmp
    Cluster* new_cluster;

    ListIterator it, it2;

    for (size_t iter = 0; iter < vectors_count; ++iter) {  // step 4

        // step 3
        graph_i = buildSubGraph(graph_n, iter + 1);

        clusters_numbers = getConnectedClustersNumbers(&graph_i, iter, w);

        // step 3.1
        if (clusters_numbers.length == 0) {
            new_cluster = (Cluster*)malloc(sizeof(Cluster));
            new_cluster->number = next_cluster_number++;
            new_cluster->formed = 0;

            pushBack(&Clusters, new_cluster);

            w[iter] = new_cluster->number;
        }
        // end of step 3.1

        // step 3.2
        else if (clusters_numbers.length == 1) {
            // find cluster in Clusters list by number in clusters_numbers
            it = findByVal(&Clusters,
                           begin(&clusters_numbers).current->value);

            // step 3.2.1
            if (it.current && ((Cluster*)it.current->value)->formed) {
                w[iter] = 0;
            }
            // end of step 3.2.1

            // step 3.2.2
            else if (it.current) {
                w[iter] = ((Cluster*)it.current->value)->number;
            }
            // if !it.current then do nothing (zero class)
            // end of step 3.2.2

        }
        // end of step 3.2

        // step 3.3
        else {
            connected_clusters_numbers =
                    (size_t*)malloc(clusters_numbers.length * sizeof(size_t));
            for (it = begin(&clusters_numbers), i = 0;
                 !isIteratorAtEnd(it);
                 next(&it), ++i) {
                connected_clusters_numbers[i] = *(size_t*)it.current->value;
            }
            qsort(connected_clusters_numbers,
                  clusters_numbers.length,
                  sizeof(size_t),
                  _intCmp);

            all_clusters_formed = 1;
            for (i = 0; i < clusters_numbers.length; ++i) {
                it = findByVal(&Clusters,
                               connected_clusters_numbers + i);
                if (!it.current
                    || !((Cluster*)it.current->value)->formed) {
                    all_clusters_formed = 0;
                    break;
                }
            }

            // step 3.3.1
            if (all_clusters_formed) {
                w[iter] = 0;
            }
            // end of step 3.3.1

            //step 3.3.2
            else {
                for (i = 0; i < clusters_numbers.length; ++i) {
                    if (connected_clusters_numbers[i]
                        && isSignificantCluster(wVectors,
                                                connected_clusters_numbers[i],
                                                w, h)) {
                        temp = (size_t*)malloc(sizeof(size_t));
                        *temp = connected_clusters_numbers[i];
                        pushBack(&significant_clusters_numbers, temp);
                    }
                }

                // step 3.3.2.1
                if (significant_clusters_numbers.length > 1
                    || connected_clusters_numbers[0] == 0) {
                    w[iter] = 0;
                    for (it = begin(&significant_clusters_numbers);
                         !isIteratorAtEnd(it);
                         next(&it)) {
                        it2 = findByVal(&Clusters, it.current->value);
                        if (it2.current) {  // TODO: error handling
                            ((Cluster*)it2.current->value)->formed = 1;
                        }
                    }
                    for (i = 0; i < wVectors->count; ++i) {
                        if (w[i] == 0) {
                            continue;
                        }
                        it = findByVal(&significant_clusters_numbers, w + i);
                        if (!it.current) {
                            removeByVal(&Clusters, w + i);
                            w[i] = 0;
                        }
                    }
                }
                // end of step 3.3.2.1

                // step 3.3.2.2
                else {
                    w[iter] = connected_clusters_numbers[0];
                    for (i = 0; i < iter; ++i) {  // TODO: check bounds
                        for (j = 1; j < clusters_numbers.length; ++j) {
                            if (w[i] == connected_clusters_numbers[j]) {
                                w[i] = connected_clusters_numbers[0];
                                break;
                            }
                        }
                    }
                    for (i = 1; i < clusters_numbers.length; ++i) {
                        removeByVal(&Clusters, connected_clusters_numbers + i);
                    }
                }
                // end of step 3.3.2.2

                clearList(&significant_clusters_numbers);
            }
            // end of step 3.3.2

            free(connected_clusters_numbers);
        }
        // end of step 3.3

        clearList(&clusters_numbers);
        // end of step 3

    }

    releaseGraph(graph_n);
    releaseVectors(wVectors);
    clearList(&Clusters);

    return w;
}