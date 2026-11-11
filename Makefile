CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

.PHONY: all test

all: depgraph.exe

depgraph.exe: src/main.c
	$(CC) $(CFLAGS) src/main.c -o depgraph.exe

test: depgraph.exe
	powershell -ExecutionPolicy Bypass -File tests/stage1.ps1
