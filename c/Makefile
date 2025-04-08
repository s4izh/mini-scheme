.PHONY: build

SRC_DIR = src
TEST_SRC_DIR = $(SRC_DIR)/tests
GENERATED_DIR = $(SRC_DIR)/generated
BUILD_DIR = build
TEST_BUILD_DIR = $(BUILD_DIR)/tests
TARGET = $(BUILD_DIR)/mscm
TARGET_TEST = $(TEST_BUILD_DIR)/mscm_test_suite
EXTERNAL = external

.DEFAULT_GOAL := $(TARGET)

# CFLAGS = -fsanitize=address -Wall -Wextra -pedantic -Werror=switch -std=c99 -g3 -MMD -MP -lreadline
# CFLAGS = -fsanitize=address -std=c99 -g3 -MMD -MP -lreadline
CFLAGS = -std=c99 -ggdb -MMD -MP -lreadline
INCLUDES = -I./src -I./external -I./src/tests
CC = gcc

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJS = $(SRCS:$(SRC_DIR)/%.c=$(TEST_BUILD_DIR)/%.o) \
            $(TEST_SRCS:$(TEST_SRC_DIR)/%.c=$(TEST_BUILD_DIR)/%.o)

MUNIT_OBJS = $(BUILD_DIR)/munit.o

$(SRC_DIR)/scm_lexer_generated.c: docs/test.puml
	python3 docs/test.py --input docs/test.puml --output $(SRC_DIR)/scm_lexer_generated.c --state-prefix SCM_LEXER_NFA_
	sed -i '1i #include "scm_lexer.h"' $(SRC_DIR)/scm_lexer_generated.c
	sed -i '2i #include "nfa.h"' $(SRC_DIR)/scm_lexer_generated.c

$(TARGET): $(OBJS) $(BUILD_DIR)/scm_lexer_generated.o $(SRC_DIR)/scm_lexer_generated.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET)

$(TARGET_TEST): $(TEST_OBJS) $(MUNIT_OBJS) $(TEST_BUILD_DIR)/scm_lexer_generated.o $(SRC_DIR)/scm_lexer_generated.c
	@mkdir -p $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_OBJS) $(MUNIT_OBJS) -o $(TARGET_TEST)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# src built with TEST_MODE enabled
$(TEST_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -DTEST_MODE -c $< -o $@

# specific test files
$(TEST_BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.c
	@mkdir -p $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -DTEST_MODE -c $< -o $@

$(BUILD_DIR)/munit.o: $(EXTERNAL)/munit/munit.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

repl: $(TARGET)
	./$(TARGET)

test: $(TARGET_TEST)
	./$(TARGET_TEST)

clean:
	@rm -rvf $(SRC_DIR)/scm_lexer_generated.c
	@rm -rvf $(BUILD_DIR)

nix:
	@rm result || exit 0
	nix build .#mscm && ./result/bin/mscm
