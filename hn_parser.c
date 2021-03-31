/*****************************************************
 * C FILE: hn_parser.c                               *
 * MODULE: Command line parser                       *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 *                                                   *
 *****************************************************/

#include "debug_log.h"
#include "hn_parser.h"
#include "hn_types.h"
#include "hn_macro_utils.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>


#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif


#define OptionCodes "N:M:w:p:s:m:t:hv"


char *g_help_string = "\nUsage:\n"
    "hn_basic_simulation [OPTIONS+ARGS]\n\n"
    "Hopfield Network Basic Simulation.\n"
    "Omitted arguments imply the default values within parentheses.\n\n"
    "-N NUM_UNITS         specify the number of units (500)\n"
    "-M NUM_PATTERNS      specify the number of patterns in data-file (10)\n"
    "-w W_FILENAME        specify the name of the  binary file containing the weights matrix\n(./example_data_files/weights500.bin)\n"
    "-p P_FILENAME        specify the name of the binary file containing the list of patterns\n(./example_data_files/patterns500.bin)\n"
    "-s S_FILENAME        specify the name of the save file for a list of doubles\n(results.bin)\n"
    "-m MODE_NAME         string representing the update mode: accepts either MODE_SEQUENTIAL or MODE_RANDOM\n(MODE_SEQUENTIAL)\n"
    "-t THRESHOLD         set the threshold of the activation function (0.0)\n"
    "-h, --help           this brief usage explanation\n"
    "-v, --version        displays version;\n";


char *g_version_info = "\nHopfield Network Basic Simulation - Version 1.0\n";


struct option g_longopts[] = {
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    {0} };


hn_options *hn_retrieve_options(hn_options *opts, int argc, char **argv)
{
    /* Set defaults */
    set_default_options(opts);
    if (errno) {
	perror("set_valid_options failure");
	errno = 0;
	return NULL;
    }
    
    /* Iterate over any retrievable options until none is left */
    int code = getopt_long(argc, argv, OptionCodes, g_longopts, NULL);

    while (code != -1) {

        Logger("retrieved code = '%c'; argument index = %d; "
               "argument = \"%s\"\n", code, optind, optarg ? optarg : "(no_arg)");

        /* Check for invalid options */
        if (code == '?') {
            fprintf(stderr, "%s: Invalid option label: '-%c'\n",
		    __FILE__, optopt);
            return NULL;

        /* If the argument is valid, update the opts pointer
	 * and check for correctness */
        } else {
            Logger("current optarg = \"%s\"\n", optarg);
            set_option_argument(opts, code, optarg);
        }
    }

    /* Check paths */
    if (!valid_paths(opts)) {
	return NULL;
    }
    
    return opts;
}


void set_default_options(hn_options *opts)
{
    opts->max_units = 500;
    opts->max_patterns = 10;
    opts->w_filename = strdup("example_data_files/weights500.bin");
    opts->p_filename = strdup("example_data_files/patterns500.bin");
    opts->s_filename = strdup("results.bin");
    opts->mode = MODE_SEQUENTIAL;
    opts->threshold = 0.;
}


size_t nonnegative_size_from_string(char *token)
{
    long lsize = strtol(token, NULL, 10);

    if (lsize < 0) {
	fprintf(stderr, "%s: Negative number as a size.\n", __func__);
	lsize = -lsize;
    }

    Logger("lsize = %ld\n", lsize);

    return (size_t)lsize;
}


double nonnegative_double_from_string(char *token)
{
    double dvalue = strtod(token, NULL);
    
    if (dvalue < 0.) {
	fprintf(stderr, "%s: Negative threshold value\n", __func__);
	dvalue = -dvalue;
    }
    
    Logger("dvalue = %f\n", dvalue);

    return dvalue;
}


void set_option_argument(hn_options *opts, char code, char *token)
{
    switch (code) {
    default:
    case 'h':
        printf("%s\n", g_help_string);
        exit(EXIT_SUCCESS);
    case 'v':
        printf("%s\n", g_version_info);
        exit(EXIT_SUCCESS);
        /* codes 'N and 'M': if the string is invalid, strtol returns 0L */
    case 'N':
	Logger("max_units token = \"%s\"\n", token);
	opts->max_units = nonnegative_size_from_string(token);
	break;
    case 'M':
	Logger("max_patterns token = \"%s\"\n", token);
	opts->max_patterns = nonnegative_size_from_string(token);
	break;
    case 'w':
	free(opts->w_filename);
	opts->w_filename = malloc(PATH_MAX * sizeof(char));
	KillUnless(opts->w_filename != NULL);
	realpath(token, opts->w_filename);
	Logger("Path = \"%s\"\n", opts->w_filename);
	break;
    case 'p':
	free(opts->p_filename);
	opts->p_filename =  malloc(PATH_MAX * sizeof(char));
	KillUnless(opts->p_filename != NULL);
	realpath(token, opts->p_filename);
	Logger("Path = \"%s\"\n", opts->p_filename);
	break;
    case 's':
	free(opts->s_filename);
	opts->s_filename =  malloc(PATH_MAX * sizeof(char));
	KillUnless(opts->s_filename != NULL);
	realpath(token, opts->s_filename);
	Logger("Path = \"%s\"\n", opts->s_filename);
	break;
    case 'm':
        if (strcmp(token, "MODE_SEQUENTIAL") == 0) {
            opts->mode = MODE_SEQUENTIAL;
        } else {
            opts->mode = MODE_RANDOM;
            if (strcmp(token, "MODE_RANDOM") == 0) {
                PrintWarning("Unknown update mode \"%s\". "
                             "Defaulting to MODE_RANDOM\n", token);
            }
        }
	break;
    case 't':
	Logger("threshold token = \"%s\"\n", token);
	opts->threshold = nonnegative_double_from_string(token);
	break;
    }
}


int valid_paths(hn_options *opts)
{
    int all_valid = 1;
    errno = 0;
    if (access(opts->w_filename, O_RDONLY) < 0) {
	fprintf(stderr, "%s: %s\n", opts->w_filename, strerror(errno));
	all_valid = 0;
    }
    
    errno = 0;
    if (access(opts->p_filename, O_RDONLY) < 0) {
	fprintf(stderr, "%s: %s\n", opts->p_filename, strerror(errno));
	all_valid = 0;
    }
    
    errno = 0;
    if (access(opts->s_filename, W_OK) < 0 && errno != ENOENT) {
	fprintf(stderr, "%s: %s\n", opts->s_filename, strerror(errno));
	all_valid = 0;
    }

    errno = 0;
                
    return all_valid;
}


void hn_free_options(hn_options *opts)
{
    free(opts->w_filename);
    free(opts->p_filename);
    free(opts->s_filename);
    free(opts);
}
