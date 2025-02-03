# Makefile for the smash program
# Variables
CC = gcc
CFLAGS = -std=c99 -g -Wall -pedantic-errors -DNDEBUG   
CCLINK = $(CC)
RM = rm -f

# Object files and the final executable
OBJS = main.o customAllocator.o
EXEC = main

# Functions and Dependencies
$(EXEC): $(OBJS)
	$(CCLINK) $(CFLAGS)$(OBJS) -o $(EXEC)

main.o: main.c customAllocator.o
	$(CC) $(CFLAGS) -c -o main.o main.c

customAllocator.o: customAllocator.c customAllocator.h
	$(CC) $(CFLAGS) -c -o customAllocator.o customAllocator.c

# Clean up old files
clean:
	$(RM) $(EXEC) $(OBJS) *~ core.* 
