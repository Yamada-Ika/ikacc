CFLAGS=-std=c11 -g -static

SRCS	:= $(wildcard *.c)
OBJS	:= $(SRCS:%.c=%.o)

ikacc: $(OBJS)

test: ikacc
	./test.sh

clean:
	rm -f ikacc *.o *~ tmp*

PWD := $(shell pwd)
up:
	docker run --rm -it -v $(PWD):/work_space -w /work_space compile_book bash

.PHONY: test clean
