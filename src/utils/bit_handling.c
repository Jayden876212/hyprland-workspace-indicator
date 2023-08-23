#include <stdio.h>
#include <stdint.h>

int print_binary(uint16_t integer) { // This is a test function for debugging only.
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

uint16_t turn_on_bit_in_array(uint16_t bit_array, int position) {
    // Shift the bit based on the position given by the function user.
    uint16_t bit = 0b1000000000000000 >> position;

    // Perform OR on the bit and the existing bit_array to turn on the bit to 1.
    bit_array |= bit;

    return bit_array;
}

unsigned int access_bit_array(uint16_t bit_array, int position) {
    // Take away 15 from the more human-readable position given by the function user.
    int true_position = 15 - position; 

    // Isolate the bit at a given position so it is on the right hand side of the bit array.
    bit_array >>= true_position;

    // Use the rightmost bit to compare 1 with the isolated bit in the bit array using AND.
    bit_array &= 0b0000000000000001;

    return bit_array;
}
