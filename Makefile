CFLAGS=-std=c11

ifdef WITH_DEBUG
	CFLAGS += -g -O0
endif

SRCS	:= $(wildcard *.c)
OBJS	:= $(SRCS:%.c=%.o)
OBJS	:= $(addprefix obj/, $(OBJS))

ikacc: $(OBJS)
	$(CC) $(CFLAGS) -o ikacc $(OBJS)

obj/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

test:
	make
	./test.sh

debug: fclean
	make WITH_DEBUG=1
	make test

re: fclean
	make

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f ikacc

PWD := $(shell pwd)
up:
	docker run --rm -it -v $(PWD):/work_space -w /work_space compile_book bash

.PHONY: test clean fclean
