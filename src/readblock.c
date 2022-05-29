#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BLK_SIZE 4096

unsigned int read_hdd(unsigned char * drv, unsigned int SecAddr, unsigned int blocks, char * buf);

int main()
{
    char buff[512];
    read_hdd("/dev/sdc",34304,1,buff);
    return 0;
}

unsigned int read_hdd(unsigned char * drv, unsigned int SecAddr, unsigned int blocks, char * buf)
{
    unsigned int ret;
    unsigned int bytestoread;
    char buffer[4096];
    int i;
    int fd;

    // /dev/sdb (블록 특수 파일) open
    fd = open(drv,O_RDWR,0666);

    // 위치 설정? 하고 이동.
    lseek(fd, SecAddr * BLK_SIZE, SEEK_SET);

    //읽어야 하는 블록의 크기.
    bytestoread = blocks * BLK_SIZE;

    // 읽기
    ret = read(fd, buffer, bytestoread);

    for(i = 0; i < 4096; i++)
    {
        if(buffer[i] >= 32 && buffer[i] <= 126)
        {
            printf("%c",buffer[i]);
        }
        else{
            printf(".");
        }
    }

    memset(buffer,0,4096);
    close(fd);
    return 0;
}
