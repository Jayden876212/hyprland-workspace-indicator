#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <cjson/cJSON.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

// include/helpers
#include "helpers/array_handling.h"
#include "helpers/json_handling.h"
#include "helpers/hyprland_struct_handling.h"

// include/utils
#include "utils/hyprland_socket_handling.h"
#include "utils/bit_handling.h"

int print_workspaces_json_array(HyprlandData * hyprland_data) {
    printf("[");
    for (int i = 0; i < hyprland_data->monitors_length; ++i) {
        printf("{\"workspaces\":[");
        for (int n = 0; n < WORKSPACES_AMOUNT; ++n) {
            (access_bit_array(hyprland_data->workspace_array[i], n) == BITWISE_FALSE) ? printf("false") : printf("true");
            if (n != WORKSPACES_AMOUNT - 1)
                printf(",");
        }
        printf("],\"activeworkspaces\":[");
        for (int n = 0; n < WORKSPACES_AMOUNT; ++n) {
            (access_bit_array(hyprland_data->activeworkspace_array[i], n) == BITWISE_FALSE) ? printf("false") : printf("true");
            if (n != WORKSPACES_AMOUNT - 1)
                printf(",");
        }
        printf("]}");
        if (i != hyprland_data->monitors_length - 1)
            printf(",");
        
    }
    printf("]\n");
    fflush(stdout);

    return 0;
}

int initialize_and_print_workspace_info_as_json() {
    HyprlandData * hyprland_data = initialise_hyprland_data_structure();

    create_workspace_array(hyprland_data);
    create_activeworkspace_array(hyprland_data);

    print_workspaces_json_array(hyprland_data);

    delete_hyprland_data_structure(hyprland_data);

    return 0;
}
