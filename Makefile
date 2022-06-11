CXX       := g++
CXX_FLAGS := -std=c++2a -ggdb

BIN     := bin
SRC     := src
INCLUDE_PATHS := include
LIBRARY_PATHS = lib

LIBRARIES   := -lSDL2 #-lSDL2_image  #Don't forget that -l is the option
EXECUTABLE  := main

.PHONY: clean all

all: $(BIN)/$(EXECUTABLE)

run: clean all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
    $(CXX) $(CXX_FLAGS) $^ -o $@ -I$(INCLUDE_PATHS) -L$(LIBRARY_PATHS) $(LIBRARIES)

#   Basically this is what we are doing.
#   g++ src/main.cpp -o main -Iinclude -Llib -lSDL2-2.0.0

clean:
    -rm $(BIN)/*