CXXFLAGS=-Wall -Wpedantic -std=c++20 -I../utils -L../lib
LIBS=-lutils
EXECS=read_file
CC=g++

all: $(EXECS)

$(EXECS): %: %.cpp
	$(CC) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf $(EXECS)
