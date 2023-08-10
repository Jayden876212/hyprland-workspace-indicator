#include <stdio.h>
#include <stdint.h>

// include/data
#include "data/constants.h"

int print_binary(uint16_t integer) {
    printf("16-bit binary representation of %u: ", integer);

    for (int i = 15; i >= 0; i--) {
        unsigned int bit = (integer >> i) & 1;
        printf("%u", bit);
        if (i % 4 == 0) {
            printf(" ");
        }
    }

    printf("\n");
    return 0;
}

uint16_t toggle_bit_array(uint16_t bit_array, int position) {
    uint16_t bit = LEFTMOST_SINGULAR_BIT >> position;

    bit_array |= bit;

    return bit_array;
}

unsigned int access_bit_array(uint16_t bit_array, int position) {
    return (bit_array >> (15-position)) & 1;
}
