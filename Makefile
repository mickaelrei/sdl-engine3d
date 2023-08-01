CC := g++

SOURCE_DIR := src

SRC := $(SOURCE_DIR)/main.cpp \
	   $(SOURCE_DIR)/engine_draw.cpp \
	   $(SOURCE_DIR)/engine_math.cpp \
	   $(SOURCE_DIR)/engine_main.cpp

# Flags
FLAGS := -lSDL2main \
		 -lSDL2

INCLUDES := ./include/

WFLAGS := -lmingw32

# Binary related
DEST := bin
OBJ_NAME := main

wbuild:
	powershell if (!(Test-Path -Path $(DEST))) {[void](New-Item -Name $(DEST) -Path . -ItemType directory)}
	$(CC) -I C:/SDL2/include -I $(INCLUDES) -L C:/SDL2/lib $(SRC) -o $(DEST)/$(OBJ_NAME) $(WFLAGS) $(FLAGS)

build:
	if [ ! -d $(DEST) ]; then mkdir $(DEST); fi
	$(CC) -I $(INCLUDES) $(SRC) -o $(DEST)/$(OBJ_NAME) $(FLAGS)

run:
	./$(DEST)/$(OBJ_NAME)