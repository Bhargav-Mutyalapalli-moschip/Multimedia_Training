#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int i;
    for (i = 0; i < 100; i++) {
        printf("\033[KThe output: %d\r", i);
        fflush(stdout); // Flush the output buffer to ensure immediate display
        sleep(1);
    }
    printf("\n"); // Print a newline to move to the next line after the loop
    return 0;
}

