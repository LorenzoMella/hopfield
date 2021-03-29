/***************************************************************
 * FILE:            debug_log.h                                *
 *                                                             *
 * FUNCTION:        Chad logging system                        *
 *                                                             *
 * AUTHOR:          Lorenzo Mella                              *
 ***************************************************************/


#ifndef H_DEBUG_LOG_H
#define H_DEBUG_LOG_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // strerror
#include <errno.h>
#include <inttypes.h>


/**
 * API identical to printf. Also prints the location and
 * is preprocessed out unless DEBUG_LOG is defined.
 */
#ifdef DEBUG_LOG

#define Logger(...) do {						\
	fprintf(stderr, "Log: %s:%d (%s): ",				\
		__FILE__, __LINE__, __func__);				\
	fprintf(stderr, __VA_ARGS__);					\
    } while(0)

#else

#define Logger(...)  /* to do nothing */

#endif /* DEBUG_LOG */


/**
 * Loggers for variables. Just use them on an appropriate variable
 * and they conveniently print name and value.
 */
#define LogInt(x)     Logger("%s = %"PRIdMAX"\n", #x, (intmax_t)(x))
#define LogFloat(x)   Logger("%s = %Lf\n", #x, (long double)(x))
#define LogString(x)  Logger("%s = \"%s\"\n", #x, (x))


/**
 * Same as Logger but always active.
 */
#define PrintWarning(...) do {						\
	fprintf(stderr, "Warning: %s:%d (%s): ",			\
		__FILE__, __LINE__, __func__);				\
	fprintf(stderr, __VA_ARGS__);					\
    } while(0)


/**
 * Print error message if errno is set by some event.
 * The idea is that errno is set either by condition or a statement
 * immediately above the macro.
 */
#define PrintErrorMessages(condition) do {				\
	if (errno) {							\
	    fprintf(stderr, "Warning: %s:%d (%s): %s: %s\n",		\
		    __FILE__, __LINE__, __func__,			\
		    #condition, strerror(errno));			\
	}								\
    } while(0)


/**
 * Like assert. If the errno variable is set,
 * the error is reported.
 */
#define KillUnless(condition)						\
    do {								\
	if (!(condition)) {						\
	    if (errno) {						\
		fprintf(stderr, "Killed: %s:%d (%s): "			\
			"condition (%s) is false: %s\n",		\
			__FILE__, __LINE__, __func__, #condition,	\
			strerror(errno));				\
	    } else {							\
		fprintf(stderr, "Killed: %s:%d (%s): "			\
			"condition (%s) is false\n",			\
			__FILE__,  __LINE__, __func__, #condition);	\
	    }								\
	    exit(EXIT_FAILURE);						\
	}								\
    } while(0)


#endif /* H_DEBUG_LOG_H */
