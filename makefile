COMPILE_FLAGS := -Wall -Wextra -Wpedantic -std=c11

clean:
	rm *.o
	rm -rf bin

debug: COMPILE_FLAGS += -O0 -g3
debug: $(patsubst %.c, %.o, $(wildcard *.c))
	mkdir -p bin
	gcc $(COMPILE_FLAGS) $^ -o bin/$@

release: COMPILE_FLAGS += -Os -g0
release: $(patsubst %.c, %.o, $(wildcard *.c))
	mkdir -p bin
	gcc $(COMPILE_FLAGS) $^ -o bin/$@

%.o: %.c	
	gcc $(COMPILE_FLAGS) -c $<
    
main.o: zip_headers.h
zip_headers.o: zip_headers.h
