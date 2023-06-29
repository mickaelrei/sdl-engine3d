CC := g++

SOURCE_DIR := src

SRC := $(SOURCE_DIR)/main.cpp \
	   $(SOURCE_DIR)/engine.cpp

# Flags
FLAGS := -lSDL2main \
		 -lSDL2

INCLUDES := ./include/

WFLAGS := -lmingw32

# Binary related
DEST := bin
OBJ_NAME := main

wbuild:
	$(CC) -I C:/SDL2/include -I $(INCLUDES) -L C:/SDL2/lib $(SRC) -o $(DEST)/$(OBJ_NAME) $(WFLAGS) $(FLAGS)

build:
	$(CC) -I $(INCLUDES) $(SRC) -o $(DEST)/$(OBJ_NAME) $(FLAGS)

run:
	./$(DEST)/$(OBJ_NAME)