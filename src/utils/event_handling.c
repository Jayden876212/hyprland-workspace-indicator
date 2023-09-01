#include <stdio.h>

#include <sys/socket.h>

#include <string.h>
#include <poll.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

int poll_for_socket_events(SocketData * events_data, int (*event_processor)(),
int (*function_executed)()) {
    // Poll the server for events that we can check for.
    if (poll(events_data->poll_descriptor, 1, -1) == -1) {
        perror("poll");
        return -1;
    }

    // Check if the server has data ready to be read by comparing the constant POLLIN using AND.
    if (events_data->poll_descriptor->revents & POLLIN) {
        // Initialise pointers to simplify the reading of the code.
        int * file_descriptor = &(events_data->poll_descriptor->fd);
        char * data_received = events_data->data_received;

        // Receive data from the server.
        ssize_t bytes_received = recv(*file_descriptor, data_received, MAX_BUFFER_SIZE + 1, 0);
        if (bytes_received == -1) {
            perror("recv");
            return -1;
        } else if (bytes_received == 0) {
            fprintf(stderr, "Error: Connection closed by the server.\n");
            return -1;
        }

        // Process the events using a user-specified event processor so we can check what events we
        // want to respond to. The response is carrying out the user-specified function_executed
        // function.
        int process_result = event_processor(events_data, function_executed);
        if (process_result == -1) {
            fprintf(stderr, "Error: Event processor failed to respond to event.\n");
            return -1;
        }
    }

    return 0;
}

int handle_workspace_socket_events(SocketData * events_data, int (*function_executed)()) {
    // Initialise pointer for better readability.
    char * data_received = events_data->data_received;

    // Check if the user switches workspace or monitor so we can respond to it with the
    // user-specified function.
    if (strstr(data_received, EVENT_WORKSPACE_CHANGED) ||
        strstr(data_received, EVENT_MONITOR_CHANGED)) {
        int function_result = function_executed();
        if (function_result == -1) {
            fprintf(stderr, "Error: Response function that was expected failed.\n");
            return -1;
        }
    }
    return 0;
}
