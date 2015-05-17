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

void config_gpio(int mode){
    for(int i=0;i<NUM_GPIO_PINS_USED;i++){
        struct gpio_pin gpiopin;
        //struct gpio_req gpioreq;
        gpiopin.gp_pin = allPins[i];
        gpiopin.gp_flags = mode;
        //Next Version make this a function!
        if ( ioctl(gpio_fd, GPIOSETCONFIG, &gpiopin) < 0 )  {
            perror("ioctl(GPIOSETCONFIG)");
            exit(1);
        }
    }
}

void* read_pipe(void *a){
    while(1){
        int buflen = MAX_DIGITS+1 , skip_char = -1, point_count = 0;
        char buf[buflen];
        FILE *fifo; 
        fifo = fopen(PIPE_FILE,"ro");
        if (fifo < 0){
            perror("Can't open Fifo\n");
            exit(-1);        
        }
        fifo_fd = fifo;
        /* open, read, and display the message from the FIFO */
        fgets(buf, buflen, fifo);
        fclose(fifo_fd);
        printf("Received: %s\n", buf);
        for(int i = 0; i < MAX_DIGITS; i++){
            if(buf[i+1] == '.'){
                point_at_digit = i;
                //Fix bug when .000 -
                printf("POINT FOUND AT DIGIT %d",point_at_digit);
                skip_char = i+1;
                point_count ++;
            }
        }
        /*If no point found restore point_at_digit*/
        if(point_count == 0){
            printf("Restoring point_at_digit to -1 because no points\n");
            point_at_digit = -1;   
        }
        //pack digits and verify point count
        if(skip_char > 0){
            if(point_count > 1){
                perror("String with more that two Points!");
                continue;
            }
            int cnt = 0;
            for(int i=0; i < MAX_DIGITS; i++){
                if(i != skip_char){
                    //FIX THIS, to prevent overflow of digits[3] do not assign more than 3 chars
                    if(cnt < 3){
                        digits[cnt] = buf[i];
                        cnt++;
                    }
                }
            }    
        }else{
            digits[0] = buf[0];
            digits[1] = buf[1];
            digits[2] = buf[2];   
        }
    }
    
    
}
void sig_handler(int signal){
    if((signal == SIGINT) || (signal == SIGTERM)|| (signal == SIGKILL)){
        printf("Terminating Threads!\n");
        pthread_cancel(displayThread);
        usleep(500);
        pthread_cancel(pipeReadThread);
        usleep(500);
        printf("Deactivating GPIO PINS!\n");
        for(int i=0;i < 11;i++){
            digitalWrite(gpio_fd,allPins[i],0);
            usleep(500);
        }
        config_gpio(GPIO_PIN_INPUT);
        close(gpio_fd);
        fclose(fifo_fd);
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
                        /*
                         *if Segment that has the point is being turned on, turn on the point
                        */
                        if(point_at_digit == digit){
                            digitalWrite(gpio_fd,P, LOW) ;
                        }
                        delayMicroseconds (140) ;
                        if(point_at_digit == digit){
                            digitalWrite(gpio_fd,P, HIGH) ;
                        }
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
    
    //Register Signals
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    if (signal(SIGTERM, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGTERM\n");
    //Open the GPIO PORT
    gpio_fd = openGPIO();
    mkfifo(PIPE_FILE, 0777);
    config_gpio(GPIO_PIN_OUTPUT);
    setHighPri () ;
    strcpy (digits, "AAA") ;

    if (pthread_create (&displayThread, NULL, displayDigits, NULL) != 0)
        {
            printf ("thread create failed: %s\n", strerror (errno)) ;
            exit (1) ;
        }
    sleep(1) ; // Just to make sure it's started

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
