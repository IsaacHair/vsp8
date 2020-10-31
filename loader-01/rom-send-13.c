#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

//everything in hex except for error count, which is in decimal

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
  char holder[2];
  while ((c = fgetc(target)) != 'x' && c != 'z') {
    holder[0] = c;
    holder[1] = '\0';
    write(fd, holder, 1);
  }
  if (c == 'x')
    write(fd, "ff", 2);
  else
    write(fd, "ffff", 4);
}

int errusr(int chip)
{
  char buff[20];
  int len;
  int werr, rerr;
  write(fd, "#", 1);
  len = read(fd, buff, sizeof(buff)-1);
  buff[len] = '\0';
  printf("errors during write: %s\n", buff);
  write(fd, "#", 1);
  len = read(fd, buff, sizeof(buff)-1);
  buff[len] = '\0';
  printf("errors during check: %s\n", buff);
  printf("re-write? (y/n)");
  if (getchar() == 'n')
    chip++;
  getchar();
  return(chip);
}

int main(int argc, char* argv[])
{
    char *portname = "/dev/ttyUSB0";
    int rlen, wlen;
    int i;
    int chip;
    int done;
    unsigned char buff[20];

    if (argc != 2) {
      printf("Usage: ./vrs <file to upload>\n");
      return (-1);
    }
    printf("Reset arduino, then press enter");
    getchar();
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
	  reset(chip);
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
    fclose(target);
    return (0);
}
