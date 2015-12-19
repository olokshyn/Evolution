//
// Created by Oleg on 12/14/15.
//

#include "WishartTest.h"


static double getDoubleFromSubString(char* begin, char* end) {
    char* buff = (char*)malloc(end - begin + 1);
    char* b = buff;
    while (begin < end) {
        *b++ = *begin++;
    }
    *b = '\0';
    double res = atof(buff);
    free(buff);
    return res;
}

void small_test() {
    size_t i, vectors_count = 4, vector_length = 2;
    double** vectors = (double**)malloc(vectors_count * sizeof(double*));
    for (i = 0; i < vectors_count; ++i) {
        vectors[i] = (double*)calloc(vector_length, sizeof(double));
    }

    double** v = vectors;

    v[0][0] = 1.0;
    v[0][1] = 1.0;

    v[1][0] = 0.8;
    v[1][1] = 0.8;

    v[2][0] = 0.0;
    v[2][1] = 0.0;

    v[3][0] = 0.1;
    v[3][1] = 0.1;

    size_t* w = Wishart((const double* const*)v, vectors_count, vector_length, 1, 0.5);

    for (i = 0; i < vectors_count; ++i) {
        printf("%zu%c", w[i], (i < vectors_count - 1) ? ' ' : '\n');
    }

    free(w);
}

void fisher_iris_test() {
    size_t i, vectors_count = 150, vector_length = 4;

    FILE* file = fopen("Fisher_iris.data", "r");
    if (!file) {
        return;
    }

    double** vectors = (double**)malloc(vectors_count * sizeof(double*));
    for (i = 0; i < vectors_count; ++i) {
        vectors[i] = (double*)calloc(vector_length + 1, sizeof(double));
    }

    size_t vector_index = 0;
    char buffer[32], *begin, *end;
    while (fgets(buffer, 32, file)) {
        begin = buffer;
        end = strchr(begin, '\t');
        for (i = 0; i < vector_length; ++i) {
            if (!end) {
                break;
            }
            vectors[vector_index][i] = getDoubleFromSubString(begin, end);
            begin = end + 1;
            end = strchr(begin, '\t');
        }
        vectors[vector_index][vector_length] =
                getDoubleFromSubString(begin, begin + 1);
        ++vector_index;
    }

    size_t* w = Wishart((const double* const*)vectors,
                        vectors_count,
                        vector_length,
                        1,
                        0.5);

    size_t right = 1;
    size_t cluster_number = w[0];
    for (i = 1; right && i < 50; ++i) {
        if (cluster_number != w[i]) {
            right = 0;
            printf("ERROR on vector %d\n", (int)i + 1);
        }
    }
    if (right && cluster_number == w[50]) {
        right = 0;
        printf("Error on vector %d\n", 51);
    }
    cluster_number = w[50];
    for (i = 51; right && i < 100; ++i) {
        if (cluster_number != w[i]) {
            right = 0;
            printf("Error on vector %d\n", (int)i + 1);
        }
    }
    if (right && cluster_number ==  w[100]) {
        right = 0;
        printf("Error on vector %d\n", 101);
    }
    cluster_number = w[100];
    for (i = 101; right && i < 150; ++i) {
        if (cluster_number != w[i]) {
            right = 0;
            printf("Error on vector %d\n", (int)i + 1);
        }
    }

    if (right) {
        printf("SUCCESS\n");
    }
    else {
        for (i = 0; i < 150; ++i) {
            printf("%3d %d\n", (int)i + 1, (int)w[i]);
        }
        printf("\n");
    }

    free(w);
    for (i = 0; i < vectors_count; ++i) {
        free(vectors[i]);
    }
    free(vectors);
}