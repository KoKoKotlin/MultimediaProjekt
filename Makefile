SHELL = /bin/sh
CC = gcc
LIBS = -lm -lglfw -ldl
FLAGS = -Wall -Wextra -Wunused -Iinclude/

ifeq "$(shell sdl2-config --version > /dev/null && echo 1 || echo 0 )" "1"
  LIBS += -lSDL2
  FLAGS += -DSOUND
endif

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


$(BUILD_DIR)/main.o : include/log.h include/obj.h
$(BUILD_DIR)/obj.o : include/obj.h
$(BUILD_DIR)/bitmap.o : include/bitmap.h
$(BUILD_DIR)/render.o: include/render.h
$(BUILD_DIR)/engine.o : include/engine.h
$(BUILD_DIR)/helper.o : include/helper.h
$(BUILD_DIR)/audio.o : include/audio.h

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -o $@ -c $(FLAGS) $<

.PHONY : clean
clean :
	rm -rf build $(TARGET)