#include <stdio.h>

#include <cjson/cJSON.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

// include/helpers
#include "helpers/array_handling.h"
#include "helpers/hyprland_struct_handling.h"
#include "helpers/json_handling.h"

// include/utils
#include "utils/bit_handling.h"

// Void function is used to reduce repeated code as both workspaces_array and activeworkspace_array
// have a similar printing method within the array.
void print_array_json_formatted(uint16_t bit_array) {
    for (int n = 0; n < WORKSPACES_AMOUNT; ++n) {
        // Access a bit among the specific 16 bits at the nth position.
        BitInterface bit_interface = {
            .bit_array = bit_array,
            .position = n
        };
        bool accessed_bit = access_bit_array(bit_interface);

        // Check what the accessed bit evaluates to and print true or false. This is so the user of
        // the program can use these values to hide or unhide a numbered workspace indicator by
        // looking up the number in the array and checking if it is true or false.
        accessed_bit ? printf("true") : printf("false");

        if (n != WORKSPACES_AMOUNT - 1) // Checks if we are at the end of the bool array. We would
                                        // not want to print a comma at the end because that is
                                        // invalid json.
            printf(","); // Print a comma to seperate values so it is valid json to be parsed.
    }
}

// We are using a function to print json manually rather than using cJSON_Print because this will
// create unnecessary complexity with the cJSON object. This should result in better performance.
int print_workspaces_json_array(HyprlandData *hyprland_data) {
    printf("["); // Open the json.
    for (int i = 0; i < hyprland_data->monitors_length; ++i) {
        printf("{\"workspaces\":[");
        print_array_json_formatted(hyprland_data->workspace_array[i]);

        printf("],\"activeworkspaces\":[");
        print_array_json_formatted(hyprland_data->activeworkspace_array[i]);
        printf("]}");
        if (i != hyprland_data->monitors_length - 1)
            printf(",");
    }
    printf("]\n");  // Close the json.
    fflush(stdout); // We flush stdout because we want the workspace data to be immediately sent to
                    // the receiving program. This will prevent undefined behaviour resulting from a
                    // buffered output.

    return 0;
}

// The data that we need about hyprland is pulled using a data structure that is passed to multiple
// functions.
int initialise_and_print_workspace_info_as_json() {
    HyprlandData *hyprland_data = initialise_hyprland_data_structure();

    create_workspace_array(hyprland_data);
    create_activeworkspace_array(hyprland_data);

    print_workspaces_json_array(hyprland_data);

    delete_hyprland_data_structure(hyprland_data);

    return 0;
}
