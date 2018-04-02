# A simple Makefile for compiling small SDL projects

# set the compiler
CC := gcc

# set the compiler flags
CFLAGS := `sdl2-config --libs --cflags` -include nes.h -ggdb3 -O0 --std=c99 -Wno-char-subscripts  -lm

# add header files here
HDRS :=
TEST_HDRS := nes.h
# add source files here
SRCS := sdl_test.c nes.c main.c
TEST_SRCS := sdl_test.c nes.c tests.c

# generate names of object files
OBJS := $(SRCS:.c=.o)
TEST_OBJS := $(TEST_SRCS:.c=.o)

# name of executable
EXEC := game

# default recipe
all: $(EXEC)

# recipe for building the final executable
$(EXEC): $(OBJS) $(TEST_HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

test: $(TEST_OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(TEST_OBJS) $(CFLAGS) 

# recipe for building object files
#$(OBJS): $(@:.o=.c) $(HDRS) Makefile
#	$(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
