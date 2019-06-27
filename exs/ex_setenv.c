#include <stdio.h>
#include <stdlib.h>

extern char ** environ;

int main()
{
    setenv("MYDIR", "~/wsp/exs", 0);
    setenv("KERNEL_SRC", "/usr/src/linux-headers-4.15.0-52-generic", 0);

    printf("MYDIR= %s\n", getenv("MYDIR"));
    printf("KERNEL_SRC= %s\n", getenv("KERNEL_SRC"));

    while(*environ)
        printf("%s\n", *environ++);
    
    return 0;
}
