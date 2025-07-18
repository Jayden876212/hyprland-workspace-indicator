#include "data/constants.h"       // for WORKSPACE_10
#include "data/data_structures.h" // for HyprlandData, BitInterface
#include "utils/bit_handling.h"   // for turn_on_bit_in_array
#include <cjson/cJSON.h>          // for cJSON_GetObjectItem, cJSON, cJSON_GetArrayItem
#include <stdint.h>               // for uint16_t
#include <stdio.h>                // for NULL, fprintf, stderr
#include <string.h>               // for strcmp

int create_workspace_array(HyprlandData *hyprland_data) {
    // Initialise pointer to workspace_array to improve code readability (reduces width).
    uint16_t *workspace_array = hyprland_data->workspace_array;

    // Loop through monitors.
    for (int monitor_index = 0; monitor_index < hyprland_data->monitors_length; ++monitor_index) {
        // Set each position (the ith monitor in this loop) in the workspace array to the int 0 to
        // clear/reset it.
        workspace_array[monitor_index] = 0b0000000000000000;

        // Perform steps to grab the name of each monitor (the ith monitor in this loop) to verify
        // the monitor belonging to each workspace. This can be used to determine which workspace
        // indicators appear on what monitor.
        cJSON *current_monitor = cJSON_GetArrayItem(hyprland_data->monitors, monitor_index);
        if (current_monitor == NULL) {
            return -1;
        }
        cJSON *monitor_name_obj = cJSON_GetObjectItem(current_monitor, "name");
        char *monitor_name = NULL;
        monitor_name = monitor_name_obj->valuestring;

        // Loop through the used workspaces.
        for (int workspace_index = 0; workspace_index < hyprland_data->workspaces_length;
             ++workspace_index) {
            // Perform steps to grab information about each monitor (the vth monitor in this loop).
            cJSON *current_workspace =
                cJSON_GetArrayItem(hyprland_data->workspaces, workspace_index);
            if (current_workspace == NULL) {
                return -1;
            }
            // Grab the name of the vth monitor's output (e.g. DP-3) to verify it belongs to the
            // ith monitor in the previous loop. Eventually it will match in that loop (only once
            // though. This can be used to reduce screen clutter and improve readability on each
            // monitor by only showing the used workspaces on THAT specific monitor.
            cJSON *active_monitor_obj = cJSON_GetObjectItem(current_workspace, "monitor");
            char *active_monitor = NULL;
            active_monitor = active_monitor_obj->valuestring;

            // Grab the ID of the workspace that the user can see what ID the workspace is
            // specifically in our resulting JSON array later on (via the array position).
            cJSON *workspace_id_obj = cJSON_GetObjectItem(current_workspace, "id");
            int workspace_id = workspace_id_obj->valueint;
            if (workspace_id < 0) {
                fprintf(stderr,
                        "Workspace of ID %i is a negative integer. This means it is likely"
                        " a special workspace. Support for these will be added soon.\n",
                        workspace_id);
            }

            // The aforementioned verification of the ith monitor output name and the vth workspace
            // output name which checks if they match.
            if (strcmp(active_monitor, monitor_name) == 0) {
                // Checks if it is workspace 10. This is because I think the user would usually
                // access the 10th workspace with the 0 key on the keyboard in some key combination.
                int target_workspace_id = (workspace_id != WORKSPACE_10) ? workspace_id : 0;

                // Uses the bit manipulating function (see the src file for bit handling) to turn on
                // the specified bit. Uses the workspace ID that the user sees as the position in
                // the bit array.
                BitInterface bit_interface = {.bit_array = workspace_array[monitor_index],
                                              .position = target_workspace_id};
                workspace_array[monitor_index] = turn_on_bit_in_array(bit_interface);
            }
        }
    }

    return 0;
}

int create_activeworkspace_array(HyprlandData *hyprland_data) {
    // Initialise pointers to improve code readability (reduces width).
    uint16_t *activeworkspace_array = hyprland_data->activeworkspace_array;
    cJSON *activeworkspace = hyprland_data->activeworkspace;

    // Perform steps needed to get the monitor output name (for comparison) and the id of the
    // workspace ID to show the user using an array (ID is position in the array).
    cJSON *active_monitor_obj = cJSON_GetObjectItem(activeworkspace, "monitor");
    char *active_monitor = NULL;
    active_monitor = active_monitor_obj->valuestring;
    cJSON *workspace_id_obj = cJSON_GetObjectItem(activeworkspace, "id");
    int workspace_id = workspace_id_obj->valueint;
    if (workspace_id < 0) {
        fprintf(stderr,
                "Workspace of ID %i is a negative integer. This means it is likely"
                " a special workspace. Support for these will be added soon.\n",
                workspace_id);
    }

    // Loop through monitors.
    for (int i = 0; i < hyprland_data->monitors_length; ++i) {
        activeworkspace_array[i] = 0b0000000000000000; // Reset array.

        // Parse the JSON to grab the monitor name.
        cJSON *ith_monitor = cJSON_GetArrayItem(hyprland_data->monitors, i);
        cJSON *monitor_name_obj = cJSON_GetObjectItem(ith_monitor, "name");
        char *monitor_name = NULL;
        monitor_name = monitor_name_obj->valuestring;

        // See previous comments on the other function
        if (strcmp(active_monitor, monitor_name) == 0) {
            int target_workspace_id = (workspace_id != WORKSPACE_10) ? workspace_id : 0;
            BitInterface bit_interface = {.bit_array = activeworkspace_array[i],
                                          .position = target_workspace_id};
            activeworkspace_array[i] = turn_on_bit_in_array(bit_interface);
        }
    }

    return 0;
}
