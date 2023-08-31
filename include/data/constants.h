// constants.h

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_BUFFER_SIZE 2048
#define HIS_PATH_BUFFER_SIZE 24
#define WORKSPACES_AMOUNT 10

#define BITWISE_FALSE 0

#define EVENT_WORKSPACE_CHANGED "workspace>>"
#define EVENT_MONITOR_CHANGED "focusedmon>>"

#define CMD_MONITORS "-j/monitors"
#define CMD_WORKSPACES "-j/workspaces"
#define CMD_ACTIVEWORKSPACE "-j/activeworkspace"

typedef enum {
    SOCKET,
    SOCKET2
} Socket;

typedef enum {
    WORKSPACES,
    ACTIVEWORKSPACES
} WorkspacesType;

#endif
