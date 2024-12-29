.PHONY: build

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/main

CFLAGS = -Wall -Wextra -pedantic -std=c99
# INCLUDES = -Iinclude
CC = gcc

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rvf $(BUILD_DIR)
