#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/gpio.h>
#include <sys/time.h>
#include <time.h>

#include <fcntl.h>
#include <paths.h>
#include <libgpio_control.h>


int openGPIO(void){
    int fd;
    char file[] = _PATH_DEVGPIOC "0";
    if ( (fd = open(file, O_RDONLY)) < 0 )  {
        perror("open");
        exit(1);
    }
    return fd;
}

int digitalWrite(int fd,int gpio_pin, int value){
    struct gpio_req gpioreq;
    gpioreq.gp_pin = gpio_pin;
    gpioreq.gp_value = value;    
    if ( ioctl(fd, GPIOSET, &gpioreq) < 0 )  {
        perror("ioctl(GPIOSET)");
        return 0;
    }
    return 1;
}

void delayMicrosecondsHard (unsigned int howLong)
{
  struct timeval tNow, tLong, tEnd ;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = howLong / 1000000 ;
  tLong.tv_usec = howLong % 1000000 ;
  timeradd (&tNow, &tLong, &tEnd) ;

  while (timercmp (&tNow, &tEnd, <))
    gettimeofday (&tNow, NULL) ;
}

void delayMicroseconds (unsigned int howLong)
{
  struct timespec sleeper ;
  unsigned int uSecs = howLong % 1000000 ;
  unsigned int wSecs = howLong / 1000000 ;

  /**/ if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = wSecs ;
    sleeper.tv_nsec = (long)(uSecs * 1000L) ;
    nanosleep (&sleeper, NULL) ;
  }
}
