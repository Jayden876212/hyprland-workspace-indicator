// event_handling.h

#ifndef EVENT_HANDLING_H
#define EVENT_HANDLING_H

int poll_for_socket_events(SocketData * events_data, void (*event_processor)(), int (*function_executed)());

void handle_workspace_socket_events(SocketData * events_data, int (*function_executed)());

#endif
