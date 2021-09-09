SHELL = /bin/sh
CC = gcc
LIBS = -lm -lglfw -ldl -lSDL2
FLAGS = -Wall -Wextra -Wunused -Iinclude/

SRC_DIR = src
SOURCE_FILES = $(wildcard $(SRC_DIR)/*.c)

BUILD_DIR = build
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCE_FILES))
TARGET = tetris.out

target: $(BUILD_DIR) | $(TARGET)

all: FLAGS += -O3
all: target

debug: FLAGS += -g -ggdb
debug: target

debugOutput: FLAGS += -DDEBUG
debugOutput: debug

log: debug
log:
	./$(TARGET) > log

valgrind:
	valgrind ./$(TARGET)

$(TARGET) : $(OBJ_FILES)
	$(CC) -o $(TARGET) $(OBJ_FILES) $(LIBS)

$(BUILD_DIR) :
	mkdir -p $@

main.o : include/log.h include/obj.h
obj.o : include/obj.h
bitmap.o : include/bitmap.h
render.o: include/render.h
engine.o : include/engine.h
helper.o : include/helper.h
audio.o : include/audio.h

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -o $@ -c $(FLAGS) $<

.PHONY : clean
clean :
	rm -rf build $(TARGET)