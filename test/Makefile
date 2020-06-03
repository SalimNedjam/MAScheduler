.PHONY : all clean
BIN =inherit_load_$(f)

all : $(BIN)

$(BIN) :  $(BIN).o bench.h
	g++ -Wall $^ -std=c++11 -lpthread -o $@

clean :
	rm -rf *.o *.so $(BIN)
