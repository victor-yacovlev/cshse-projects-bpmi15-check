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
    printf("Thanks, there are numbers:\n");
    int n = atoi(envp[0]+6);
    for(int i = 1; i <= n; ++i)
        printf("%d ", i);
    printf("\n");
}

