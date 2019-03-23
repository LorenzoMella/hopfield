/*****************************************************
 * HEADER FILE: hn_parser.h                          *
 * MODULE: Command line parser                       *
 *                                                   *
 * ROLE:     Interprets command line arguments       *
 *           and communicates the "option"           *
 *           information in a compact structure      *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 *                                                   *
 *****************************************************/

#ifndef HN_PARSER_H
#define HN_PARSER_H

#include "hn_types.h"


/**
 * Generate hn_options from a syntactically correct sequence of
 * command line tokens (or handle any errors).
 *
 * @param opts:     a pointer to the hn_options data structure to fill
 * @param argc:     number of command line tokens
 * @param argv:     command line tokens
 *
 * @return:         the hn_options data structure or NULL on error
 */
hn_options *hn_retrieve_options(hn_options *opts, int argc, char **argv);


/**
 * Sets default values to the options in the passed structure, namely:
 * w_filename = "weights.bin"; p_filename = "patterns.bin";
 * s_filename = "results.bin"; mode = "Sequential"; threshold = 0.0.
 *
 * @param opts:     pointer to the hn_options structure
 */
void set_default_options(hn_options *opts);


/**
 * Sets the correct option described by the passed argument (token); the option
 * that must be updated is identified by the option code.
 *
 * @param opts:         pointer to the hn_options structure
 * @param code:         the code that identifies the option
 * @param token:        the token that describes the option value
 */
void set_option_argument(hn_options *opts, char code, char *token);


/**
 * Returns a size_t integer from a string token,
 * changing sign and issuing a warning if it is negative.
 */
size_t nonnegative_size_from_string(char *token);


/**
 * Returns a double from a string token,
 * changing sign and issuing a warning if it is negative.
 */
double nonnegative_double_from_string(char *token);


/**
 * Verifies the filenames in the hn_options structure.
 * If sizes are positive
 *
 * @param opts:     pointer to hn_options data structure
 *
 * @return:         boolean answer
 */
int valid_paths(hn_options *opts);


/**
 * Frees the internal buffers and the hn_options structure.
 */
void hn_free_options(hn_options *opts);

    
#endif /* HN_PARSER_H */
