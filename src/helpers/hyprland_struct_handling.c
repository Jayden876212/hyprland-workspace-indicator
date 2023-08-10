#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <poll.h>

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

HyprlandData * initialise_hyprland_data_structure() {
    HyprlandData * hyprland_data = (HyprlandData*)malloc(sizeof(HyprlandData));


    SocketData * hyprctl_data_monitors = create_socket_data();
    hyprland_data->monitors = grab_json_from_socket_data("-j/monitors", hyprctl_data_monitors);
    SocketData * hyprctl_data_workspaces = create_socket_data();
    hyprland_data->workspaces = grab_json_from_socket_data("-j/workspaces", hyprctl_data_workspaces);
    SocketData * hyprctl_data_activeworkspace = create_socket_data();
    hyprland_data->activeworkspace = grab_json_from_socket_data("-j/activeworkspace", hyprctl_data_activeworkspace);

    hyprland_data->monitors_length = cJSON_GetArraySize(hyprland_data->monitors);
    hyprland_data->workspaces_length = cJSON_GetArraySize(hyprland_data->workspaces);

    hyprland_data->workspace_array = (uint16_t *)malloc(hyprland_data->monitors_length * sizeof(uint16_t));
    hyprland_data->activeworkspace_array = (uint16_t *)malloc(hyprland_data->monitors_length * sizeof(uint16_t));

    return hyprland_data;
}

int delete_hyprland_data_structure(HyprlandData * hyprland_data) {
    cJSON_Delete(hyprland_data->monitors);
    hyprland_data->monitors = NULL;
    cJSON_Delete(hyprland_data->activeworkspace);
    hyprland_data->activeworkspace = NULL;

    free(hyprland_data->workspace_array);
    hyprland_data->workspace_array = NULL;
    free(hyprland_data->activeworkspace_array);
    hyprland_data->activeworkspace_array = NULL;

    free(hyprland_data);
    hyprland_data = NULL;

    return 0;
}

SocketData * create_socket_data() {
    SocketData * socket_data = (SocketData*)malloc(sizeof(SocketData));

    socket_data->data_received = (char*)malloc(MAX_BUFFER_SIZE + 1);

    socket_data->poll_descriptor = (struct pollfd*)malloc(sizeof(struct pollfd));
    socket_data->poll_descriptor->events = POLLIN;

    return socket_data;
}
