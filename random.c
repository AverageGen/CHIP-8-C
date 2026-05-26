#include <stdlib.h>
#include <time.h>
#include <stdio.h>




int main() {

    srand(time(NULL));

    int random_number = rand();

    printf("%d\n", random_number);
}