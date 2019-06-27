#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pwd.h>	// getpwuid()
#include <grp.h>	// getgrgid()
#include <sys/ioctl.h>

#define PERM_LENGTH 	11
#define PATH_LENGTH 	100
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

//#define DEBUG

//=========================================
// dirent 구조체
//=========================================
/*
struct dirent
{
	long d_ino; //inode 번호
	off_t d_off; // offset
	unsigned short d_reclen; //d_name 길이
	char d_name[NAME_MAX+1]; //파일 이름
};
*/

//=========================================
// stat 구조체
//=========================================
/*
struct stat {
    dev_t     st_dev;     // ID of device containing file 
    ino_t     st_ino;     // inode number 
    mode_t    st_mode;    // 파일의 종류 및 접근권한
    nlink_t   st_nlink;   // hardlink 된 횟수
    uid_t     st_uid;     // 파일의 owner
    gid_t     st_gid;     // group ID of owner
    dev_t     st_rdev;    // device ID (if special file)
    off_t     st_size;    // 파일의 크기(bytes)
    blksize_t st_blksize; // blocksize for file system I/O
    blkcnt_t  st_blocks;  // number of 512B blocks allocated
    time_t    st_atime;   // time of last access
    time_t    st_mtime;   // time of last modification
    time_t    st_ctime;   // time of last status change
};
*/

//=========================================
// group 구조체
//=========================================
/*
struct group {
    char   *gr_name;       // group name 
    char   *gr_passwd;     // group password 
    gid_t   gr_gid;        // group ID 
    char  **gr_mem;        // group members 
};
*/

//=========================================
// group 구조체
//=========================================
/*
struct passwd {
	char *pw_name;
	char *pw_passwd;
	uid_t pw_uid;
	gid_t pw_gid;
	time_t pw_change;
	char *pw_class;
	char *pw_gecos;
	char *pw_dir;
	char *pw_shell;
	time_t pw_expire;
}; 
*/

//=========================================
// Function Name : access Name
// arg1 : permission value
// arg2 : mode 
// return : void
// Description : 
//=========================================
void access_perm(char *perm, mode_t mode)
{
	int i;
	char permchar[] = "rwx";
	memset(perm, '-', 10);
	perm[10] = '\0';

	if (S_ISDIR(mode)) perm[0] = 'd';
	else if (S_ISCHR(mode)) perm[0] = 'c';
	else if (S_ISBLK(mode)) perm[0] = 'b';
	else if (S_ISFIFO(mode)) perm[0] = 'p';
	else if (S_ISLNK(mode)) perm[0] = 'l';

	for (i = 0; i < 9; i++) 
	{
		if ((mode << i) & 0x100)
			perm[i+1] = permchar[i%3];
	}

	if (mode & S_ISUID) perm[3] = 's';
	if (mode & S_ISGID) perm[6] = 's';
	if (mode & S_ISVTX) perm[9] = 't';
}

//void terminal_size(int* rows, int* columns);
void terminal_width(int* colums);

int main(int argc, char *argv[])
{
	DIR *dp;		// DIR pointer
	struct stat statbuf;	// inode info
	struct dirent *dent;
	struct group *group_entry;
	struct passwd *user_pw;

	char perm[PERM_LENGTH];
	char pathname[PATH_LENGTH];
	char dirname[PATH_LENGTH];

	int flag;
	char temp[20];

	//time
	struct tm *tm;


	if (argc==1)	
		flag = 0;
		strcpy(dirname,".");

	if(argc>1)
	{
		// "-al" 문자열을 비교
		sprintf(temp,"-al");
        	if(!strcmp(temp,argv[1]))
		{
			flag = 1;
			strcpy(dirname,".");
		}
		else
		{
			flag = 2;
			strcpy(dirname, argv[1]);
		}
	}

	if(argc>2)
	{
		flag = 3;
		strcpy(dirname, argv[2]);
	}
		
#ifdef DEBUG	
	printf("dir=%s\n",dirname);
	printf("flag=%d\n",flag);
#endif

	// 디렉토리의 inode정보 읽기
	stat(dirname, &statbuf);

	// 디렉토리가 아닌경우 에러 메시지 출력 후 종료
	if (!S_ISDIR(statbuf.st_mode)) 
	{
		fprintf(stderr, "%s is not directory\n",dirname);
		exit(1);
	}

	// 디렉토리가 정상적으로 열리지 않은 경우
	if ((dp = opendir(dirname)) == NULL) 
	{
		perror("Error:");
		exit(1);
	}
	
#ifdef DEBUG
	// 정상적으로 열리면 디렉토리명을 출력
	printf("Lists of Directory(%s):\n", dirname);
#endif

    int n, m;
    terminal_width(&n);
    n %= 16;
    m = n;

	// 디렉토리의 내용을 읽어온다. 
	// 더이상 읽을 디렉토리 내용이 없을 때 까지
	while((dent = readdir(dp)) != NULL)
	{
		if(flag==1 | flag==3)
		{
			sprintf(pathname, "%s/%s", dirname, dent->d_name);
			lstat(pathname, &statbuf);
			access_perm(perm, statbuf.st_mode);
			user_pw=getpwuid(statbuf.st_uid);
			group_entry=getgrgid(statbuf.st_gid);
			tm = localtime(&statbuf.st_mtime); 
			strftime(temp, sizeof(temp), "%m월 %e %H:%M", tm);
			printf("%s %3ld %6s %6s %8ld %s %s\n", \
				perm, statbuf.st_nlink, user_pw->pw_name, \
				group_entry->gr_name, statbuf.st_size, temp, dent->d_name);
		}
		else
		{
            lstat(dent->d_name, &statbuf);
            if(statbuf.st_mode&S_IXUSR)
            {
                printf(ANSI_COLOR_BLUE "%-15s", dent->d_name);
                printf(ANSI_COLOR_RESET " ");
            }
            else
			    printf("%-15s ", dent->d_name);
            m--;
            if(m==0)
            {
                printf("\n");
                m = n;
            }
		}
	}
    if(n!=m) printf("\n");
	closedir(dp);
    //printf(ANSI_COLOR_BLUE "\naaaaaaaaaaa" ANSI_COLOR_RESET "\n");
	return 0;
}

//void terminal_size(int* rows, int* columns)
void terminal_width(int* columns)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    *columns = w.ws_col;
}
