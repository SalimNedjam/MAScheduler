.PHONY : all clean check

SRCS = $(wildcard *.c)
BENCH = $(patsubst %.c,%,$(SRCS))

all : $(BENCH)

% :  %.c 
	g++ -Wall $^ -std=c++11 -lpthread -o $@

clean :
	rm -rf *.o *.so $(BENCH)
