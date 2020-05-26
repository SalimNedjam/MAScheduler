.PHONY : all clean

all : inherit_load

% :  %.c
	g++ -Wall $^ -std=c++11 -lpthread -o $@
	

clean :
	rm -rf *.o *.so inherit_load
