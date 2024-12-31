FLAGS := -Wall -Werror -std=c99

all: encode decode

encode: encode.c utils.c utils.h
	gcc $(FLAGS) $^ -o $@

decode: decode.c utils.c utils.h
	gcc $(FLAGS) $^ -o $@

clean:
	rm -f encode
	rm -f decode
