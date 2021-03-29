/*****************************************************
 * HEADER FILE: hn_macro_utils.h                     *
 *                                                   *
 * FUNCTION: Utility macros                          *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 18/07/2016                               *
 *                                                   *
 *****************************************************/


#ifndef HN_MACRO_UTILS_H
#define HN_MACRO_UTILS_H

#include "debug_log.h"


/* Intuitive restatement of !strcmp that yields a boolean */
#define StringsAreEqual(str1, str2)  !strcmp(str1, str2)


/* Self-explanatory. As usual, beware of side-effects,
 * as both macros use both arguments twice */
#define Max(a, b)   ((a) < (b) ? (b) : (a))
#define Min(a, b)   ((a) < (b) ? (a) : (b))


/* The activation function we use */
#define Sign(x)     ((x) >= 0 ? +1 : -1)


/* Extract a random integer 0 <= n <= size-1
 * (approximately uniform if size << RAND_MAX) */
#define RandI(size)     (rand() / (1 + RAND_MAX / (size)))


/*
 * The following are polymorphic macros that allocate and free 2D arrays.
 * Based on make_matrix() and matrix_free() found in
 *
 *      R. Rostamian, "Programming Projects in C", SIAM.
 */

#define MatrixAlloc(mat, max_rows, max_cols)                            \
    do {                                                                \
        size_t MATRIX_ALLOC_I;                                          \
        (mat) = malloc(((max_rows)+1) * sizeof (*(mat)));               \
        KillUnless(NULL != (mat));                                      \
        (mat)[(max_rows)] = NULL; /* Terminal element */                \
        for (MATRIX_ALLOC_I = 0; MATRIX_ALLOC_I < (max_rows); ++MATRIX_ALLOC_I) { \
            (mat)[MATRIX_ALLOC_I] = malloc((max_cols) * sizeof (**(mat))); \
            KillUnless(NULL != (mat)[MATRIX_ALLOC_I]);                  \
        }                                                               \
    } while(0)


/* This is in case we want to fill the matrix with zeros. */
#define MatrixZeros(mat, max_rows, max_cols)                            \
    do {                                                                \
        size_t MATRIX_ALLOC_I;                                          \
        (mat) = malloc(((max_rows) + 1) * sizeof (*(mat)));             \
        KillUnless((mat) != NULL);                                      \
        (mat)[(max_rows)] = NULL; /* Terminal element */                \
        for (MATRIX_ALLOC_I = 0; MATRIX_ALLOC_I < (max_rows); ++MATRIX_ALLOC_I) { \
            (mat)[MATRIX_ALLOC_I] = malloc((max_cols) * sizeof (**(mat))); \
            KillUnless(NULL != (mat)[MATRIX_ALLOC_I]);                  \
            memset((mat)[MATRIX_ALLOC_I], 0, (max_cols) * sizeof(**(mat))); \
        }                                                               \
    } while(0)


#define MatrixFree(mat)                                                 \
    do {                                                                \
        size_t MATRIX_FREE_I = 0;                                       \
        while ((mat)[MATRIX_FREE_I] != NULL) {                          \
            free((mat)[MATRIX_FREE_I++]);                               \
        }                                                               \
        free((mat));                                                    \
    } while(0)


#endif /* HN_MACRO_UTILS_H */
