#ifndef HN_TEST_PATTERN_Q
#define HN_TEST_PATTERN_Q

#include <stdlib.h>
#include "hn_types.h"

long hn_test_pattern_q(hn_network net, spike_T *pattern, size_t max_units,
                       size_t warning_threshold, hn_mode_utils_q utils);


#endif
