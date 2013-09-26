CC=clang++
CFLAGS=-c -Wall -g -O3
LDFLAGS=-lev
SOURCES=net.cpp ktmtbot.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLES=ktmtbot

all: $(SOURCES) $(EXECUTABLES)

$(EXECUTABLES): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o $(OBJECTS) $(EXECUTABLES) 
