// data_structures.h

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <cjson/cJSON.h>

#include <stdint.h>

#include <poll.h>

typedef struct {
    cJSON * monitors;
    cJSON * workspaces;
    cJSON * activeworkspace;

    int monitors_length;
    int workspaces_length;

    uint16_t * workspace_array;
    uint16_t * activeworkspace_array;
} HyprlandData;

struct pollfd;

typedef struct {
    char * data_received;
    struct pollfd *poll_descriptor;
} SocketData;

extern SocketData * events_data_global;
// The use of a global variable might seem like a strange choice here, but this is because
// signal handlers don't accept any other arguments other than the signum. We set the global
// variable to point to a local variable which is passed amongst multiple functions in
// event_handling.c and hyprland_socket_handling.c, which is done to allow reusability of the code
// without the use of a global variable.

#endif
