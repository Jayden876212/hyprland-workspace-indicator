// constants.h

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_BUFFER_SIZE 2048
#define HIS_PATH_BUFFER_SIZE 24
#define WORKSPACES_AMOUNT 10

#define BITWISE_FALSE 0
//#define BIT10_MASK 0b1111111111000000
#define LEFTMOST_SINGULAR_BIT 0b1000000000000000

typedef enum {
    SOCKET,
    SOCKET2
} Socket;

typedef enum {
    WORKSPACES,
    ACTIVEWORKSPACES
} WorkspacesType;

#endif
