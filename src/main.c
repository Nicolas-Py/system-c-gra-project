#include <stdio.h>
#include <stdlib.h>

extern int run_simulation();

int main(int argc, char* argv[]) {
    printf("Simulation result: %d", run_simulation());
    return 0;
}