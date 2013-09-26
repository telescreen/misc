#include "SpookyV2.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int main(int argc, char **argv)
{
	uint64_t out1, out2;
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s message [seed]\n", argv[0]);
		return 1;
	}
	if (argv[2]) {
		out1 = out2 = strtoull(argv[2], NULL, 0);
	}
	SpookyHash::Hash128(argv[1], strlen(argv[1]), &out1, &out2);
	// first 64 bit value
	printf("%16llx", out1);
	// second 64 bit value
	printf("%16llx\n", out2);
	return 0;
}
