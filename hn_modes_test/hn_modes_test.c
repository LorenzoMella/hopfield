/* hn_modes_test.c */

#include <stdio.h>
#include <stdlib.h>
#include "hn_types.h"
#include "hn_modes.h"


int main(int argc, char **argv)
{
    size_t i;
    size_t max_units = 20;
    hn_mode_utils utils = hn_utils_with_mode(SEQUENTIAL);
    
    printf("Testing hn_modes.[hc]\n\n");
    
    printf("Testing sequential_select_unit(): checking that it maintains "
           "state and correctly computes the remainder (mod 20)\n");
    for (i = 0; i < 100; ++i) {
        printf("%lu ", sequential_select_unit(max_units, 0));
    }
    printf("\n\n");
    
    printf("Testing random_select_unit():\n");
    for (i = 0; i < 100; ++i) {
        printf("%lu ", random_select_unit(max_units, 0));
    }
    printf("\n\n");
    
    printf("Testing (*select_unit)(): checking whether it is correctly "
           "referenced in the hn_mode_utils data structure\n");
    for (i = 0; i < max_units; ++i) {
        printf("%lu ", utils.select_unit(max_units, 0));
    }
    printf("\n\n");
    
    printf("Testing sequential_stability_warning(): checking whether "
           "stability_counter resets correctly.\n");
    for (i = 0; i < 21; ++i) {
        printf("%d ", sequential_stability_warning(0, 5));
    }
    printf("\n\n");
    
        printf("Testing (*stability_warning)(): checking whether it is "
               "correctly referenced in the hn_mode_utils data structure\n");
    for (i = 0; i < 5; ++i) {
        printf("%d ", utils.stability_warning(0, 1));
    }
    printf("\n\n");

    
    return 0;
}