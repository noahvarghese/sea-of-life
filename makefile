SHELL := /bin/bash
CC := gcc
LINKS := -lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl -lm
CFLAGS := -Werror -Wall
OUTPUT := ./main

CGLM_VERSION := 0.8.4

build: 
	$(CC) $(CFLAGS) -o $(OUTPUT) src/*.c src/**/*.c -I ./include $(LINKS)

build-debug:
	$(CC) -g $(CFLAGS) -o $(OUTPUT) src/*.c src/**/*.c  -I ./include $(LINKS)