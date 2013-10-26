/* kmp.c
 * Implementation of Knuth-Morris-Pratt algorithm
 * Reference:
 *   http://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
 */

#include <stdio.h>
#include <string.h>

/* Maxlength for keyword */
#define STRING_MAX_LENGTH 256

/* failure function */
static int f[256];

/*
 * Build failure table.
 * Return: length of failure table
 */
int build_ff_table(const char* s)
{
    int len;
    int t = -1, i;
    char *p;

    len = strlen(s);
    memset(f, 0, len);
    f[0] = -1;

    for (i = 0; i < strlen(s) - 1; ++i) {
        while (t >= 0 && s[i+1] != s[t+1]) t = f[t];
        if (s[i+1] == s[t+1]) {
            t++;
            f[i+1] = t;
        } else {
            f[i+1] = -1;
        }
    }
    return len;
}


const char* kmp_search(const char *k, int len, const char *s)
{
    const char *p;
    int t = -1;

    for(p = s; *p; p++) {
        while (t >= 0 && *p != k[t+1]) t = f[t];
        if (*p == k[t+1])
            t++;
        if (t == len - 1) return p;
    }
    return NULL;
}

int main(int argc, char **argv)
{
    const char *p;
    int len;

    if (argc < 2) {
        fprintf(stdout, "usage: %s keyword search\n", argv[0]);
        return -1;
    }

    p = argv[2];
    len = build_ff_table(argv[1]);
    while ((p = kmp_search(argv[1], len, p)) != NULL) {
        printf("%s\n", p);
    }

    return 0;
}
