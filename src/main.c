#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

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
#include "utils/event_handling.h"

SocketData * events_data = NULL;

void sig_int_handler(int signum) {
    delete_socket_data_structure(events_data);
    fprintf(stderr, "Exiting due to interrupt. (signal = %d)\n", signum);
    exit(signum);
}

int main() {
    signal(SIGINT, sig_int_handler);

    events_data = initialise_socket_data_structure();
    if (events_data == NULL) {
        fprintf(stderr, "Error: Failed to allocate socket data structure. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    int hyprland_socket_creation_result = set_up_hyprland_socket(SOCKET2, events_data);
    if (hyprland_socket_creation_result == -1) {
        fprintf(stderr, "Error: unable to set up hyprland socket. Exiting.\n");
        delete_socket_data_structure(events_data);
        exit(EXIT_FAILURE);
    }

    void (*event_handler)(int (*func_executed)()) = handle_workspace_socket_events;
    int (*function_executed)() = initialise_and_print_workspace_info_as_json;
    
    while (1) {
        int poll_result = poll_for_socket_events(event_handler, function_executed);
        if (poll_result == -1) {
            fprintf(stderr, "Error: failed to poll for socket events. Exiting.\n");
            delete_socket_data_structure(events_data);
            exit(EXIT_FAILURE);
        }
    }

    delete_socket_data_structure(events_data);

    return 0;
}

