#include <stdio.h>

#include <sys/socket.h>

#include <string.h>
#include <poll.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

int poll_for_socket_events(SocketData * events_data, void (*event_processor)(), int (*function_executed)()) {
    if (poll(events_data->poll_descriptor, 1, -1) == -1) {
        perror("poll");
        return -1;
    }
    if (events_data->poll_descriptor->revents & POLLIN) {
        int * file_descriptor = &(events_data->poll_descriptor->fd);
        char * data_received = events_data->data_received;
        ssize_t bytes_received = recv(*file_descriptor, data_received, MAX_BUFFER_SIZE + 1, 0);
        if (bytes_received == -1) {
            perror("recv");
            return -1;
        } else if (bytes_received == 0) {
            fprintf(stderr, "Error: Connection closed by the server.\n");
            return -1;
        }

        events_data->data_received[bytes_received] = '\0';
        event_processor(events_data, function_executed);
    }

    return 0;
}

void handle_workspace_socket_events(SocketData * events_data, int (*function_executed)()) {
    char * data_received = events_data->data_received;

    if (strstr(data_received, EVENT_WORKSPACE_CHANGED) ||
        strstr(data_received, EVENT_MONITOR_CHANGED)) {
        function_executed();
    }
}
