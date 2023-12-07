# LAST MODIFIED : March 15, 2021
# MAKE PROGRAM  : mingw32-make.exe
# PLATFORM      : Windows
# FUNCTON       : make all *.c file to corresponding *.exe

CC    := gcc
FLAGS := -Wall -g3 -mwindows
LIBS  := 
OUT   := $(patsubst %.c, %.exe, $(wildcard *.c))

all: $(OUT)

%.exe: %.o
	$(CC) $(FLAGS) $(LIBS) $< -o $@

%.o: %.c
	$(CC) -c $<

.PHONY: clean
clean:
	del *.exe *.o

