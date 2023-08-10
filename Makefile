# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O3 -march=native -mtune=native -g

# Directories
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# Target executable name
TARGET = $(BIN_DIR)/workspaces

# Source files
SRCS = $(shell find src -name '*.c')

# Include paths
INCLUDES = -I$(INC_DIR)

# Libraries to link with
LIBS = -lcjson

# Phony targets
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(SRCS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(TARGET) $(SRCS)
