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

#include "../../debug_log/debug_log.h"
#include "../hn_parser.h"
#include "../hn_types.h"


int main(int argc, char **argv)
{
    hn_options *opts = malloc(sizeof (hn_options));
    KillUnless(opts != NULL);
    opts = hn_retrieve_options(opts, argc, argv);
    KillUnless(opts != NULL);
    printf("opts->mode = \"%s\"\n", opts->mode);
    printf("opts->threshold = %f\n", opts->threshold);
    printf("opts->max_units = %zu\n", opts->max_units);
    printf("opts->max_patterns = %zu\n", opts->max_patterns);
    printf("opts->w_filename = \"%s\"\n", opts->w_filename);
    printf("opts->p_filename = \"%s\"\n", opts->p_filename);
    printf("opts->s_filename = \"%s\"\n", opts->s_filename);
    hn_free_options(opts);
    Logger("Got to exit statement\n");
    exit(EXIT_SUCCESS);
}
