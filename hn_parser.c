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
 * VERSION: 25/07/2017                               *
 *                                                   *
 *****************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "hn_types.h"
#include "hn_parser.h"
#include "debug_log.h"

/* strcmp returns 0 iff the argument strings are equal: counterintuitive! */
#define STRINGS_ARE_EQUAL !strcmp


/*
 * Some facts about using getopt():
 * 1 - it stops reading as soon as it encounters an unknown option (returns -1)
 * 2 - if it expects an argument, it simply reads what follows, even if it
 *     starts with - and might therefore be another option token
 * 3 - the allowable codes must be passed to getopt as a single string; see
 *     HN_OPTION_CODES below. If a code is followed by ':', it means that the
 *     corresponding option expects also an argument.
 */

/*
 * Character code meanings:
 * -N   number of units (usually stored in max_units)
 * -M   number of patterns in data-file (usually stored in max_patterns)
 * -w   name of binary file containing the weights matrix
 * -p   name of binary file containing the list of patterns
 * -s   name of a savefile for a list of doubles
 * -m   string representing the update mode
 * -t   the threshold of the activation function
 */
#define HN_OPTION_CODES "N:M:w:p:s:m:t:"

/* You can enter arguments in any order, but if an invalid option code is met
 * subsequent correct ones are ignored */
parse_error_code hn_retrieve_options(hn_options *opts, int argc, char **argv)
{
    char code;
    
    /* Set defaults */
    set_default_options(opts);
    
    /* Iterate over retrievable options until none is left */
    while ((code = getopt(argc, argv, HN_OPTION_CODES)) != -1) {
        debug_log("retrieved code = '%c'; argument index = %d; "
                  "argument = \"%s\"\n", code, optind, optarg);
        /* Check for invalid options */
        if (code == '?') {
            fprintf(stderr, "%s: Invalid option: '%c'\n", __FILE__, optopt);
            return PARSE_INVALID_OPT;
        /* If argument is valid, update opts pointer */
        } else if (is_valid_option_argument(code, optarg)) {
            debug_log("current optarg = \"%s\"\n", optarg);
            set_option_argument(opts, code, optarg);
        /* If argument is invalid */
        } else {
            fprintf(stderr, "%s: Invalid argument: \"%s\"\n", __FILE__, optarg);
            return PARSE_INVALID_ARG;
        }
    }
    return PARSE_SUCCESS;
}


void set_default_options(hn_options *opts)
{
    opts->max_units = 500;
    opts->max_patterns = 10;
    opts->w_filename = "weights.bin";
    opts->p_filename = "patterns.bin";
    opts->s_filename = "results.bin";
    opts->mode = SEQUENTIAL;
    opts->threshold = 0.;
}


int is_valid_option_argument(char option_code, char *token)
{
    switch (option_code) {
        case 'w':
        case 'p':
        case 's':
            if(!is_valid_path(token)) {
                printf("Option code '%c': invalid path %s\n",
                       option_code, token);
                return 0;
            }
            break;
        case 'm':
            /* If the two available possibilities don't match */
            if (!STRINGS_ARE_EQUAL(token, "SEQUENTIAL") &&
                !STRINGS_ARE_EQUAL(token, "RANDOM")) {
                return 0;
            }
            break;
        case 'N':
        case 'M':
        case 't':
            /* No easy way to check these: for instance, strtod, strtol, etc.
             * return 0 if the token doesn't represent a number */
            debug_log("Argument is %s\n", token);
            break;
    }
    /* Syntax text passed */
    return 1;
}


void set_option_argument(hn_options *opts, char code, char *token)
{
    switch (code) {
        /* codes 'N and 'M': if string is invalid, strtol returns 0L */
        case 'N':
            debug_log("max_units token = \"%s\"\n", token);
            opts->max_units = (size_t)strtol(token, NULL, 10);
            break;
        case 'M':
            debug_log("max_patterns token = \"%s\"\n", token);
            opts->max_patterns = (size_t)strtol(token, NULL, 10);
            break;
        case 'w':
            opts->w_filename = make_full_path(token);
            debug_log("Path = \"%s\"\n", opts->w_filename);
            break;
        case 'p':
            opts->p_filename = make_full_path(token);
            debug_log("Path = \"%s\"\n", opts->p_filename);
            break;
        case 's':
            opts->s_filename = make_full_path(token);
            debug_log("Path = \"%s\"\n", opts->s_filename);
            break;
        case 'm':
            if (STRINGS_ARE_EQUAL(token, "SEQUENTIAL")) {
                opts->mode = SEQUENTIAL;
            } else if (STRINGS_ARE_EQUAL(token, "RANDOM")) {
                opts->mode = RANDOM;
            }
            break;
        case 't':
            /* If string is invalid, strtod returns 0.0 */
            debug_log("threshold token = \"%s\"\n", token);
            opts->threshold = strtod(token, NULL);
            break;
        }
}


/* 
 * Token is already without whitespace. Hence we must verify that,
 * after it's broken with strtok as a collection of tokens (all without
 * whitespace), they are either: valid names (in particular not "", that is,
 * no // in the original string) or, regarding the first, must be exactly
 * one of "~", "." or "..".
 */
int is_valid_path(char *path)
{
    int is_valid_path = 1;
    char *path_copy = malloc((strlen(path) + 1) * sizeof (char));
    char *token = malloc((strlen(path) + 1) * sizeof (char));
    /* strtok disrupts the original string: better work with a copy */
    strcpy(path_copy, path);

    token = strtok(path_copy, "/");
    /* Check that the first token is either a valid folder/filename
     * or one of ".", ".." or "~" */
    
    debug_log("Analysed token = %s\n", token);
    
    if (!is_portable_filename(token) && !is_path_abbreviation(token)) {
        is_valid_path = 0;
    }
    /* Check that the other tokens are valid folder/filenames */
    while ((token = strtok(NULL, "/")) != NULL) {
        
        debug_log("Analysed token = %s\n", token);
        
        if (!is_portable_filename(token)) {
            is_valid_path = 0;
        }
    }
    
    free(token);
    free(path_copy);
    return is_valid_path;
}


int is_portable_filename(char *name)
{
    /* ctype.h functions take unsigned chars */
    unsigned char c;
    /* Check for NULL pointers or empty strings */
    if (name == NULL || *name == '\0') {
        return 0;
    }
    
    /* Search for non-allowable characters until '\0'*/
    while ((c = *(name++)) != '\0') {
        debug_log("Checking character '%c'...\n", c);
        if (isspace(c) || (!isalnum(c) && c != '_' && c != '-' && c != '.')) {
            return 0;
        }
        
    }
    debug_log("All characters ok.\n");
    return 1;
}


int is_path_abbreviation(char *str)
{
    return STRINGS_ARE_EQUAL(str, ".") || STRINGS_ARE_EQUAL(str, "..") ||
           STRINGS_ARE_EQUAL(str, "~");
}


static char *make_full_path(char *path)
{
    char *full_path = NULL;
    
    if (path[0] != '~') {
        full_path = path;
    } else {
        asprintf(&full_path, "%s%s", getenv("HOME"), path+1)
        if (full_path == NULL) {
            printf("%s, line %d: memory allocation error\n",
                   __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    }
    return full_path;
}

