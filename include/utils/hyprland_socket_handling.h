// hyprland_socket_handling.h

#ifndef HYPRLAND_SOCKET_HANDLING_H
#define HYPRLAND_SOCKET_HANDLING_H

#include <stdio.h>
#include <cjson/cJSON.h>

#include "data/data_structures.h"
#include "data/constants.h"

char *
get_hyprland_socket(Socket socket); // SOCKET is for simply "socket", which only handles requests;
                                    // SOCKET2 is for "socket2", which handles events
int set_up_hyprland_socket(Socket socket_type, SocketData *socket_data);
char *recv_cat(int file_descriptor, size_t buffer_size, int flags);
cJSON *grab_json_from_socket_data(const char *command, SocketData *socket_data);

#endif
