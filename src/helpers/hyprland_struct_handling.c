#include "helpers/hyprland_struct_handling.h"
#include "data/constants.h" // for CMD_ACTIVEWORKSPACE, CMD_MONITORS, CMD_WORKSPACES, MAX_BUFFER_SIZE
#include "data/data_structures.h"           // for HyprlandData, SocketData
#include "utils/hyprland_socket_handling.h" // for grab_json_from_socket_data
#include <cjson/cJSON.h> // for cJSON_Delete, cJSON_GetArraySize, cJSON_IsArray, cJSON
#include <errno.h>       // for errno
#include <poll.h>        // for pollfd, POLLIN
#include <stdint.h>      // for uint16_t
#include <stdio.h>       // for NULL, fprintf, perror, stderr
#include <stdlib.h>      // for free, malloc
#include <string.h>      // for strerror
#include <unistd.h>      // for close

cJSON *allocate_and_grab_json(const char *cmd) { // This function exists to reduce repeated code.
    // Initialise the socket data structure so it can be used to receive the json data.
    SocketData *socket_data = initialise_socket_data_structure();
    if (socket_data == NULL) {
        fprintf(stderr, "Error: Failed to allocate socket data structure.\n");
        return NULL;
    }

    // Grab the json so we can add it to our HyprlandData structure.
    cJSON *output_json = grab_json_from_socket_data(cmd, socket_data);
    if (output_json == NULL) {
        fprintf(stderr, "Error: Failed to grab JSON from socket data.\n");
        return NULL;
    }

    return output_json;
}

int grab_hyprland_indicator_data_json(HyprlandData *hyprland_data) {
    // Grab all the necessary data to determine the final array to be used in our workspace
    // indicator.
    cJSON *monitors = allocate_and_grab_json(CMD_MONITORS);
    if (monitors == NULL) {
        return -1; // We don't need an error message because the allocate_and_grab_json already
                   // returns the necessary error messages.
    }
    hyprland_data->monitors = monitors;

    cJSON *workspaces = allocate_and_grab_json(CMD_WORKSPACES);
    if (workspaces == NULL) {
        cJSON_Delete(monitors);
        return -1;
    }
    hyprland_data->workspaces = workspaces;

    cJSON *activeworkspace = allocate_and_grab_json(CMD_ACTIVEWORKSPACE);
    if (activeworkspace == NULL) {
        cJSON_Delete(monitors);
        cJSON_Delete(workspaces);
        return -1;
    }
    hyprland_data->activeworkspace = activeworkspace;

    return 0;
}

HyprlandData *initialise_hyprland_data_structure() {
    // We are allocating data for a struct because we can pass this through many different
    // functions as an argument. This will reduce the amount of arguments we have to pass, reducing
    // code width and repeated code in our coupling. All the code for managing the data for our
    // indicator to work is abstracted into this data structure, where the functions are coupled
    // together using the HyprlandData struct.

    HyprlandData *hyprland_data = NULL;
    hyprland_data = (HyprlandData *)malloc(sizeof(HyprlandData));
    if (hyprland_data == NULL) {
        perror("malloc");
        return NULL;
    }

    // Grabs the necessary hyprland data for our workspace indicator to work. We will process this
    // data in an algorithm in array_handling.c.
    int json_grab_result = grab_hyprland_indicator_data_json(hyprland_data);
    if (json_grab_result == -1) {
        fprintf(stderr, "Error: Failed to grab the necessary json data for the indicator.");
        free(hyprland_data);
        return NULL;
    }

    // There is a chance that Hyprland changes it's IPC or a severe error happens, so we warn the
    // user that the program either needs updating or that I need to be aware of the bug.
    if (!cJSON_IsArray(hyprland_data->monitors)) {
        fprintf(stderr, "Error: Monitors not a valid JSON array. Update or report this issue.");
        free(hyprland_data);
        return NULL;
    }
    hyprland_data->monitors_length = cJSON_GetArraySize(hyprland_data->monitors);

    if (!cJSON_IsArray(hyprland_data->workspaces)) {
        fprintf(stderr, "Error: Workspaces not a valid JSON array. Update or report this issue.");
        free(hyprland_data);
        return NULL;
    }
    hyprland_data->workspaces_length = cJSON_GetArraySize(hyprland_data->workspaces);

    // These are our arrays where we will store our processed data. These will be printed to json
    // format later. See bit_handling.c for an explanation as to why we use a 16-bit integer.
    uint16_t *workspace_array = NULL;
    workspace_array = (uint16_t *)malloc(hyprland_data->monitors_length * sizeof(uint16_t));
    if (workspace_array == NULL) {
        perror("malloc");
        free(hyprland_data);
        return NULL;
    }
    hyprland_data->workspace_array = workspace_array;

    uint16_t *activeworkspace_array = NULL;
    activeworkspace_array = (uint16_t *)malloc(hyprland_data->monitors_length * sizeof(uint16_t));
    if (activeworkspace_array == NULL) {
        perror("malloc");
        free(hyprland_data->workspace_array);
        free(hyprland_data);
        return NULL;
    }
    hyprland_data->activeworkspace_array = activeworkspace_array;

    return hyprland_data;
}

// No linting because this is a helper function to free all related resources
// NOLINTNEXTLINE(readability-non-const-parameter)
void delete_hyprland_data_structure(HyprlandData *hyprland_data) {
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

SocketData *initialise_socket_data_structure() {
    // Dynamically allocate our memory and error handle it in case the program does not have enough
    // available memory.
    SocketData *socket_data = (SocketData *)malloc(sizeof(SocketData));
    if (socket_data == NULL) {
        perror("malloc");
        return NULL;
    }

    socket_data->data_received = (char *)malloc(MAX_BUFFER_SIZE + 1);
    if (socket_data->data_received == NULL) {
        free(socket_data);
        perror("malloc");
        return NULL;
    }

    socket_data->poll_descriptor = (struct pollfd *)malloc(sizeof(struct pollfd));
    if (socket_data->poll_descriptor == NULL) {
        free(socket_data->data_received);
        free(socket_data);
        perror("malloc");
        return NULL;
    }

    socket_data->poll_descriptor->events = POLLIN;

    return socket_data;
}

// No linting because this is a helper function to free all related resources
// NOLINTNEXTLINE(readability-non-const-parameter)
void delete_socket_data_structure(SocketData *socket_data) {
    // The OS might not be able to close the socket so we warn the user of this.
    int close_result = close(socket_data->poll_descriptor->fd);
    if (close_result == -1) {
        fprintf(stderr, "Error closing socket: %s\n", strerror(errno));
    }

    free(socket_data->data_received);
    socket_data->data_received = NULL;

    free(socket_data->poll_descriptor);
    socket_data->poll_descriptor = NULL;

    free(socket_data);
}
