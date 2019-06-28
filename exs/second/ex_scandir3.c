#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int backup_file_only(const struct dirent *info)
{
    char *ext;

    ext = strrchr(info->d_name '.');
    if(ext == NULL)
        return 0;
    if(strcmp(ext, ".bak") == 0)
        return 1;
    else
        return 0;
}
