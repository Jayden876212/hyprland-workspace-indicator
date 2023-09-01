// hyprland_struct_handling.h

#ifndef HYPRLAND_STRUCT_HANDLING_H
#define HYPRLAND_STRUCT_HANDLING_H

cJSON * allocate_and_grab_json(const char * cmd);
int grab_hyprland_indicator_data_json(HyprlandData * hyprland_data);
HyprlandData * initialise_hyprland_data_structure();
void delete_hyprland_data_structure(HyprlandData * hyprland_data);
SocketData * initialise_socket_data_structure();
void cleanup_socket_data_structure(SocketData * socket_data);
void delete_socket_data_structure();

#endif
