#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char** argv)
{
    struct dirent **namelist;
    int count;
    int idx;

    if(argc != 2) return -1;
    if((count = scandir(argv[1], &namelist, NULL, alphasort)) == -1)
    {
        perror("scandir()");
        return 1;
    }

    for(idx = 0; idx < count; idx++)
        printf("%s\n", namelist[idx]->d_name);

    for(idx = 0; idx < count; idx++)
        free(namelist[idx]);

    free(namelist);

    return 0;
}
