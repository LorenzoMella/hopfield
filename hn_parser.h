/*****************************************************
 * HEADER FILE: hn_parser.h                          *
 * MODULE: Command line parser                       *
 *                                                   *
 * FUNCTION: Interprets command line arguments       *
 *           and communicates the "option"           *
 *           information in a compact structure      *
 *           (assumes bash/Unix-like conventions)    *
 *                                                   *
 * PROJECT NAME: Hopfield Network simulation         *
 *               (prototype)                         *
 *                                                   *
 * SUPERVISOR: Mark van Rossum                       *
 *                                                   *
 * AUTHOR:  Lorenzo Mella                            *
 * VERSION: 29/06/2016                               *
 *                                                   *
 *****************************************************/

#ifndef HN_PARSER_H
#define HN_PARSER_H

#include "hn_types.h"


typedef enum parse_error_code {
    PARSE_SUCCESS,
    PARSE_INVALID_OPT,
    PARSE_INVALID_ARG
} parse_error_code;


/**
 * Generate hn_options from a syntactically correct sequence of
 * command line tokens (or handle any errors).
 *
 * @param argc:     number of command line tokens
 * @param argv:     command line tokens
 *
 * @return:         the decoded options
 */
parse_error_code hn_retrieve_options(hn_options *opts, int argc, char **argv);


/**
 * Sets default values to the options in the passed structure, namely:
 * w_filename = "weights.bin"; p_filename = "patterns.bin";
 * s_filename = "results.bin"; mode = SEQUENTIAL; threshold = 0.0.
 *
 * @param opts:     pointer to the hn_options structure
 */
void set_default_options(hn_options *opts);


/**
 * Verifies whether the option token makes syntactical sense.
 *
 * @param option_code:  the code that identifies the type of argument
 * @param token:        the string to be checked
 *
 * @return:             boolean answer (1/0)
 */
int is_valid_option_argument(char option_code, char *token);


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
 * Verifies whether a token is a syntactically correct path to a file or folder;
 * essentially, it must not have spaces and the only allowable characters are
 * alphanumerical, '-', '_' and '.' and '/';
 * (example: /User/myName/project_3.proj).
 *
 * @param path:     the string to be checked
 *
 * @return:         boolean answer (1/0)
 */
int is_valid_path(char *path);

/**
 * Verifies whether a trimmed token (not starting or ending with whitespace)
 * is a file or folder name composed exclusively by alphanumerical ASCII
 * characters or the characters '-', '_' and '.' (POSIX-compatible filenames).
 *
 * @param name:     the string to be checked
 *
 * @return:         boolean answer (1/0)
 */
int is_portable_filename(char *name);


/**
 * Verifies whether a trimmed token (not starting or ending with whitespace)
 * is exactly one of the special path abbreviations ".", ".." or "~".
 *
 * @param str:     the string to be checked
 *
 * @return:        boolean answer (1/0)
 */
int is_path_abbreviation(char *str);


/**
 * If path doesn't start with the abbreviation "~", path is returned as is;
 * otherwise "~" is replaced by the path to the user folder, e.g.,
 * "~/myFolder/myFile.abc" becomes "/Users/myUserName/myFolder/myFile.abc".
 *
 * @param path:     the path (should be syntactically correct)
 *
 * @return:         the correctly expanded path
 */
char *make_full_path(char *path);

#endif
