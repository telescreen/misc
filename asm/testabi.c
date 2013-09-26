#include <stdio.h>

int test(int *a)
{
	*a = 5;
}

int main()
{
	int a;
	test(&a);
	return 0;
}
