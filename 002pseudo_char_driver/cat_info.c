#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define DEV_MEM_SIZE 512

int main()
{
    int fd;
    static char buffer[DEV_MEM_SIZE];
    int len, ret;



// Pass you sysfs device as input argument
    fd = open("/dev/pcd", O_RDWR);
    printf("fd = %d\n", fd);

    if (fd<0)
    {
        perror("can not open file");
        return errno;
    }

// Read from sysfs file

   len = read(fd, buffer, DEV_MEM_SIZE);
    if (len < 0)
    {
        perror("read error\n");
        return errno;
    }
    buffer[len]='\0';

    printf("Value read from char device = %s\n",buffer);

   // strcpy (buffer, "writing in driver");
    //len=strlen(buffer);

/*
// Write into sysfs file
    ret = write(fd,buffer,len);
    if (ret != len)
        printf("can not write in file - return value = %d\n", ret);
*/


// Close sysfs file

   close(fd);
}

