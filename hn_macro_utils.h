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

/* Self-explanatory. Both macros use both arguments twice, so beware
 * of side-effects! */
#define MAX(a, b)   ((a) < (b) ? (b) : (a))
#define MIN(a, b)   ((a) < (b) ? (a) : (b))


/* The activation function we use */
#define SIGN(x)     ((x) >= 0 ? +1 : -1)


/* Extract a random integer 0 <= n <= size-1
 * (approximately uniform if size << RAND_MAX) */
#define RANDI(size)     (rand() / (1 + RAND_MAX / (size)))


/*
 * The following hn_matrix_alloc, hn_matrix_zeros and hn_matrix_free
 * are polymorphic macros that allocate and free 2D arrays.
 * Used to de-clutter notation.
 * Based on make_matrix() and matrix_free() found in
 *
 *      R. Rostamian, "Programming Projects in C", SIAM.
 */

#define hn_matrix_alloc(mat, max_rows, max_cols)                               \
do {                                                                           \
    size_t MATRIX_ALLOC_I;                                                     \
    (mat) = malloc(((max_rows)+1) * sizeof (*(mat)));                          \
    exit_on_exception(NULL != (mat));                                          \
    (mat)[(max_rows)] = NULL; /* This allows for easy deallocation */          \
    for (MATRIX_ALLOC_I = 0; MATRIX_ALLOC_I < (max_rows); ++MATRIX_ALLOC_I) {  \
        (mat)[MATRIX_ALLOC_I] = malloc((max_cols) * sizeof (**(mat)));         \
        exit_on_exception(NULL != (mat)[MATRIX_ALLOC_I]);                      \
    }                                                                          \
} while(0)


/* This is in case we want to fill the matrix with zeros. */
#define hn_matrix_zeros(mat, max_rows, max_cols)                               \
do {                                                                           \
    size_t MATRIX_ALLOC_I, MATRIX_ALLOC_J;                                     \
    (mat) = malloc(((max_rows)+1) * sizeof (*(mat)));                          \
    exit_on_exception(NULL != (mat));                                          \
    (mat)[(max_rows)] = NULL; /* This allows for easy deallocation */          \
    for (MATRIX_ALLOC_I=0; MATRIX_ALLOC_I<(max_rows); ++MATRIX_ALLOC_I) {      \
        (mat)[MATRIX_ALLOC_I] = malloc((max_cols) * sizeof (**(mat)));         \
        exit_on_exception(NULL != (mat)[MATRIX_ALLOC_I]);                      \
        for (MATRIX_ALLOC_J=0; MATRIX_ALLOC_J<(max_cols); ++MATRIX_ALLOC_J) {  \
            /* The 0 will be cast into the appropriate numeric type */         \
            (mat)[MATRIX_ALLOC_I][MATRIX_ALLOC_J] = 0;                         \
        }                                                                      \
    }                                                                          \
} while(0)


#define hn_matrix_free(mat)                                                    \
do {                                                                           \
    size_t MATRIX_FREE_I = 0;                                                  \
    while ((mat)[MATRIX_FREE_I] != NULL) {                                     \
        free((mat)[MATRIX_FREE_I++]);                                          \
    }                                                                          \
    free((mat));                                                               \
} while(0)


#endif /* HN_MACRO_UTILS_H */
