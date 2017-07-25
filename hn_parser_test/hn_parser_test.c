/*****************************************************
 * C FILE (main): hn_parser_test.c                   *
 * MODULE: Command line parser functionality test    *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 05/07/2016                               *
 *                                                   *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hn_types.h"
#include "hn_parser.h"


int main(int argc, char **argv)
{
    hn_options *opts;
    
    if ((opts = malloc(sizeof (hn_options))) == NULL) {
        fprintf(stderr, "Memory error\n");
        exit(EXIT_FAILURE);
    }
    
    hn_retrieve_options(opts, argc, argv);
    
    printf("opts->mode = %s\n",
            opts->mode == SEQUENTIAL ? "SEQUENTIAL" : "RANDOM");
    printf("opts->threshold = %f\n", opts->threshold);
    printf("opts->max_units = %lu\n", opts->max_units);
    printf("opts->max_patterns = %lu\n", opts->max_patterns);
    printf("opts->w_filename = \"%s\"\n", opts->w_filename);
    printf("opts->p_filename = \"%s\"\n", opts->p_filename);
    printf("opts->s_filename = \"%s\"\n", opts->s_filename);
    
    printf("Got to exit statement\n");
    exit(EXIT_SUCCESS);
}
