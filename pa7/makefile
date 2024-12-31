FLAGS := -Wall -Werror -std=gnu99
APP := mem_alloc

all: test

main: main.c  
	gcc $(FLAGS) $^ -o main

test: test_main.c $(APP).c
	gcc $(FLAGS) $^ -o test

clean:
	rm -f main test_main $(APP) test
	
