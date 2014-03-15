#include <stdio.h>

double calculate_pi()
{
    double pi = 1;
    double sign = 1;
    int i;
    for(i = 1; i <= 1000000; ++i) {
        sign *= -1;
        pi = pi + sign / (2*(i+1) - 1);
    }
    return 4*pi;
}

int main() {
    double pi;
    pi = calculate_pi();
    printf("Value of pi: %f\n", pi);
    return 0;
}
