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

void four_points_test() {
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

    size_t* w = Wishart((const double* const*)v,
                        vectors_count,
                        vector_length,
                        1,
                        0.5);

    for (i = 0; i < vectors_count; ++i) {
        printf("%zu%c", w[i], (i < vectors_count - 1) ? ' ' : '\n');
    }

    free(w);
}

void eight_points_test() {
    size_t i, vectors_count = 8, vector_length = 2;
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

    v[4][0] = 0.4;
    v[4][1] = 0.1;

    v[5][0] = 0.1;
    v[5][1] = 0.25;

    v[6][0] = 0.9;
    v[6][1] = 0.8;

    v[7][0] = 0.8;
    v[7][1] = 1.0;

    size_t* w = Wishart((const double* const*)v,
                        vectors_count,
                        vector_length,
                        1,
                        0.001);

    for (i = 0; i < vectors_count; ++i) {
        printf("%zu%c", w[i], (i < vectors_count - 1) ? ' ' : '\n');
    }

    free(w);
}

void twelve_points_test() {
    size_t i, vectors_count = 12, vector_length = 2;
    double** vectors = (double**)malloc(vectors_count * sizeof(double*));
    for (i = 0; i < vectors_count; ++i) {
        vectors[i] = (double*)calloc(vector_length, sizeof(double));
    }

    double** v = vectors;

    v[0][0] = 10;
    v[0][1] = 10;

    v[1][0] = 8;
    v[1][1] = 8;

    v[2][0] = 0;
    v[2][1] = 0;

    v[3][0] = 1;
    v[3][1] = 1;

    v[4][0] = 4;
    v[4][1] = 1;

    v[5][0] = 1;
    v[5][1] = 2.5;

    v[6][0] = 9;
    v[6][1] = 8;

    v[7][0] = 8;
    v[7][1] = 10;

    v[8][0] = 6;
    v[8][1] = 4.5;

    v[9][0] = 2;
    v[9][1] = 10;

    v[10][0] = 4;
    v[10][1] = 4;

    v[11][0] = 10;
    v[11][1] = 6;

    size_t* w = Wishart((const double* const*)v,
                        vectors_count,
                        vector_length,
                        1,
                        0.1);

    for (i = 0; i < vectors_count; ++i) {
        printf("%zu%c", w[i], (i < vectors_count - 1) ? ' ' : '\n');
    }

    free(w);
}

void fisher_iris_test() {
    size_t i, vectors_count = 150, vector_length = 4;

    char temp_buf[128];
    sprintf(temp_buf, "../TestInputs/%zu_fisher_iris.data", vectors_count);
    FILE* file = fopen(temp_buf, "r");
    if (!file) {
        printf("File not found!\n");
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
                        2,
                        0.01334);

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
        for (i = 0; i < vectors_count; ++i) {
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