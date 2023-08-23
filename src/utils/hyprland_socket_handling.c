#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <cjson/cJSON.h>

// include/data
#include "data/constants.h"
#include "data/data_structures.h"

// include/helpers
#include "helpers/hyprland_struct_handling.h"

char * get_hyprland_socket(Socket socket_type) { 
    // Grab the hyprland_instance_signature for use in the socket path
    const char * hyprland_instance_signature = getenv("HYPRLAND_INSTANCE_SIGNATURE");
    if (hyprland_instance_signature == NULL) {
        fprintf(stderr, "Error: hyprland instance signature not found. Make sure hyprland is running.\n");
        return NULL;
    }


    // Add the rest of the path to the hyprland_instance_signature 
    int his_buffer_size = strlen(hyprland_instance_signature) + HIS_PATH_BUFFER_SIZE;

    // Configure the socket path depending on the socket given
    // SOCKET is for simply "socket", which only handles requests; SOCKET2 is for "socket2", which handles events
    char * socket_name_string;
    if (socket_type == SOCKET) {
        socket_name_string = "socket";
    } else if (socket_type == SOCKET2) {
        socket_name_string = "socket2";
        his_buffer_size += 1;
    } else {
        fprintf(stderr, "Error: invalid socket\n");
        return NULL;
    }

    // Concatenate the hyprland_instance_signature, socket_name and the rest of the path to get the full path to the socket
    char socket_path[his_buffer_size];
    int chars_written = snprintf(socket_path, his_buffer_size, "/tmp/hypr/%s/.%s.sock", hyprland_instance_signature, socket_name_string);
    if (chars_written == -1) {
        perror("malloc");
        return NULL;
    }

    // Dynamically allocate a duplicate of the string for use outside of the function
    char * socket_path_duplicate = strdup(socket_path);
    if (socket_path_duplicate == NULL) {
        perror("strdup");
        free(socket_path_duplicate);
        return NULL;
    }

    return socket_path_duplicate; // It is the function user's job to free() the string
}

int set_up_hyprland_socket(Socket socket_type, SocketData * socket_data) {
    // Grab the path of the hyprland socket to communicate with it
    char * socket_path = get_hyprland_socket(socket_type);
    if (socket_path == NULL) {
        fprintf(stderr, "Error: Failed to obtain hyprland socket path.\n");
        free(socket_path);
        return -1;
    }

    // Set file descriptor to the unix socket (to communicate with it) and initialise unix socket
    socket_data->poll_descriptor->fd = socket(AF_UNIX, SOCK_STREAM, 0);
    int socket_file_descriptor = socket_data->poll_descriptor->fd;
    if (socket_file_descriptor == -1) {
        perror("socket");
        free(socket_path);
        return -1;
    }

    // Set up socket address family and duplicate allocated path to address path
    // (this is necessary for it to be used to receive data from hyprland)
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);
    free(socket_path);
    if (connect(socket_file_descriptor, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("connect");
        return -1;
    }

    return 0;
}

cJSON * grab_json_from_socket_data(const char * command, SocketData * socket_data) {
    // 
    if (set_up_hyprland_socket(SOCKET, socket_data) == -1) {
        fprintf(stderr, "Error: Failed to grab information from hyprland socket\n");
        return NULL;
    }

    int socket_file_descriptor = socket_data->poll_descriptor->fd;
    char * data_received = socket_data->data_received;
    
    send(socket_file_descriptor, command, strlen(command), 0);

    ssize_t num_bytes_received = recv(socket_file_descriptor, data_received, MAX_BUFFER_SIZE, 0);
    if (num_bytes_received == -1) {
        perror("recv");
        free(data_received);
        return NULL;
    }

    data_received[num_bytes_received] = '\0';

    cJSON * bufferjson = cJSON_Parse(data_received);
    if (bufferjson == NULL) {
        const char * error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON parsing error at : %s\n", error_ptr);
        }
        free(data_received);
        return NULL;
    }

    close(socket_file_descriptor);

    free(data_received);
    socket_data->data_received = NULL;

    free(socket_data->poll_descriptor);
    socket_data->poll_descriptor = NULL;

    return bufferjson;
}
