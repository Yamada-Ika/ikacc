CFLAGS=-std=c11 -g -static

SRCS	:= $(wildcard *.c)
OBJS	:= $(SRCS:%.c=%.o)
OBJS	:= $(addprefix obj/, $(OBJS))

ikacc: $(OBJS)

obj/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

test: ikacc
	./test.sh

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f ikacc

PWD := $(shell pwd)
up:
	docker run --rm -it -v $(PWD):/work_space -w /work_space compile_book bash

.PHONY: test clean fclean
