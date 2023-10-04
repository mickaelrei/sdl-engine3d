CXX := g++

# Source folder
SRC := ./src

# Tests folder
TESTS := ./tests

# Include folder
INCLUDE := ./include

# Flags
FLAGS := -lSDL2main \
		 -lSDL2

# Binary folder
DST := ./bin

all: dir tests

tests: $(DST)/clock

$(DST)/clock: $(TESTS)/clock.cpp $(DST)/engine.o
	$(CXX) -I $(INCLUDE) $(TESTS)/clock.cpp  $(DST)/camera.o  \
		   $(DST)/engine.o  \
		   $(DST)/mat4.o    \
		   $(DST)/mesh.o    \
		   $(DST)/texture.o \
		   $(DST)/texuv.o   \
		   $(DST)/vec2.o    \
		   $(DST)/vec3.o    \
		   -o $(DST)/clock $(FLAGS)

dir: $(DST)
	if [ ! -d $(DST) ]; then mkdir $(DST); fi

clean:
	if [ -d $(DST) ]; then rm -f $(DST)/*.o; fi

$(DST)/engine.o: $(SRC)/engine.cpp $(INCLUDE)/engine.hpp $(DST)/vec2.o $(DST)/camera.o $(DST)/mesh.o
	$(CXX) -I $(INCLUDE) -c $(SRC)/engine.cpp $(FLAGS) -o $(DST)/engine.o

$(DST)/camera.o: $(SRC)/camera.cpp $(INCLUDE)/camera.hpp $(DST)/vec3.o $(DST)/mat4.o
	$(CXX) -I $(INCLUDE) -c $(SRC)/camera.cpp -o $(DST)/camera.o

$(DST)/mat4.o: $(SRC)/mat4.cpp $(INCLUDE)/mat4.hpp $(DST)/vec3.o
	$(CXX) -I $(INCLUDE) -c $(SRC)/mat4.cpp -o $(DST)/mat4.o

$(DST)/mesh.o: $(SRC)/mesh.cpp $(INCLUDE)/mesh.hpp $(DST)/vec3.o $(DST)/texture.o
	$(CXX) -I $(INCLUDE) -c $(SRC)/mesh.cpp $(FLAGS) -o $(DST)/mesh.o

$(DST)/texture.o: $(SRC)/texture.cpp $(INCLUDE)/texture.hpp $(DST)/texuv.o
	$(CXX) -I $(INCLUDE) -c $(SRC)/texture.cpp $(FLAGS) -o $(DST)/texture.o

$(DST)/texuv.o: $(SRC)/texuv.cpp $(INCLUDE)/texuv.hpp
	$(CXX) -I $(INCLUDE) -c $(SRC)/texuv.cpp -o $(DST)/texuv.o

$(DST)/vec2.o: $(SRC)/vec2.cpp $(INCLUDE)/vec2.hpp
	$(CXX) -I $(INCLUDE) -c $(SRC)/vec2.cpp -o $(DST)/vec2.o

$(DST)/vec3.o: $(SRC)/vec3.cpp $(INCLUDE)/vec3.hpp
	$(CXX) -I $(INCLUDE) -c $(SRC)/vec3.cpp -o $(DST)/vec3.o
