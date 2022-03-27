CFLAGS=-std=c11 -g -static

9cc: 9cc.c

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

PWD := $(shell pwd)
up:
	docker run --rm -it -v $(PWD):/work_space -w /work_space compile_book bash

.PHONY: test clean
