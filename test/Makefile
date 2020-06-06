.PHONY : all clean

SRCS = $(wildcard *.c)
TESTS = $(patsubst %.c,%,$(SRCS))

all : $(TESTS)
	@echo Tests file compiled

% :  %.c test.h
	g++ -Wall $^ -std=c++11 -lpthread -o $@

clean :
	rm -rf *.o *.so $(TESTS)
