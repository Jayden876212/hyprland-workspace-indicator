#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <poll.h>
#include <errno.h>

#include <cjson/cJSON.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

// include/helpers
#include "helpers/array_handling.h"
#include "helpers/json_handling.h"
#include "helpers/hyprland_struct_handling.h"

// include/utils
#include "utils/hyprland_socket_handling.h"

cJSON * allocate_and_grab_json(const char * cmd) {
    SocketData * socket_data = initialise_socket_data_structure();
    if (socket_data == NULL) {
        fprintf(stderr, "Error: Failed to allocate socket data structure.\n");
        return NULL;
    }
    cJSON * output_json = grab_json_from_socket_data(cmd, socket_data);
    if (output_json == NULL) {
        fprintf(stderr, "Error: Failed to grab JSON from socket data.\n");
        return NULL;
    }
    return output_json;
}

int grab_hyprland_indicator_data_json(HyprlandData * hyprland_data) {
    cJSON * monitors = allocate_and_grab_json(CMD_MONITORS);
    if (monitors == NULL) {
        return -1;
    }
    hyprland_data->monitors = monitors;

    cJSON * workspaces = allocate_and_grab_json(CMD_WORKSPACES);
    if (workspaces == NULL) {
        return -1;
    }
    hyprland_data->workspaces = workspaces;

    cJSON * activeworkspace = allocate_and_grab_json(CMD_ACTIVEWORKSPACE);
    if (activeworkspace == NULL) {
        return -1;
    }
    hyprland_data->activeworkspace = activeworkspace;
    
    return 0;
}

HyprlandData * initialise_hyprland_data_structure() {
    HyprlandData * hyprland_data;
    hyprland_data = (HyprlandData*)malloc(sizeof(HyprlandData));
    if (hyprland_data == NULL) {
        perror("malloc");
        return NULL;
    }

    int json_grab_result = grab_hyprland_indicator_data_json(hyprland_data);
    if (json_grab_result == -1) {
        fprintf(stderr, "Error: Failed to grab the necessary json data for the indicator.");
        return NULL;
    }

    hyprland_data->monitors_length = cJSON_GetArraySize(hyprland_data->monitors);
    hyprland_data->workspaces_length = cJSON_GetArraySize(hyprland_data->workspaces);

    uint16_t * workspace_array;
    workspace_array = (uint16_t *)malloc(hyprland_data->monitors_length * sizeof(uint16_t));
    hyprland_data->workspace_array = workspace_array;

    uint16_t * activeworkspace_array;
    activeworkspace_array = (uint16_t *)malloc(hyprland_data->monitors_length * sizeof(uint16_t));
    hyprland_data->activeworkspace_array = activeworkspace_array;

    return hyprland_data;
}

void delete_hyprland_data_structure(HyprlandData * hyprland_data) {
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
}

SocketData * initialise_socket_data_structure() {
    SocketData * socket_data = (SocketData*)malloc(sizeof(SocketData));

    socket_data->data_received = (char*)malloc(MAX_BUFFER_SIZE + 1);

    socket_data->poll_descriptor = (struct pollfd*)malloc(sizeof(struct pollfd));
    socket_data->poll_descriptor->events = POLLIN;

    return socket_data;
}

void cleanup_socket_data_structure(SocketData * socket_data) {
    free(socket_data->data_received);
    socket_data->data_received = NULL;

    free(socket_data->poll_descriptor);
    socket_data->poll_descriptor = NULL;
}

void delete_socket_data_structure(SocketData * socket_data) {
    if (socket_data == NULL) {
        fprintf(stderr, "Error accessing SocketData struct: SocketData invalid memory (NULL).\n");
        cleanup_socket_data_structure(socket_data);
        return;
    }

    int close_result = close(socket_data->poll_descriptor->fd);
    if (close_result == -1) {
        fprintf(stderr, "Error closing socket: %s\n", strerror(errno));
        cleanup_socket_data_structure(socket_data);
        return;
    }

    cleanup_socket_data_structure(socket_data);
    free(socket_data);
}
