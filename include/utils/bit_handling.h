// bit_handling.h

#ifndef BIT_HANDLING_H
#define BIT_HANDLING_H

#include <stdbool.h>
#include <stdint.h>

#include "data/data_structures.h"

uint16_t turn_on_bit_in_array(BitInterface bit_interface);

bool access_bit_array(BitInterface bit_interface);

int print_binary(uint16_t integer);

#endif
