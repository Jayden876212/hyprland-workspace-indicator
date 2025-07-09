#include "data/constants.h"        // for TOTAL_BITS_IN_INT, RIGHTMOST_BIT, SINGULAR_BIT, TRUE_BIT_POSITION_OFFSET
#include "data/data_structures.h"  // for BitInterface
#include <stdbool.h>               // for bool, false
#include <stdint.h>                // for uint16_t
#include <stdio.h>                 // for printf, fprintf, stderr

// At first, bit manipulation might seem like a strange technique to create an array. After all it
// does not use traditional notation for an array, making the code more verbose and less readable.
// However, each boolean value in C uses a byte (8 bits) therefore a person with for example 3
// monitors would have a 2D array which uses a total of 3 (amount of monitors) * 10 (amount of
// workspaces) * 8 (amount of bits in a boolean value), which equals 240 bytes. However, the same
// person using a 1D integer array (a 2D bit array) would be using 3 (amount of monitors) * 16
// (amount of bits in a 16-bit integer) which equals 48 bits.

int print_binary(uint16_t integer) { // This is a test function for debugging only.
    printf("16-bit binary representation of %u: ", integer);

    for (int i = (TOTAL_BITS_IN_INT - 1); i >= 0; i--) {
        unsigned int bit = (integer >> i) & 1;
        printf("%u", bit);
        if (i % 4 == 0) {
            printf(" ");
        }
    }

    printf("\n");
    return 0;
}

uint16_t turn_on_bit_in_array(BitInterface bit_interface) {
    // Check if the function user's given position is out of bounds for safety.
    if (bit_interface.position > TOTAL_BITS_IN_INT) {
        fprintf(stderr, "Error accessing bit array: Given position is out of range.\n");
        return bit_interface.bit_array;
    }

    // Shift a bit based on the position given by the function user.
    uint16_t bit = SINGULAR_BIT >> bit_interface.position;

    // Perform OR on the bit and the existing bit_array to turn on the bit to 1.
    bit_interface.bit_array |= bit;

    return bit_interface.bit_array;
}

bool access_bit_array(BitInterface bit_interface) {
    // Check if the function user's given position is out of bounds for safety.
    if (bit_interface.position > TOTAL_BITS_IN_INT) {
        fprintf(stderr, "Error accessing bit array: Given position is out of range.\n");
        return false;
    }

    // Take away 15 from the more human-readable position given by the function user.
    int true_position = TRUE_BIT_POSITION_OFFSET - bit_interface.position;

    // Isolate the bit at a given position so it is on the right hand side of the bit array.
    bit_interface.bit_array >>= true_position;

    // Use the rightmost bit to compare 1 with the isolated bit in the bit array using AND.
    bit_interface.bit_array &= RIGHTMOST_BIT;

    // We want to return a boolean value rather than a 16-bit int because it uses 8 bit (half as
    // much memory). It is also more readable as the resulting JSON array will use a bool.
    bool accessed_bit_value = bit_interface.bit_array;

    return accessed_bit_value;
}
