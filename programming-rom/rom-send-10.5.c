#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

//everything in hex

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}

FILE *target;
char *location;
int fd;

int main(int argc, char* argv[])
{
    char *portname = "/dev/ttyUSB0";
    int rlen, wlen;
    int i;
    int chip;
    int done;
    unsigned char buff[20];

    if (argc != 2) {
      printf("Usage: vup <file to upload>");
      return (-1);
    }
    location = argv[1];
    target = fopen(location, "r");
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return (-1);
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B115200);
    //set_mincount(fd, 0);                /* set to pure timed read */
    
    for (chip = 0, done = 0; chip <= 4; done = 0) {
      getchip(chip);
      while (!done) {
	read(fd, buff, sizeof(buff)-1);
	switch(buff[0]) {
	case 'r':
	  target = reset(chip);
	  break;
	case 'p':
	  sendpage();
	  break;
	case 'e':
	  chip = errusr(chip);
	  done = 1;
	  break;
	}
      }
    }

    close(fd);
    fclose(argv[1]);
    return (0);
}

void getchip(int chip)
{
  printf("insert chip %d, then press enter", chip);
  getchar();
  write(fd, "#", 1);
}

void reset(int chip)
{
  fclose(target);
  target = fopen(location, "r");
  for (; chip > 0; chip--)
    while (fgetc(target) != 'z')
      ;
  write(fd, "#", 1);
}

void sendpage()
{
  char c;
  while ((c = fgetc(target)) != 'x' && c != 'z')
    write(fd, c, 1);
  if (c == 'x') {
    write(fd, "ff", 2);
    write(fd, "#", 1);
  }
  else {
    write(fd, "ffff", 4);
    write(fd, "#", 1);
  }
}

int errusr(int chip)
{
  char buff[20];
  int werr, rerr;
  write(fd, "#", 1);
  read(fd, buff, sizeof(buff)-1);
  printf("%c\n", buff[0]);
  write(fd, "#", 1);
  read(fd, buff, sizeof(buff)-1);
  printf("%c\n", buff[0]);

  return(chip+1);
}
