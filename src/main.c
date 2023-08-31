#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>

#include <cjson/cJSON.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

// include/helpers
#include "helpers/json_handling.h"
#include "helpers/hyprland_struct_handling.h"

// include/utils
#include "utils/hyprland_socket_handling.h"
#include "utils/event_handling.h"

// Initialise global variable so it can be cleaned up with sig_int_handler.
SocketData * events_data_global = NULL;
// The use of a global variable might seem like a strange choice here, but this is because
// signal handlers don't accept any other arguments other than the signum. We set the global
// variable to point to a local variable which is passed amongst multiple functions in
// event_handling.c and hyprland_socket_handling.c, which is done to allow reusability of the code
// without the use of a global variable.

static void sig_int_handler(int signum) {
    delete_socket_data_structure(events_data_global);
    fprintf(stderr, "Exiting due to interrupt. (signal = %d)\n", signum);
    exit(signum);
}

int main() {
    signal(SIGINT, sig_int_handler);

    SocketData * events_data = initialise_socket_data_structure();
    if (events_data == NULL) {
        fprintf(stderr, "Error: Failed to allocate socket data structure. Exiting.\n");
        exit(EXIT_FAILURE);
    }
    events_data_global = events_data;

    int hyprland_socket_creation_result = set_up_hyprland_socket(SOCKET2, events_data);
    if (hyprland_socket_creation_result == -1) {
        fprintf(stderr, "Error: unable to set up hyprland socket. Exiting.\n");
        delete_socket_data_structure(events_data);
        exit(EXIT_FAILURE);
    }

    void (*event_handler)(SocketData *,int (*)()) = handle_workspace_socket_events;
    int (*function_executed)() = initialise_and_print_workspace_info_as_json;
    
    while (1) {
        int poll_result = poll_for_socket_events(events_data, event_handler, function_executed);
        if (poll_result == -1) {
            fprintf(stderr, "Error: failed to poll for socket events. Exiting.\n");
            delete_socket_data_structure(events_data);
            exit(EXIT_FAILURE);
        }
    }

    delete_socket_data_structure(events_data);

    return 0;
}
