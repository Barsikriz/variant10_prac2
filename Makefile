CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)

.PHONY: all test

all: depgraph.exe

depgraph.exe: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o depgraph.exe

test: depgraph.exe
	powershell -ExecutionPolicy Bypass -File tests/stage1.ps1
