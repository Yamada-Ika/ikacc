#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "error: invalid argument\n");
		return (1);
	}

	char	*p = argv[1];
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("\n");
	printf("main:\n");
	printf("\tmov rax, %lld\n", strtoll(p, &p, 10));
	while (p != NULL)
	{
		if (*p == '\0')
			break ;
		if (*p == '+')
		{
			p++;
			printf("\tadd rax, %lld\n", strtoll(p, &p, 10));
			continue ;
		}
		if (*p == '-')
		{
			p++;
			printf("\tsub rax, %lld\n", strtoll(p, &p, 10));
			continue ;
		}
		fprintf(stderr, "error: unexpected charcter '%c'\n", *p);
		return 1;
	}
	printf("\tret\n");
	return (0);
}
