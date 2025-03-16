PR := bgl_to_dsl.exe

CPPFLAGS:=-std=c++11
LDFLAGS:=-std=c++11 -lz -liconv

HEADERS := $(wildcard *.h)

SRC := $(wildcard *.cpp)
OBJ := $(notdir $(SRC:.cpp=.o))

# ------------------------

all: $(PR)

$(PR): $(OBJ)
	g++ -g $^ -o $@ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	g++ -o $@ -g $(CPPFLAGS) -c $<

clean:
	del /Q /S *.o *.exe
