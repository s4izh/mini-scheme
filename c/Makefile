.PHONY: build

SRC_DIR = src
GENERATED_DIR = src/generated
BUILD_DIR = build
TARGET = $(BUILD_DIR)/mscm

.DEFAULT_GOAL := $(TARGET)

CFLAGS = -Wall -Wextra -pedantic -std=c99 -g3 -MMD -MP -lreadline
INCLUDES = -I./src
CC = gcc

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

$(SRC_DIR)/scm_lexer_generated.c: docs/test.puml
	python3 docs/test.py --input docs/test.puml --output $(SRC_DIR)/scm_lexer_generated.c --state-prefix SCM_LEXER_NFA_
	sed -i '1i #include "scm_lexer.h"' $(SRC_DIR)/scm_lexer_generated.c
	sed -i '2i #include "nfa.h"' $(SRC_DIR)/scm_lexer_generated.c

$(TARGET): $(OBJS) $(BUILD_DIR)/scm_lexer_generated.o $(SRC_DIR)/scm_lexer_generated.c 
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rvf $(SRC_DIR)/scm_lexer_generated.c
	@rm -rvf $(BUILD_DIR)

nix:
	@rm result || exit 0
	nix build .#mscm && ./result/bin/mscm
