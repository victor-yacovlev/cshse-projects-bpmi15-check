#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>  
#include <limits.h>
#include <string.h>
#include <errno.h>

int
main(int argc, char* argv[], char* envp[]) 
{   
    int n = atoi(envp[0]+6), s = 0;
    printf("Please, enter %d numbers:\n", n);
    for(int i = 1; i <= n; ++i) {
        int a;
        scanf("%d ", &a);
        s += a;
    }
    printf("Sum: %d\n", s);
}

