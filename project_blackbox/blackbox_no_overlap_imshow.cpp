#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace cv;
using namespace std;

char * mkdirs(void);
void * cleaning(void * check_duration);
void * recordCam(void * record_sec);
double storage_available();
int rmdirs(const char *path, int is_error_stop);

const char *path = ".";

int main()
{
    int rsec = 60; // recording length in second
    int space = 20; // minimum space required
    pthread_t rec_thr; // for recording
    pthread_t space_thr; // for cleaning
    int i;
    /*
    //getting permission for camera access (OSX Mojave)
    for(i=0; i<3; i++)
    {
    VideoCapture cap_temp(0);
    if(cap_temp.isOpened())
    break;
    sleep(1);
    }
     */
    if(pthread_create(&space_thr, NULL, cleaning, (void*)&space) < 0)
    {
        perror("pthread_cread() 1");
        return -1;
    }
    if(pthread_create(&rec_thr, NULL, recordCam, (void*)&rsec) < 0)
    {
        perror("pthread_create() 2");
        return -1;
    }
    pthread_join(rec_thr, NULL); 
    pthread_join(space_thr, NULL);

}
void * recordCam(void * sec)
{
    char str_newDir[256] = {0};
    time_t rawtime;
    struct tm *tm_ptr;
    char * strBuffer;
    char filename[32] = {0};
    int rsec = *(int*)sec;
    int i;
    int j = 0;
    Mat img_color;

    VideoCapture cap("nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)640, height=(int)360,format=(string)NV12, framerate=(fraction)24/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink");
/**************************************
    VideoCapture cap("nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)1280, height=(int)720,format=(string)NV12, framerate=(fraction)24/1 ! nvvidconv flip-method=2 ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink");
**************************************/
    if (!cap.isOpened()) {
        perror("class VideoCapture");
        exit(-1);
    }
    Size size = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
            (int)cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = (double)cap.get(CAP_PROP_FPS);
    /****************** recording *******************/ 
    while(++j) { 
        time(&rawtime);
        tm_ptr = localtime(&rawtime);
        strBuffer = mkdirs();
        strcpy(str_newDir, strBuffer);
        free(strBuffer);
        sprintf(filename, "/%d%02d%02d_%02d%02d%02d.avi",
                tm_ptr->tm_year+1900, tm_ptr->tm_mon+1,
                tm_ptr->tm_mday, tm_ptr->tm_hour,
                tm_ptr->tm_min, tm_ptr->tm_sec);
        strcat(str_newDir, filename);
        printf("%s\n", str_newDir);

        VideoWriter writer;
        writer.open(str_newDir, VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);
        if (!writer.isOpened())
        {
            perror("VideoWriter open()");
            exit(-1);
        }
        printf("no. %d recording \n", j);
        for(i=0; i<(int)(fps*rsec); i++)
        {
            cap.read(img_color); // reading a frame
            if (img_color.empty()) {
                perror("VideoCapture read() empty");
                break;
            }

            flip(img_color, img_color, -1);
            writer.write(img_color); // writing a frame
            imshow(str_newDir, img_color);
            if (waitKey(1) >= 0)
                break;
        }
        destroyAllWindows();
    } 
    /*************************************************/
}
char* mkdirs(void)
{
    char str_newDir[256] = {0};
    mode_t mode = 0777;
    getcwd(str_newDir, 256);
    time_t rawtime;
    time(&rawtime);
    struct tm *tm_ptr;
    char strBuffer[128] = {0};
    tm_ptr = localtime(&rawtime);
    sprintf(strBuffer, "/%d%02d%02d_%02d",
            tm_ptr->tm_year+1900, tm_ptr->tm_mon+1,
            tm_ptr->tm_mday, tm_ptr->tm_hour);
    strcat(str_newDir, strBuffer);

    char tmp_path[256];
    const char *tmp = str_newDir;
    int len = 0;
    int ret;

    fprintf(stdout, "making directory (%s)\n", str_newDir);
    if(str_newDir == NULL || strlen(str_newDir) >= 256)
    {
        perror("invalid path");
        return NULL;
    }
    while((tmp = strchr(tmp, '/')) != NULL)
    {
        len = tmp - str_newDir;
        tmp++;
        if(len == 0)
            continue;
        strncpy(tmp_path, str_newDir, len);
        tmp_path[len] = 0x00;
        if((ret = mkdir(tmp_path, mode)) == -1)
            if(errno != EEXIST)
            {
                fprintf(stderr, "error: %s\n", strerror(errno));
                return NULL;
            }
    }
    if((ret = mkdir(str_newDir, mode) == -1))
    {
        fprintf(stderr, "error: %s\n", strerror(errno));
        len = strlen(str_newDir);
        char * cr = (char*)malloc(len);
        strncpy(cr, str_newDir, len); 
        return cr;
        //return NULL;
    }
    else 
    {
        len = strlen(str_newDir);
        char * cr = (char*)malloc(len);
        strncpy(cr, str_newDir, len); 
        return cr;
    }
}
const char *MMOUNT = "/proc/mounts";
struct f_size
{
    long blocks;
    long avail; 
};
typedef struct _mountinfo 
{
    FILE *fp;                // 파일 스트림 포인터    
    char devname[80];        // 장치 이름
    char mountdir[80];        // 마운트 디렉토리 이름
    char fstype[12];        // 파일 시스템 타입
    struct f_size size;        // 파일 시스템의 총크기and 사용율 
} MOUNTP;
MOUNTP *dfopen()
{
    MOUNTP *MP;

    // /proc/mounts 파일을 연다.
    MP = (MOUNTP *)malloc(sizeof(MOUNTP));
    if(!(MP->fp = fopen(MMOUNT, "r")))
    {
        return NULL;
    }
    else
        return MP;
}
MOUNTP *dfget(MOUNTP *MP)
{
    char buf[256];
    char *bname;
    char null[16];
    struct statfs lstatfs;
    struct stat lstat; 
    int is_root = 0;

    // /proc/mounts로 부터 마운트된 파티션의 정보를 얻어온다.
    while(fgets(buf, 255, MP->fp))
    {
        is_root = 0;
        sscanf(buf, "%s%s%s",MP->devname, MP->mountdir, MP->fstype);
        if (strcmp(MP->mountdir,"/") == 0) is_root=1;
        if (stat(MP->devname, &lstat) == 0 || is_root)
        {
            if (strstr(buf, MP->mountdir) && S_ISBLK(lstat.st_mode) || is_root)
            {
                // 파일시스템의 총 할당된 크기와 사용량을 구한다.        
                statfs(MP->mountdir, &lstatfs);
                MP->size.blocks = lstatfs.f_blocks * (lstatfs.f_bsize/1024); 
                MP->size.avail  = lstatfs.f_bavail * (lstatfs.f_bsize/1024); 
                return MP;
            }
        }
    }
    rewind(MP->fp);
    return NULL;
}
int dfclose(MOUNTP *MP)
{
    fclose(MP->fp);
}
double storage_available()
{
    MOUNTP *MP;
    if ((MP=dfopen()) == NULL)
    {
        perror("dfopen()");
        return -1;
    }
    dfget(MP);
    return (double)MP->size.avail/(double)MP->size.blocks*100;
}
int rmdirs(const char *path, int is_error_stop)
{
    DIR *  dir_ptr      = NULL;
    struct dirent *file = NULL;
    struct stat   buf;
    char   filename[1024];

    printf("removing %s\n", path);

    if((dir_ptr = opendir(path)) == NULL) {
        return unlink(path);
    }

    while((file = readdir(dir_ptr)) != NULL) {
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }

        sprintf(filename, "%s/%s", path, file->d_name);

        if(lstat(filename, &buf) == -1) {
            continue;
        }

        if(S_ISDIR(buf.st_mode)) {
            if(rmdirs(filename, is_error_stop) == -1 && is_error_stop) {
                return -1;
            }
        } else if(S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)) {
            if(unlink(filename) == -1 && is_error_stop) {
                return -1;
            }
        }
    }
    closedir(dir_ptr);
    return rmdir(path);
}
void * cleaning(void * n)
{
    int i = *(int*)n; 
    struct dirent **namelist;
    int count;
    int index;
    while(1)
    {
        int available = (int)storage_available();
        printf("storage available %d\n", available);
        if(available < i)
        {
            if((count = scandir(path, &namelist, NULL, alphasort)) == -1)
            {
                perror("scandir()");
                return NULL;
            }
            if(rmdirs(namelist[2]->d_name, 1) == -1)
            {
                perror("rmdirs()");
                return NULL;
            }
            for(index = 0; index < count; index++)
                free(namelist[index]);
            printf("the oldest directory deleted for space\n");
        }
        sleep(30);
    }
}
