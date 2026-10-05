#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MY_STACK_PUSH   335
#define MY_STACK_POP    336
#define MY_STACK_CLEAR  337

int randint(int a, int b) {
    return rand() % (b - a + 1) + a;
}

int main(void) {
    srand(time(NULL));

    // Initialize stack
    printf("Reset stack before test.\n");
    syscall(MY_STACK_CLEAR);

    // Push five times
    printf("Push values:\t");
    for (int i = 0; i < 5; i++) {
        int push_val = randint(1, 100);
        syscall(MY_STACK_PUSH, push_val);
        printf("\t%i", push_val);
    }
    putchar('\n');

    // Pop three times
    printf("Pop values:\t");
    for (int i = 0; i < 3; i++) {
        printf("\t%ld", syscall(MY_STACK_POP));
    }
    putchar('\n');

    // Clear
    printf("Clear remaining nodes:\t%ld\n", syscall(MY_STACK_CLEAR));

    printf("Check kernel log with: dmesg | tail -30\n");
    return 0;
}
