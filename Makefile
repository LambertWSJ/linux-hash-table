CC=gcc
FLAGS=-Iinclude -g -Wattributes
SRC=main.c
DIST = main

build:
	$(CC) $(SRC) $(FLAGS) -o $(DIST)
run: build
	./$(DIST)	