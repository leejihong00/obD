#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h> // mkdir

int main()
{
    DIR * dir_info;
    struct dirent * dir_entry;

    // mkdir //
    mkdir("test_A", 0755);
    mkdir("test_B", 0755);

    // opendir //
    dir_info = opendir(".");

    if(dir_info != NULL)
        while(dir_entry = readdir(dir_info)) // readdir
            printf("%s\n", dir_entry->d_name);

    // closedir //
    closedir(dir_info);
}
