COMPILE_FLAGS := -Wall -Wextra -Wpedantic -std=c11

clean:
	rm *.o
	rm build_*

build_dbg: COMPILE_FLAGS += -O0 -g3
build_dbg: $(patsubst %.c, %.o, $(wildcard *.c))	
	gcc $(COMPILE_FLAGS) $^ -o $@

build_rls: COMPILE_FLAGS += -Os -g0
build_rls: $(patsubst %.c, %.o, $(wildcard *.c))
	gcc $(COMPILE_FLAGS) $^ -o $@

%.o: %.c
	gcc $(COMPILE_FLAGS) -c $<
    
main.o: zip_headers.h
zip_headers.o: zip_headers.h
