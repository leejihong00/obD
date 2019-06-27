#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#define BUF_SIZE 4096

int main(int argc, char** argv)
{
    int fd1, fd2, len;
    char buf[BUF_SIZE];
    
    if(argc != 3)
        return -1;

    fd1 = open(argv[1], O_RDONLY);
    fd2 = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC);

    while((len=read(fd1, buf, sizeof(buf))) >0)
        write(fd2, buf, len);

    close(fd1);
    close(fd2);

    return 0;
}
