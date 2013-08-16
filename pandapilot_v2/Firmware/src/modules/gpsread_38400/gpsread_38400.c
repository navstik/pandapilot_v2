#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <poll.h>
 
__EXPORT int gpsread_38400_main(int argc, char *argv[]);
 
int gpsread_38400_main(int argc, char *argv[])
{
  int fd; /* File descriptor for the port */
  char c;
 
  /*GPS is connected to the controller via UART2 and the port for uart 2 is /dev/ttyS1 */
  fd = open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NDELAY);/*Opening the port */
 
  printf("Port opened\n");
 
  /* Try to set baud rate */
  struct termios uart_config;
 
  tcgetattr(fd, &uart_config); // get what is the current baud rate
 
   /* Setting baud rate of UART to 38400 for communicating with GPS at its default baudrate of 38400 */
   cfsetispeed(&uart_config, B38400);
   cfsetospeed(&uart_config, B38400);
 
  tcsetattr(fd, TCSANOW, &uart_config); //confirming that the baud rate is set
 
   int i=0,n=0;
 
 struct pollfd fds;
	fds.fd = fd;
	fds.events = POLLIN;
 
while (i<100)
{   if (poll(&fds, 1, 1000)>0)
    {
     n=read(fd,&c,1); // reading and printing the characters coming from GPS to ensure the connection with gps
     if (n<0)
     printf("read failed");
 
     printf("%c ",c); // printing the characters on console
   }
i++;
}
  close(fd);
 
 printf("\nPort Closed\n");
}