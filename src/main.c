#include <stdio.h>
#include <stdlib.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

// include/helpers
#include "helpers/hyprland_struct_handling.h"
#include "helpers/json_handling.h"

// include/utils
#include "utils/event_handling.h"
#include "utils/hyprland_socket_handling.h"

int main() {
    SocketData *events_data = NULL;

    // We do not need to have a signal handler because the operating system handles closing file
    // descriptors when the user closes the program.
    // (See https://stackoverflow.com/questions/9022495/freeing-memory-inside-a-signal-handler)

    // Set up the data structure for hyprland's events so we can print the workspace information
    // only when it's needed. This will reduce performance overhead as we won't be constantly
    // printing the workspace information in a loop, rather we will be checking for when the
    // information actually changes.
    events_data = initialise_socket_data_structure();
    if (events_data == NULL) {
        fprintf(stderr, "Error: Failed to allocate socket data structure. Exiting.\n");
        exit(EXIT_FAILURE); // We exit the program because without a connection to a socket, it is
                            // essentially useless.
    }

    // Set up the hyprland socket so it can be used to poll for events.
    int hyprland_socket_creation_result = set_up_hyprland_socket(SOCKET2, events_data);
    if (hyprland_socket_creation_result == -1) {
        fprintf(stderr, "Error: Unable to set up hyprland socket. Exiting.\n");
        delete_socket_data_structure(events_data);
        exit(EXIT_FAILURE);
    }

    // The function event_handler processes events so we can check when the user interacts with the
    // compositor.
    EventHandler event_handler = handle_workspace_socket_events;

    // The function function_executed is to decide what action to take upon the event_handler's
    // condition having success (e.g. when the workspace is changed or when the monitor is
    // focused.)
    // We use the function initialise_and_print_workspace_info_as_json because that is the purpose
    // of the program.
    Callback function_executed = initialise_and_print_workspace_info_as_json;

    while (1) {
        // Poll for when the user interacts with the compositor. In this case, it checks for when
        // the focused workspace or monitor is changed, which will reduce overhead as mentioned
        // earlier.
        int poll_result = poll_for_socket_events(events_data, event_handler, function_executed);
        if (poll_result == -1) {
            fprintf(stderr, "Error: Failed to poll for socket events. Exiting.\n");
            delete_socket_data_structure(events_data);
            exit(EXIT_FAILURE);
        }
    }

    // Cleanup memory when done.
    delete_socket_data_structure(events_data);

    return 0;
}
