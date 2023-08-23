#include <stdio.h>
#include <stdlib.h>

#include <string.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

// include/utils
#include "utils/bit_handling.h"

int create_workspace_array(HyprlandData * hyprland_data) {
    for (int i = 0; i < hyprland_data->monitors_length; ++i) {
        hyprland_data->workspace_array[i] = BITWISE_FALSE; 
        cJSON * ith_monitor = cJSON_GetArrayItem(hyprland_data->monitors, i);
        if (ith_monitor == NULL) {
            return -1;
            cJSON_Delete(hyprland_data->monitors);
        }
        cJSON * monitor_name_obj = cJSON_GetObjectItem(ith_monitor, "name");
        char * monitor_name = monitor_name_obj->valuestring;

        for (int v = 0; v < hyprland_data->workspaces_length; ++v) {
            cJSON * vth_workspace = cJSON_GetArrayItem(hyprland_data->workspaces, v);
            if (vth_workspace == NULL) {
                return -1;
                cJSON_Delete(hyprland_data->monitors);
            }

            cJSON * active_monitor_obj = cJSON_GetObjectItem(vth_workspace, "monitor");
            char * active_monitor = active_monitor_obj->valuestring;
            cJSON * workspace_id_obj = cJSON_GetObjectItem(vth_workspace, "id");
            int workspace_id = workspace_id_obj->valueint;


            if (strcmp(active_monitor, monitor_name) == 0) {
                int target_workspace_id = (workspace_id != 10) ? workspace_id : 0;
                hyprland_data->workspace_array[i] = toggle_bit_array(hyprland_data->workspace_array[i], target_workspace_id);
            }
        }
    }

    return 0;
}

int create_activeworkspace_array(HyprlandData * hyprland_data) {
    cJSON * active_monitor_obj = cJSON_GetObjectItem(hyprland_data->activeworkspace, "monitor");
    char * active_monitor = active_monitor_obj->valuestring;

    cJSON * workspace_id_obj = cJSON_GetObjectItem(hyprland_data->activeworkspace, "id");
    int workspace_id = workspace_id_obj->valueint;

    for (int i = 0; i < hyprland_data->monitors_length; ++i) {
        hyprland_data->activeworkspace_array[i] = BITWISE_FALSE; 

        cJSON * ith_element = cJSON_GetArrayItem(hyprland_data->monitors, i);
        cJSON * monitor_name_obj = cJSON_GetObjectItem(ith_element, "name");
        char * monitor_name = monitor_name_obj->valuestring;

        if (strcmp(active_monitor, monitor_name) == 0) {
            int target_workspace_id = (workspace_id != 10) ? workspace_id : 0;
            hyprland_data->activeworkspace_array[i] = toggle_bit_array(hyprland_data->activeworkspace_array[i], target_workspace_id);
        }
    }

    return 0;
}
