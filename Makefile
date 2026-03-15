CC = clang
FLAGS = -Wall -Wextra -Wpedantic -march=native -m64
LDFLAGS =
DEBUG_FLAGS = -g -O0

BIN_DIR = bin

DEBUG_DIR = debug
DEBUG_TARGET = $(BIN_DIR)/$(DEBUG_DIR)/todo-db.exe

SRCS = main.c

debug: $(DEBUG_TARGET)
$(DEBUG_TARGET): $(SRCS)
	if not exist $(BIN_DIR)\$(DEBUG_DIR) mkdir $(BIN_DIR)\$(DEBUG_DIR)
	$(CC) $(FLAGS) $(LDFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_TARGET) $^

clean:
	rmdir /Q /S $(BIN_DIR)
