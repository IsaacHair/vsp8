#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

int main()
{
  struct termios options;
  
  int fd;
  int n;
  char buf[200];
  int i, j;
  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    printf("err opening\n");
    return(-1);
  }
  else {
    fcntl(fd, F_SETFL, 0);
    printf("%d\n", fd);
  }


/*
 * Get the current options for the port...
 */

tcgetattr(fd, &options);

/*
 * Set the baud rates to 9600...
 */

cfsetispeed(&options, B9600);
cfsetospeed(&options, B9600);

/*
 * Enable the receiver and set local mode...
 */

options.c_cflag |= (CLOCAL | CREAD);

/*
 * Set the new options for the port...
 */

tcsetattr(fd, TCSANOW, &options);

 options.c_cflag &= ~PARENB;
 options.c_cflag &= ~CSTOPB;
options.c_cflag &= ~CSIZE;
options.c_cflag |= CS8;



//send A
n = write(fd, "A", 1);
if (n < 0)
  printf("err sending\n");
 else
   printf("sent ok; %d\n", n);

//read
 for (i = 0; i < 10; i++) {
   printf("%c", readchar(fd));
 }
 

 close(fd);
 return(0);
}
