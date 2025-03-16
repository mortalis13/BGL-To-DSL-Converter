BIN := bgl_to_dsl

CPPFLAGS:=-std=c++11
LDFLAGS:=-std=c++11 -lz -liconv

HEADERS := $(wildcard *.h)

SRC := $(wildcard *.cpp)
OBJ := $(notdir $(SRC:.cpp=.o))

ifeq ($(OS),Windows_NT)
	RM := del /Q /S
	BIN := $(BIN).exe
else
	RM := rm -f
endif


all: $(BIN)

$(BIN): $(OBJ)
	g++ -g $^ -o $@ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	g++ -o $@ -g $(CPPFLAGS) -c $<

clean:
	$(RM) *.o $(BIN)
