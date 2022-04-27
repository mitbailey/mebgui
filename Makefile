CXX = g++
CC = gcc
CPPOBJS = src/mebgui.o examples/guimain.o
COBJS =
EDCXXFLAGS = -I ./ -I ./include/ -I ./examples/ -std=c++11 -Wall -pthread $(CXXFLAGS)
EDCFLAGS = $(CFLAGS)
EDLDFLAGS := -lpthread -lm -lmenu -lncurses $(LDFLAGS)
TARGET = ui.out

all: $(COBJS) $(CPPOBJS)
	$(CXX) $(EDCXXFLAGS) $(COBJS) $(CPPOBJS) -o $(TARGET) $(EDLDFLAGS)
	./$(TARGET)

%.o: %.cpp
	$(CXX) $(EDCXXFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(EDCFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	$(RM) *.out
	$(RM) *.o
	$(RM) src/*.o
	$(RM) examples/*.o

.PHONY: spotless

spotless:
	$(RM) *.out
	$(RM) *.o
	$(RM) src/*.o
	$(RM) examples/*.o
	$(RM) drivers/*.o