FLAGS := -Wall -Werror -std=gnu99
APP := matrix_mult

all: $(APP)

$(APP): $(APP).c main.c
	gcc $(FLAGS) $^ -o $(APP) -lpthread

clean: 
	rm -f $(APP)
