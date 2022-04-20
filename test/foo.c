#include <stdio.h>
#include <stdlib.h>

void foo(void) {
	printf("foo called\n");
}

void bar(int n1, int n2, int n3, int n4, int n5, int n6) {
	printf("bar called %d %d %d %d %d %d\n", n1, n2, n3, n4, n5, n6);
}

void print_int(int n) {
	printf("%d\n", n);
}

void alloc4(int **p, int n1, int n2, int n3, int n4) {
	*p = malloc(4 * sizeof(int));
	*(*p)=n1;
	*(*p+1)=n2;
	*(*p+2)=n3;
	*(*p+3)=n4;
}
