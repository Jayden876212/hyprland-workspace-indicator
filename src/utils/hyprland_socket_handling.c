// Include a safer version of snprintf if provided by the standard library implementation
#if defined(_MSC_VER)
#define __STDC_WANT_LIB_EXT1__ 1
#define SAFE_SNPRINTF snprintf_s
#else
#define SAFE_SNPRINTF snprintf // glibc does not yet support snprintf_s
#endif

#include "data/constants.h"                    // for SOCKET, Socket, HIS_PATH_BUFFER_SIZE, MAX_BUFFER_SIZE, SOCKET2
#include "data/data_structures.h"              // for SocketData
#include "helpers/hyprland_struct_handling.h"  // for delete_socket_data_structure
#include <cjson/cJSON.h>                       // for cJSON_GetErrorPtr, cJSON_Parse, cJSON
#include <poll.h>                              // for pollfd
#include <stdio.h>                             // for fprintf, perror, NULL, stderr, ssize_t, snprintf
#include <stdlib.h>                            // for free, getenv, calloc, realloc
#include <string.h>                            // for strlen, strdup, strncat, strncpy
#include <sys/socket.h>                        // for connect, recv, send, socket, AF_UNIX, SOCK_STREAM
#include <sys/un.h>                            // for sockaddr_un

char *get_hyprland_socket(Socket socket_type) {
    // Grab the hyprland_instance_signature for use in the socket path.
    const char *hyprland_instance_signature = getenv("HYPRLAND_INSTANCE_SIGNATURE");
    if (hyprland_instance_signature == NULL) {
        fprintf(stderr, "Error: no HYPRLAND_INSTANCE_SIGNATURE. Make sure hyprland is running.\n");
        return NULL;
    }

    // Grab the xdg_runtime_dir for use in the socket path.
    const char *xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    if (xdg_runtime_dir == NULL) {
        fprintf(stderr, "Error: no XDG_RUNTIME_DIR. Make sure hyprland is running.\n");
        return NULL;
    }

    // Add the rest of the path to the hyprland_instance_signature.
    int his_buffer_size = 0;
    his_buffer_size = (int)strlen(xdg_runtime_dir) + (int)strlen(hyprland_instance_signature) +
                      HIS_PATH_BUFFER_SIZE;

    // Configure the socket path depending on the socket given.
    // SOCKET is for "socket" (handles requests); SOCKET2 is for "socket2", (handles events).
    char *socket_name_string = NULL;
    if (socket_type == SOCKET) {
        socket_name_string = "socket";
    } else if (socket_type == SOCKET2) {
        socket_name_string = "socket2";
        his_buffer_size += 1;
    } else {
        fprintf(stderr, "Error: invalid socket\n");
        return NULL;
    }

    // Concatenate the hyprland_instance_signature, socket_name and the rest of the path
    // to get the full path to the socket.
    char socket_path[his_buffer_size];
    int chars_written =
        SAFE_SNPRINTF(socket_path, his_buffer_size, "%s/hypr/%s/.%s.sock", xdg_runtime_dir,
                      hyprland_instance_signature, socket_name_string);
    if (chars_written == -1) {
        perror("snprintf");
        return NULL;
    }

    // Dynamically allocate a duplicate of the string for use outside of the function.
    char *socket_path_duplicate = strdup(socket_path);
    if (socket_path_duplicate == NULL) {
        perror("strdup");
        free(socket_path_duplicate);
        return NULL;
    }

    return socket_path_duplicate; // It is the function user's job to free() the string.
}

int set_up_hyprland_socket(Socket socket_type, SocketData *socket_data) {
    // Grab the path of the hyprland socket to communicate with it.
    char *socket_path = get_hyprland_socket(socket_type);
    if (socket_path == NULL) {
        fprintf(stderr, "Error: Failed to obtain hyprland socket path.\n");
        free(socket_path);
        return -1;
    }

    // Set file descriptor to the unix socket (to communicate with it) and initialise unix socket.
    socket_data->poll_descriptor->fd = socket(AF_UNIX, SOCK_STREAM, 0);
    int socket_file_descriptor = socket_data->poll_descriptor->fd;
    if (socket_file_descriptor == -1) {
        perror("socket");
        free(socket_path);
        return -1;
    }

    // Set up socket address family and duplicate allocated path to address path.
    // (this is necessary for it to be used to receive data from hyprland).
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);
    free(socket_path);
    if (connect(socket_file_descriptor, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("connect");
        return -1;
    }

    return 0;
}

// Looped version of recv that dynamically allocates a growing chunk of memory to store data
// retrieved.
char *recv_cat(int file_descriptor, ssize_t buffer_size, int flags) {
    char *full_data = (char *)calloc(1, buffer_size);
    if (full_data == NULL) {
        perror("calloc");
        return NULL;
    }
    ssize_t signed_buffer_size = buffer_size;
    ssize_t num_bytes_received = buffer_size;
    ssize_t cur_buffer_size = 0;

    while (num_bytes_received == signed_buffer_size) {
        char buffer[buffer_size];

        num_bytes_received = recv(file_descriptor, buffer, buffer_size, flags);
        if (num_bytes_received == -1) {
            perror("recv");
        } else if (num_bytes_received == 0) {
            fprintf(stderr, "Error: Connection closed by the server.\n");
        }

        if (num_bytes_received < 1) {
            free(full_data);
            return NULL;
        }

        buffer[num_bytes_received] = '\0';
        cur_buffer_size += buffer_size;

        void *temporary_buffer = realloc(full_data, cur_buffer_size);
        if (temporary_buffer == NULL) {
            free(full_data);
            perror("realloc");
            return NULL;
        }
        full_data = temporary_buffer;
        strncat(full_data, buffer, num_bytes_received);
    }

    return full_data;
}

cJSON *grab_json_from_socket_data(const char *command, SocketData *socket_data) {
    // Set up the hyprland socket so we can receive the information to be parsed from hyprland.
    if (set_up_hyprland_socket(SOCKET, socket_data) == -1) {
        fprintf(stderr, "Error: Failed to set up hyprland socket\n");
        return NULL;
    }

    // Ask for data from the socket using the command given so we can choose what info we need.
    int socket_file_descriptor = socket_data->poll_descriptor->fd;
    if (send(socket_file_descriptor, command, strlen(command), 0) == -1) {
        perror("send");
        return NULL;
    }

    socket_data->data_received = recv_cat(socket_file_descriptor, MAX_BUFFER_SIZE, 0);
    if (socket_data->data_received == NULL) {
        fprintf(stderr, "Error: Failed to receive socket data into dynamic pool of memory.");
    }

    // Parse the data using cJSON as json so we can easily access different information.
    cJSON *bufferjson = cJSON_Parse(socket_data->data_received);
    if (bufferjson == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON parsing error at : %s\n", error_ptr);
        }
        return NULL;
    }

    // Handle the closing of the socket and any dynamic memory associated with it to avoid errors.
    delete_socket_data_structure(socket_data);

    return bufferjson; // It is up to the user to free the json buffer using cJSON_Delete().
}
