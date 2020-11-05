# Define the source and object files for the executable
SRC     = $(wildcard *.cpp)
OBJ 		= $(SRC:.cpp=.o)
EXE 		= $(SRC:.cpp=)
#
# set up extra flags for explicitly setting mode
debug:      CFLAGS    += -g -O0
release:    CFLAGS    += -O0

#
# Link all the objects into an executable.
all:    $(OBJ)
	$(CXX) -o processor $(CFLAGS) $(LDFLAGS) $(OBJ) $(LOADLIBES) $(LDLIBS)

# $(EXE)
#
# Though both modes just do a normal build.
debug:      all
release:    all

clean:
	rm $(OBJ)
