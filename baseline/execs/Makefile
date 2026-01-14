CXXFLAGS=-Wall -Wpedantic -std=c++20 -I../utils -L../lib
LIBS=-lutils
EXECS=infer extract count_unique_types
CC=g++

all: $(EXECS)

$(EXECS): %: %.cpp
	$(CC) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf $(EXECS) *.png *.txt *.xbnf
