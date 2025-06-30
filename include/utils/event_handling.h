// event_handling.h

#ifndef EVENT_HANDLING_H
#define EVENT_HANDLING_H

#include "data/data_structures.h"

int poll_for_socket_events(SocketData *events_data, Callback event_processor,
                           Callback function_executed);

int handle_workspace_socket_events(SocketData *events_data, Callback function_executed);

#endif
