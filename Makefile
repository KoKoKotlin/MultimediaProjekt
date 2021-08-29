SHELL = /bin/sh
CC = gcc
LIBS = -lm -lglfw -ldl
FLAGS = -Wall -Wextra -Wunused -Iinclude/

OBJECTS = main.o obj.o bitmap.o glad.o render.o engine.o
TARGET = tetris.out

all: FLAGS += -O3
all: $(TARGET)

debug: FLAGS += -g -ggdb
debug: $(TARGET)

debugOutput: FLAGS += -DDEBUG
debugOutput: debug

log: debug
log:
	./$(TARGET) > log

valgrind:
	valgrind ./$(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)

main.o : include/log.h include/obj.h
obj.o : include/obj.h
bitmap.o : include/bitmap.h
render.o: include/render.h
engine.o : include/engine.h

%.o : %.c
	$(CC) -c $(FLAGS) $<

.PHONY : clean
clean :
	rm -rf $(TARGET) $(OBJECTS) log