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

// Set up a function for the signal handler so we can cleanup memory and print a message when the
// user of the program closes the process.
static void sig_int_handler(int signum) {
    delete_socket_data_structure(events_data_global);
    fprintf(stderr, "Exiting due to interrupt. (signal = %d)\n", signum);
    exit(signum);
}

int main() {
    // Set up the signal handler using our signal handler function so we can use defined behaviour
    // when the user closes the program.
    signal(SIGINT, sig_int_handler);

    // Set up the data structure for hyprland's events so we can print the workspace information
    // only when it's needed. This will reduce performance overhead as we won't be constantly
    // printing the workspace information in a loop, rather we will be checking for when the
    // information actually changes.
    SocketData * events_data = initialise_socket_data_structure();
    if (events_data == NULL) {
        fprintf(stderr, "Error: Failed to allocate socket data structure. Exiting.\n");
        exit(EXIT_FAILURE); // We exit the program because without a connection to a socket, it is
                            // essentially useless.
    }
    // Set the global variable to the events_data struct so we may use it in our cleanup after the
    // program is closed.
    events_data_global = events_data;

    // Set up the hyprland socket so it can be used to poll for events.
    int hyprland_socket_creation_result = set_up_hyprland_socket(SOCKET2, events_data);
    if (hyprland_socket_creation_result == -1) {
        fprintf(stderr, "Error: unable to set up hyprland socket. Exiting.\n");
        delete_socket_data_structure(events_data);
        exit(EXIT_FAILURE);
    }

    // Intialise function pointers so we can choose what behaviour we want when we check what events
    // are outputted.
    // The function event_handler processes events so we can check when the user interacts with the
    // compositor.
    void (*event_handler)(SocketData *,int (*)()) = handle_workspace_socket_events;
    // The function function_executed is to decide what action to take upon the event_handler's
    // condition having success (e.g. when the workspace is changed or when the monitor is
    // focused.)
    // We use the function initialise_and_print_workspace_info_as_json because that is the purpose
    // of the program.
    int (*function_executed)() = initialise_and_print_workspace_info_as_json;
    
    while (1) {
        // Poll for when the user interacts with the compositor. In this case, it checks for when
        // the focused workspace or monitor is changed, which will reduce overhead as mentioned
        // earlier.
        int poll_result = poll_for_socket_events(events_data, event_handler, function_executed);
        if (poll_result == -1) {
            fprintf(stderr, "Error: failed to poll for socket events. Exiting.\n");
            delete_socket_data_structure(events_data);
            exit(EXIT_FAILURE);
        }
    }

    // Cleanup memory when done.
    delete_socket_data_structure(events_data);

    return 0;
}
