CC = clang++
CFLAGS = -Wall -O2 -I./src
SRC_DIR = ./src
BUILD_DIR = ./build
LIB_NAME = libflisp.a
EXEC_NAME = flisp

SRC_FILES = $(filter-out $(SRC_DIR)/main.cc, $(wildcard $(SRC_DIR)/*.cc))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SRC_FILES))

all: $(BUILD_DIR)/$(LIB_NAME) $(BUILD_DIR)/$(EXEC_NAME)

$(BUILD_DIR)/$(LIB_NAME): $(OBJ_FILES)
	ar rcs $@ $^

$(BUILD_DIR)/$(EXEC_NAME): $(BUILD_DIR)/main.o $(BUILD_DIR)/$(LIB_NAME)
	$(CC) $^ -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cc | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run:
	$(BUILD_DIR)/$(EXEC_NAME)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean all
