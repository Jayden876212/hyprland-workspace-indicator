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

SocketData * events_data = NULL;

void sig_int_handler(int signum) {
    delete_socket_data_structure(events_data);
    fprintf(stderr, "Exiting due to interrupt. (sigint)\n");
    exit(signum);
}

void poll_for_socket_events(void (*event_processor)(), int (*function_executed)()) {
    if (poll(events_data->poll_descriptor, 1, -1) == -1) {
        perror("poll");
        delete_socket_data_structure(events_data);
        exit(EXIT_FAILURE);
    }
    if (events_data->poll_descriptor->revents & POLLIN) {
        int * file_descriptor = &(events_data->poll_descriptor->fd);
        char * data_received = events_data->data_received;
        ssize_t bytes_received = recv(*file_descriptor, data_received, MAX_BUFFER_SIZE + 1, 0);
        if (bytes_received == -1) {
            perror("recv");
            delete_socket_data_structure(events_data);
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            fprintf(stderr, "Error: Connection closed by the server.\n");
            delete_socket_data_structure(events_data);
            exit(EXIT_FAILURE);
        }

        events_data->data_received[bytes_received] = '\0';
        event_processor(function_executed);
    }
}

void handle_workspace_socket_events(int (*function_executed)()) {
    char * data_received = events_data->data_received;
    char * search_string_workspace = EVENT_WORKSPACE_CHANGED;
    char * search_string_focusedmon = EVENT_MONITOR_CHANGED;
    if (strstr(data_received, search_string_workspace) ||
        strstr(data_received, search_string_focusedmon)) {
        function_executed();
    }
}

int main() {
    signal(SIGINT, sig_int_handler);

    events_data = initialise_socket_data_structure();
    if (events_data == NULL) {
        fprintf(stderr, "Error: Failed to allocate socket data structure.\n");
        exit(EXIT_FAILURE);
    }

    int hyprland_socket_creation_result = set_up_hyprland_socket(SOCKET2, events_data);
    if (hyprland_socket_creation_result == -1) {
        fprintf(stderr, "Error: unable to set up hyprland socket.\n");
        delete_socket_data_structure(events_data);
        exit(EXIT_FAILURE);
    }

    void (*event_handler)(int (*func_executed)()) = handle_workspace_socket_events;
    int (*function_executed)() = initialise_and_print_workspace_info_as_json;
    
    while (1) {
        poll_for_socket_events(event_handler, function_executed);
    }

    delete_socket_data_structure(events_data);

    return 0;
}

