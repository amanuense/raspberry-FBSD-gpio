#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
/*#include <sched.h>*/
#include <pthread.h>
#include <sys/gpio.h>
#include <sys/time.h>
#include <time.h>
#include<signal.h>
#include <fcntl.h>
#include <paths.h>
#include <sched.h>
#include <libgpio_control.h>
#include "display.h"
#include <sys/stat.h>
#include <sys/types.h>

//Global FD For GPIO.
int gpio_fd,fifo_fd;

void* read_pipe(void *a){
    mkfifo(PIPE_FILE, 0777);
    while(1){
        char buf[MAX_DIGITS+1];
        int fifo; 
        fifo = open(PIPE_FILE,O_RDONLY);
        if (fifo < 0){
            perror("FIFO File didn't existed!, creating it!\n");
            exit(-1);        
        }
        fifo_fd = fifo;
        /* open, read, and display the message from the FIFO */
        read(fifo, buf, sizeof(buf));
        close(fifo_fd);
        printf("Received: %s\n", buf);
        digits[0] = buf[0];
        digits[1] = buf[1];
        digits[2] = buf[2];
    }
    
    
}
void sig_handler(int signal){
    if((signal == SIGINT) || (signal == SIGTERM)|| (signal == SIGKILL)){
        perror("Recieved Signal, deactivating GPIO PINS!\n");
        for(int i=0;i<< 11;i++){
            digitalWrite(gpio_fd,segmentPins[i],0);
        }
        close(gpio_fd);
        close(fifo_fd);
        system("rm /var/tmp/displayctl");
        exit(signal);
    }else{
        perror("Recieved Signall but Dunno What to do!");
    }
}

void *displayDigits (void *dummy){
  uint8_t digit, segment ;
  uint8_t index, d, segVal ;
  for (;;)
  {    
        for (digit = 0 ; digit < 3 ; ++digit)
            {
                //Enable the Digit to Print!
                //GPIO Pin is taken from segmentPins Array
                digitalWrite(gpio_fd,digitPins[digit], HIGH) ;
                for (segment = 0 ; segment < 8 ; ++segment)
                    {
                        int segment_pin = segmentPins[segment];
                        d = toupper (digits [digit]) ;
                        if((d >= '0') && (d <= '9'))	// Digit
                            index = d - '0' ;
                        else if ((d >= 'A') && (d <= 'F'))	// Hex
                            index = d - 'A' + 10 ;
                        else
                            index = 16 ;				// Blank
                        segVal = segmentDigits [index * 8 + segment] ;
                        digitalWrite(gpio_fd,segment_pin, !segVal) ;
                        delayMicroseconds (140) ;
                        digitalWrite (gpio_fd,segment_pin,HIGH) ;
                    }
                //Turn off the Digit
                digitalWrite (gpio_fd,digitPins[digit],LOW) ;
        }    
  } 
}

void setHighPri (void){
  struct sched_param sched ;
  memset (&sched, 0, sizeof(sched)) ;
  sched.sched_priority = 29 ;
  if (sched_setscheduler (0, SCHED_FIFO, &sched))
    printf ("Warning: Unable to set high priority\n") ;
}


int setup(void){
    //Open the GPIO Device
    int i;
    pthread_t displayThread, pipeReadThread ;
    //Register Signals
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    if (signal(SIGTERM, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGTERM\n");
    //Open the GPIO PORT
    gpio_fd = openGPIO();
    
    
    for(int i=0;i<NUM_GPIO_PINS_USED;i++){
        struct gpio_pin gpiopin;
        //struct gpio_req gpioreq;
        gpiopin.gp_pin = allPins[i];
        gpiopin.gp_flags = GPIO_PIN_OUTPUT;
        //Next Version make this a function!
        if ( ioctl(gpio_fd, GPIOSETCONFIG, &gpiopin) < 0 )  {
            perror("ioctl(GPIOSETCONFIG)");
            exit(1);
        }
    }
    setHighPri () ;
    strcpy (digits, "AAA") ;

    if (pthread_create (&displayThread, NULL, displayDigits, NULL) != 0)
        {
            printf ("thread create failed: %s\n", strerror (errno)) ;
            exit (1) ;
        }
    sleep(1) ; // Just to make sure it's started

    // Quick countdown LED test sort of thing

    for (i = 100 ; i < 112 ; ++i)
        {
        sprintf (digits, "%d", i) ;
        printf("%d\n", i);
        sleep(1) ;
        }
    if (pthread_create (&pipeReadThread, NULL, read_pipe, NULL) != 0)
        {
            printf ("thread create failed: %s\n", strerror (errno)) ;
            exit (1) ;
        }
    sleep(1) ; // Just to make sure it's started
    return 1;
}



int main(void){
    setup();
    while(1){
        sleep(1);
    }
    return 0;
}
