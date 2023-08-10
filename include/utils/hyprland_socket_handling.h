// hyprland_socket_handling.h

#ifndef HYPRLAND_SOCKET_HANDLING_H
#define HYPRLAND_SOCKET_HANDLING_H

char * get_hyprland_socket(Socket socket); // SOCKET is for simply "socket", which only handles requests; SOCKET2 is for "socket2", which handles events
int * grab_information_from_hyprland_socket(Socket socket_type, SocketData * socket_data);
cJSON * grab_json_from_socket_data(const char * cmd, SocketData * socket_data);

#endif
