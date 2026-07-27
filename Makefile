CXX ?= g++
CXXFLAGS ?= -std=c++20 -fmodules-ts -Wall -Wextra -pedantic
TARGET := constructor
VECTOR_STAMP := .vector-header-unit

INTERFACE_OBJECTS := \
	types.o tile.o residence.o vertex.o edge.o dice.o board.o builder.o \
	boardsetupstrategy.o gamestateio.o textdisplay.o gamecontroller.o

IMPLEMENTATION_OBJECTS := \
	tile-impl.o residence-impl.o vertex-impl.o edge-impl.o dice-impl.o \
	board-impl.o builder-impl.o boardsetupstrategy-impl.o \
	gamestateio-impl.o textdisplay-impl.o gamecontroller-impl.o

OBJECTS := $(INTERFACE_OBJECTS) $(IMPLEMENTATION_OBJECTS) main.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

$(VECTOR_STAMP):
	$(CXX) $(CXXFLAGS) -x c++-system-header vector
	@echo built > $@

%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c $<

types.o: src/types.cc
tile.o: types.o
residence.o: types.o
vertex.o: residence.o
edge.o: types.o
dice.o: src/dice.cc
board.o: $(VECTOR_STAMP) types.o tile.o vertex.o edge.o residence.o
builder.o: types.o dice.o
boardsetupstrategy.o: board.o
gamestateio.o: board.o builder.o
textdisplay.o: board.o
gamecontroller.o: board.o builder.o dice.o types.o gamestateio.o boardsetupstrategy.o

tile-impl.o: tile.o
residence-impl.o: residence.o
vertex-impl.o: vertex.o
edge-impl.o: edge.o
dice-impl.o: dice.o
board-impl.o: board.o residence.o
builder-impl.o: builder.o
boardsetupstrategy-impl.o: boardsetupstrategy.o types.o
gamestateio-impl.o: gamestateio.o types.o tile.o vertex.o edge.o residence.o
textdisplay-impl.o: textdisplay.o edge.o residence.o tile.o types.o vertex.o
gamecontroller-impl.o: gamecontroller.o residence.o textdisplay.o
main.o: boardsetupstrategy.o gamecontroller.o

test:
	bash src/tests/run-tests.sh

ifeq ($(OS),Windows_NT)
clean:
	-cmd /C "del /Q $(OBJECTS) $(TARGET) $(TARGET).exe $(VECTOR_STAMP) 2>NUL"
	-cmd /C "if exist gcm.cache rmdir /S /Q gcm.cache"
else
clean:
	rm -rf $(OBJECTS) $(TARGET) $(TARGET).exe gcm.cache $(VECTOR_STAMP)
endif
