#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int backup_file_only(const struct dirent *info)
{
    char *ext;

    ext = strrchr(info->d_name '.');
    if(ext == NULL)
        return 0;
