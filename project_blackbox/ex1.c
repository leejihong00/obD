#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>

int mkdirs(const char *path, mode_t mode);

int main(int argc, char** argv)
{
    char str_newDir[256] = {0};
    getcwd(str_newDir, 256);

    time_t rawtime;
    time(&rawtime);
    struct tm *tm_ptr;
    char strBuffer[128] = {0};
    tm_ptr = localtime(&rawtime);
    //sprintf(str_newDir, "/home/pi/user/blackbox/%d%02d%d_%02d", 
    sprintf(strBuffer, "/%d%02d%d_%02d", 
            tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, 
            tm_ptr->tm_mday, tm_ptr->tm_hour);
    strcat(str_newDir, strBuffer);

    mkdirs(str_newDir, 0777);
}

int mkdirs(const char *path, mode_t mode)
{
    char tmp_path[256];
    const char *tmp = path;
    int len = 0;
    int ret;

    fprintf(stdout, "making directory (%s)\n", path);
    if(path == NULL || strlen(path) >= 256)
    {
        perror("invalid path");
        return -1;
    }
    while((tmp = strchr(tmp, '/')) != NULL)
    {
        len = tmp - path;
        tmp++;
        if(len == 0)
            continue;
        strncpy(tmp_path, path, len);
        tmp_path[len] = 0x00;
        if((ret = mkdir(tmp_path, mode)) == -1)
            if(errno != EEXIST)
            {
                fprintf(stderr, "error: %s\n", strerror(errno));
                return -1;
            }
    }
    if((ret = mkdir(path, mode) == -1))
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return -1;
    }
    else return 0;
}
