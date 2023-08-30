// event_handling.h

#ifndef EVENT_HANDLING_H
#define EVENT_HANDLING_H

int poll_for_socket_events(void (*event_processor)(), int (*function_executed)());

void handle_workspace_socket_events(int (*function_executed)());

#endif
