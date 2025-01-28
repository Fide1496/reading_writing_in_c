#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

struct data
{
    double ax;
    double ay;
    double az;
    
    double wx;
    double wy;
    double wz;

    double roll;
    double pitch;
    double yaw;
};

int checkError(int val, const char *msg){
    if (val == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main( )
{
    int fd;
    int i;
    struct data valz[20];
    int count = 20;
    int bytes = 0;
    

    fd = checkError(open("raw.dat",O_RDONLY, S_IRUSR), "open for read"); 
    
    bytes = checkError(read(fd, valz, sizeof(struct data)*20), "read");

    for (i = 0; i< bytes/sizeof(struct data);i++){
        printf("ax: %lf  ay: %lf   az: %lf\n", valz[i].ax, valz[i].ay,valz[i].az);

        printf("wx: %lf  wy: %lf   wz: %lf\n", valz[i].wx, valz[i].wy,valz[i].wz);

        printf("roll: %lf  pitch: %lf   yaw: %lf\n", valz[i].roll, valz[i].pitch,valz[i].yaw);
    }

    close(fd);
    exit(EXIT_SUCCESS);
}