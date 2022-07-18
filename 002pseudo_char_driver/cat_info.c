#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define DEV_MEM_SIZE 512
static char receive[DEV_MEM_SIZE];

int main()
{
    int fd, ret;
    char buffer[DEV_MEM_SIZE];
    //char recieve[DEV_MEM_SIZE];

// Pass you sysfs device as input argument
    fd = open("/dev/pcd", O_RDWR);
    printf("fd = %d\n", fd);

    if (fd<0)
    {
        perror("can not open file");
        return errno;
    }

// Read from sysfs file

    printf("Type something to the file /dev/pcd: \n");
    scanf("%[^\n]%*c", buffer);
    printf("Writing Messege to the file /dev/pcd\n");
    printf("%s\n", buffer);
    printf("%d\n", fd);
    ret = write(fd, buffer, strlen(buffer));
    if(ret < 0) {
    	perror("cannot write in the file");
    	return errno;
    }
    
    printf("Press Enter to read from the /dev/pcd file");
    getchar();

   // ret = 0;
    printf("Reading from the /dev/pcd your content is: \n");
   
    int location = lseek(fd, 0, SEEK_SET);
	
    ret = read(fd, receive, DEV_MEM_SIZE);
    printf("ret = %d\n", ret);
    if(ret < 0)
    {
        perror("read error\n");
        return errno;
    }
    printf("Value read from char device = %s", receive);

// Close sysfs file

   close(fd);
}

