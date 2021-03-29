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


#include "debug_log.h"
#include "hn_data_io.h"
#include "hn_types.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Return the file length in bytes or -1 in case of failure
 * (one could call 'wc', or use 'stat', but this is more portable)
 */
static long file_byte_length(FILE *fp)
{
    /* Save current stream position */
    long current_position = ftell(fp);
    /* Check success of previous ftell; go to end-file position;
     * check if successful */
    if (current_position == -1L) {
	fprintf(stderr, "%s - ftell failed: %s\n", __func__, strerror(errno));
        errno = 0;
	return current_position;
    }
    if (fseek(fp, 0L, SEEK_END) != 0) {
	fprintf(stderr, "%s - fseek failed: %s\n", __func__, strerror(errno));
        errno = 0;
	return -1L;
    }
    /* Of course, the end-position is the size! */
    long size = ftell(fp);
    if (size == -1L) {
	fprintf(stderr, "%s - ftell failed: %s\n", __func__, strerror(errno));
        errno = 0;
	return size;
    }
    /* Restore the original position */
    if (fseek(fp, current_position, SEEK_SET) != 0) {
	fprintf(stderr, "%s - fseek failed: %s\n", __func__, strerror(errno));
        errno = 0;
	return -1L;
    }
    
    return size;
}


enum io_error_code hn_read_weights(double **weights, char *w_filename,
				   size_t max_units)
{
    Logger("w_filename = \"%s\"\n", w_filename);
    
    FILE *w_fp = fopen(w_filename, "r");
    if (w_fp == NULL) {
        perror(__func__);
        return IOFailure;
    }
    
    /* Check whether the file could actually hold
     * a weight matrix of the specified dimension */
    Logger("file length in bytes = %lu\n", file_byte_length(w_fp));

    if (file_byte_length(w_fp) != sizeof (double) * max_units * max_units) {
        fprintf(stderr, "%s - File dimension not matching request\n", __func__);
        errno = 0;
	fclose(w_fp);
        return IOFailure;
    }
    
    /* At this point it is clear that the file holds enough elements
     * and we can safely read max_units lines of max_units elements each */
    for (size_t i = 0; i < max_units; ++i) {
        size_t items_read = fread(weights[i], sizeof (double), max_units, w_fp);
	if (items_read < max_units) {

            Logger("%s: error at iteration %zu\n", __func__, i);

            if (!feof(w_fp)) {
                perror(__func__);
                errno = 0;
            }
            
	    fclose(w_fp);

            return IOFailure;
	}
    }
    
    fclose(w_fp);

    Logger("hn_read_weights got to IOSuccess\n");

    return IOSuccess;
}


enum io_error_code hn_read_next_pattern(spike_T *pattern, char *p_filename,
					size_t max_units)
{
    static long pattern_position = 0L;
    
    FILE *p_fp = fopen(p_filename, "r");
    if (p_fp == NULL) {
        fprintf(stderr, "%s, line %d: fopen error: %s\n", __FILE__, __LINE__,
		strerror(errno));
        errno = 0;
        return IOFailure;
    }
    
    long file_length = file_byte_length(p_fp);
    
    /* The position of the next pattern after previous calls is saved
     * in the static offset pattern_position; shift there */
    if (fseek(p_fp, pattern_position, SEEK_SET) != 0) {
        fprintf(stderr, "%s, line %d: fseek error: %s\n", __FILE__, __LINE__,
		strerror(errno));
        errno = 0;
        return IOFailure;
    }
    
    /* If no more sequences can be extracted, return failure */
    if (ftell(p_fp) == file_length) {
        fprintf(stderr, "%s, line %d: no more sequences\n", __FILE__, __LINE__);
        return IOFailure;
    }

    Logger("ftell after repositioning: %ld; equals "
              "pattern_position? %s\n", ftell(p_fp),
              ftell(p_fp) == pattern_position ? "YES" : "NO");
    
    /* fread return the number of objects successfully read:
     * check if it's exactly the size (in spike_Ts) of a pattern */
    if (fread(pattern, sizeof (spike_T), max_units, p_fp) != max_units) {
        fprintf(stderr, "%s, line %d: fread error: %s\n", __FILE__, __LINE__,
		strerror(errno));
        errno = 0;
        return IOFailure;
    }
    
    /* The new offset after reading max_unit spike_T objects */
    pattern_position += max_units * sizeof (spike_T);
    
    fclose(p_fp);
    
    Logger("hn_read_next_pattern got to IOSuccess\n");

    return IOSuccess;
}


enum io_error_code hn_save(double *output, char *s_filename,
			   size_t output_length, size_t *bytes_written)
{
    FILE *s_fp = fopen(s_filename, "w");
    if (s_fp == NULL) {
	perror(__func__);
        errno = 0;
        return IOFailure;
    }

    size_t num_copied = fwrite(output, sizeof(*output), output_length, s_fp);
    
    if (num_copied < output_length) {
        perror(__func__);
        errno = 0;
	fclose(s_fp);
        return IOFailure;
    }
    
    fclose(s_fp);

    *bytes_written = num_copied * sizeof(*output);
    
    Logger("hn_save got to IOSuccess\n");

    return IOSuccess;
}


enum io_error_code hn_save_weights(double **weights, char *w_filename,
				   size_t max_units)
{
    FILE *w_fp = NULL;
    if ((w_fp = fopen(w_filename, "w")) == NULL) {
        perror(__func__);
        errno = 0;
        return IOFailure;
    }
    
    for (size_t i = 0; i < max_units; ++i) {
        if (fwrite(weights[i], sizeof(**weights), max_units, w_fp) < max_units) {
	    perror(__func__);
            errno = 0;
	    fclose(w_fp);
            return IOFailure;
        }
    }
    
    fclose(w_fp);
    
    Logger("hn_save_weights got to IOSuccess\n");

    return IOSuccess;
}


enum io_error_code hn_save_next_pattern(spike_T *pattern, char *p_filename,
					size_t max_units)
{
    FILE *p_fp = NULL;
    /* We only allow appending for simplicity. */
    if ((p_fp = fopen(p_filename, "a")) == NULL) {
	perror(__func__);
        errno = 0;
	return IOFailure;
    }

    if (fwrite(pattern, sizeof(*pattern), max_units, p_fp) < max_units) {
	perror(__func__);
	fclose(p_fp);
        errno = 0;
	return IOFailure;
    }
    
    fclose(p_fp);

    Logger("hn_save_next_pattern got to IOSuccess\n");

    return IOSuccess;
}


void hn_fill_rand_pattern(spike_T *pattern, double coding_level,
                          size_t max_units)
{
    for (size_t i = 0; i < max_units; ++i) {
        pattern[i] = ((double)rand() / RAND_MAX) < coding_level ? +1 : -1;
    }
}
