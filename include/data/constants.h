// constants.h

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_BUFFER_SIZE 1024
#define HIS_PATH_BUFFER_SIZE 24
#define WORKSPACES_AMOUNT 10

#define BITWISE_FALSE 0
#define TRUE_BIT_POSITION_OFFSET 15
#define TOTAL_BITS_IN_INT 16
#define SINGULAR_BIT 0b1000000000000000
#define RIGHTMOST_BIT 0b0000000000000001

#define EVENT_WORKSPACE_CHANGED "workspace>>"
#define EVENT_MONITOR_CHANGED "focusedmon>>"

#define CMD_MONITORS "-j/monitors"
#define CMD_WORKSPACES "-j/workspaces"
#define CMD_ACTIVEWORKSPACE "-j/activeworkspace"

typedef enum { SOCKET, SOCKET2 } Socket;

typedef enum { WORKSPACES, ACTIVEWORKSPACES } WorkspacesType;

#endif
