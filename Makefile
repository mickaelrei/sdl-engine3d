CXX := g++

SOURCE := ./src

# Flags
FLAGS := -lSDL2main \
		 -lSDL2

INCLUDE := ./include

WFLAGS := -lmingw32

# Binary related
DEST := ./bin
OBJ_NAME := main

all: $(SOURCE)/main.cpp $(DEST)/main $(DEST)/engine.o
	$(CXX) -I $(INCLUDE)     \
		   $(DEST)/camera.o  \
		   $(DEST)/engine.o  \
		   $(DEST)/mat4.o    \
		   $(DEST)/mesh.o    \
		   $(DEST)/texture.o \
		   $(DEST)/texuv.o   \
		   $(DEST)/vec2.o    \
		   $(DEST)/vec3.o    \
		   $(SOURCE)/main.cpp $(FLAGS) -o $(DEST)/main

dir:
	if [ ! -d $(DEST) ]; then mkdir $(DEST); fi

clean:
	rm -f ./*.o; if [ -d $(DEST) ]; then rm -f $(DEST)/*.o; fi

$(DEST)/camera.o: $(SOURCE)/camera.cpp $(INCLUDE)/camera.hpp $(DEST)/vec3.o $(DEST)/mat4.o
	$(CXX) -I $(INCLUDE) -c $(SOURCE)/camera.cpp -o $(DEST)/camera.o

$(DEST)/engine.o: $(SOURCE)/engine.cpp $(INCLUDE)/engine.hpp $(DEST)/vec2.o $(DEST)/camera.o $(DEST)/mesh.o
	$(CXX) -I $(INCLUDE) -c $(SOURCE)/engine.cpp $(FLAGS) -o $(DEST)/engine.o

$(DEST)/mat4.o: $(SOURCE)/mat4.cpp $(INCLUDE)/mat4.hpp $(DEST)/vec3.o
	$(CXX) -I $(INCLUDE) -c $(SOURCE)/mat4.cpp -o $(DEST)/mat4.o

$(DEST)/mesh.o: $(SOURCE)/mesh.cpp $(INCLUDE)/mesh.hpp $(DEST)/vec3.o $(DEST)/texture.o
	$(CXX) -I $(INCLUDE) -c $(SOURCE)/mesh.cpp $(FLAGS) -o $(DEST)/mesh.o

$(DEST)/texture.o: $(SOURCE)/texture.cpp $(INCLUDE)/texture.hpp $(DEST)/texuv.o
	$(CXX) -I $(INCLUDE) -c $(SOURCE)/texture.cpp $(FLAGS) -o $(DEST)/texture.o

$(DEST)/texuv.o: $(SOURCE)/texuv.cpp $(INCLUDE)/texuv.hpp
	$(CXX) -I $(INCLUDE) -c $(SOURCE)/texuv.cpp -o $(DEST)/texuv.o

$(DEST)/vec2.o: $(SOURCE)/vec2.cpp $(INCLUDE)/vec2.hpp
	$(CXX) -I $(INCLUDE) -c $(SOURCE)/vec2.cpp -o $(DEST)/vec2.o

$(DEST)/vec3.o: $(SOURCE)/vec3.cpp $(INCLUDE)/vec3.hpp
	$(CXX) -I $(INCLUDE) -c $(SOURCE)/vec3.cpp -o $(DEST)/vec3.o