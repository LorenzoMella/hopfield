/*****************************************************
 * C FILE: hn_data_io.c                              *
 * MODULE: Data I/O, encoding/decoding               *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 19/07/2016                               *
 *                                                   *
 *****************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "hn_types.h"
#include "hn_data_io.h"
#include "debug_log.h"


/* Returns the file length in bytes (it works only for files that have been
 * just opened, i.e., the FILE structure points at the beginning) */
static size_t file_byte_length(FILE *fp);


io_error_code hn_read_weights(double **weights, char *w_filename,
                              size_t max_units)
{
    size_t i;
    FILE *w_fp;
    
    debug_log("w_filename = \"%s\"\n", w_filename);
    
    w_fp = fopen(w_filename, "r");
    if (w_fp == NULL) {
        fprintf(stderr, "%s, line %d: fopen error\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    /* Check whether the file could actually hold
     * a weight matrix of the specified dimension */
    debug_log("file length in bytes = %lu\n", file_byte_length(w_fp));

    if (file_byte_length(w_fp) != sizeof (double) * max_units * max_units) {
        fprintf(stderr, "%s, line %d: File dimension not matching request\n",
                __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    /* At this point it is clear that the file holds enough elements
     * and we can safely read max_units lines of max_units elements each */
    for (i = 0; i < max_units; ++i) {
        fread(weights[i], sizeof (double), max_units, w_fp);
    }
    
    fclose(w_fp);
    debug_log("hn_read_weights got to IO_SUCCESS\n");
    return IO_SUCCESS;
}


io_error_code hn_read_next_pattern(spike_T *pattern, char *p_filename,
                                   size_t max_units)
{
    static long pattern_position = 0L;
    size_t file_length;
    
    FILE *p_fp = fopen(p_filename, "r");
    if (p_fp == NULL) {
        fprintf(stderr, "%s, line %d: fopen error\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    file_length = file_byte_length(p_fp);
    
    /* The position of the next pattern after previous calls is saved
     * in the static offset pattern_position; shift there */
    if (fseek(p_fp, pattern_position, SEEK_SET) != 0) {
        fprintf(stderr, "%s, line %d: fseek error\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    /* If the file is over, return failure */
    if (ftell(p_fp) == file_length) {
        fprintf(stderr, "%s, line %d: no more sequences\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    debug_log("ftell after repositioning: %ld; equals "
              "pattern_position? %s\n", ftell(p_fp),
              ftell(p_fp) == pattern_position ? "YES" : "NO");
    
    /* fread return the number of objects successfully read:
     * check if it's exactly the size (in spike_Ts) of a pattern */
    if (fread(pattern, sizeof (spike_T), max_units, p_fp) != max_units) {
        fprintf(stderr, "%s, line %d: fread error\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    /* The new offset after reading max_unit spike_T objects */
    pattern_position += max_units * sizeof (spike_T);
    
    fclose(p_fp);
    debug_log("hn_read_next_pattern got to IO_SUCCESS\n");
    return IO_SUCCESS;
}


io_error_code hn_save(double *output, char *s_filename, size_t output_length)
{
    FILE *s_fp = fopen(s_filename, "w");
    if (s_fp == NULL) {
        fprintf(stderr, "%s, line %d: Memory error\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    if (fwrite(output, sizeof (double), output_length, s_fp) != output_length) {
        fprintf(stderr, "%s, line %d: fwrite error\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    fclose(s_fp);
    debug_log("hn_save got to IO_SUCCESS\n");
    return IO_SUCCESS;
}


/* THE FOLLOWING 2 HAVEN'T BEEN TESTED!! */
io_error_code hn_save_weights(double **weights, char *w_filename,
                              size_t max_units)
{
    size_t i;
    FILE *w_fp = NULL;
    if ((w_fp = fopen(w_filename, "w")) == NULL) {
        fprintf(stderr, "%s, line %d: Memory error\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    for (i = 0; i < max_units; ++i) {
        if (fwrite(weights[i], sizeof (double), max_units, w_fp) !=
            max_units) {
            fprintf(stderr, "%s, line %d: fwrite fails at matrix row %lu\n",
                    __FILE__, __LINE__, i);
            return IO_FAILURE;
        }
    }
    
    fclose(w_fp);
    debug_log("hn_save_weights got to IO_SUCCESS\n");
    return IO_SUCCESS;
}


io_error_code hn_save_next_pattern(spike_T *pattern, char *p_filename,
                                   size_t max_units)
{
    FILE *p_fp = NULL;
    /* We only allow to append for simplicity. An argument
     * (e.g, an enumeration for safety) may allow for file overwriting */
    if ((p_fp = fopen(p_filename, "a")) == NULL) {
        fprintf(stderr, "%s, line %d: Memory error\n", __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    if (fwrite(pattern, sizeof (double), max_units, p_fp) != max_units) {
        fprintf(stderr, "%s, line %d: fwrite fails at writing pattern\n",
                __FILE__, __LINE__);
        return IO_FAILURE;
    }
    
    fclose(p_fp);
    debug_log("hn_save_next_pattern got to IO_SUCCESS\n");
    return IO_SUCCESS;
}


void hn_fill_rand_pattern(spike_T *pattern, double coding_level,
                          size_t max_units)
{
    size_t i;
    for (i = 0; i < max_units; ++i) {
        pattern[i] = ((double)rand() / RAND_MAX) < coding_level ? +1 : -1;
    }
}


static size_t file_byte_length(FILE *fp)
{
    size_t filesize;
    exit_on_exception(fseek(fp, 0L, SEEK_END) == 0);
    filesize = ftell(fp);
    rewind(fp);
    return filesize;
}
