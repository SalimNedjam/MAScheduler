.PHONY : all clean check
.ONESHELL:
EXEC=force_inherit
BENCH=force_inherit_bench

all : share force_inherit_bench



$(BENCH):  $(BENCH).c
	g++ -Wall $^ -std=c++11 -lbenchmark -lpthread -o $@






check :
	for f in *.c *.h ; do
		$(CHECK_PATCH) -f $$f
	done

clean :
	rm -rf *.o *.so $(EXEC)
