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
    const char * hyprland_instance_signature = getenv("HYPRLAND_INSTANCE_SIGNATURE");
    if (hyprland_instance_signature == NULL) {
        fprintf(stderr, "Error: hyprland instance signature not found. Make sure hyprland is running.\n");
        return NULL;
    }

    int his_buffer_size = strlen(hyprland_instance_signature) + HIS_PATH_BUFFER_SIZE;
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

    char socket_path[his_buffer_size];
    int chars_written = snprintf(socket_path, his_buffer_size, "/tmp/hypr/%s/.%s.sock", hyprland_instance_signature, socket_name_string);
    if (chars_written == -1) {
        perror("malloc");
        return NULL;
    }

    char * socket_path_duplicate = strdup(socket_path);
    if (socket_path_duplicate == NULL) {
        perror("strdup");
        free(socket_path_duplicate);
        return NULL;
    }

    return socket_path_duplicate;
}

int grab_information_from_hyprland_socket(Socket socket_type, SocketData * socket_data) {
    char * socket_path = get_hyprland_socket(socket_type);
    struct sockaddr_un address;

    socket_data->poll_descriptor->fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if ((socket_data->poll_descriptor->fd) == -1) {
        perror("socket");
        free(socket_path);
        return -1;
    }

    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);
    free(socket_path);

    if (connect(socket_data->poll_descriptor->fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("connect");
        return -1;
    }

    return 0;
}

cJSON * grab_json_from_socket_data(const char * cmd, SocketData * socket_data) {
    grab_information_from_hyprland_socket(SOCKET, socket_data);

    const char * message = cmd;
    send(socket_data->poll_descriptor->fd, message, strlen(message), 0);


    ssize_t num_bytes_received = recv(socket_data->poll_descriptor->fd, socket_data->data_received, MAX_BUFFER_SIZE, 0);
    if (num_bytes_received == -1) {
        perror("recv");
        free(socket_data->data_received);
        return NULL;
    }

    socket_data->data_received[num_bytes_received] = '\0';



    cJSON * bufferjson = cJSON_Parse(socket_data->data_received);
    if (bufferjson == NULL) {
        const char * error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON parsing error at : %s\n", error_ptr);
        }
        free(socket_data->data_received);
        return NULL;
    }

    close(socket_data->poll_descriptor->fd);

    free(socket_data->data_received);
    socket_data->data_received = NULL;

    free(socket_data->poll_descriptor);
    socket_data->poll_descriptor = NULL;

    return bufferjson;
}
