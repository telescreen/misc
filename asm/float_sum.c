#include <stdio.h>

int main()
{
	float a[5];
	float sum = 0;
	int i;

	for (i = 0; i < 5; ++i)
		scanf("%f", &a[i]);

	for (i = 0; i < 5; ++i)
		sum += a[i];

	printf("Sum of 5 float: %f\n", sum);
	return 0;
}
