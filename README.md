raspberry-FBSD-gpio

Code taken from wiringPi and http://project-downloads.drogon.net/files/led/led.c
currently only digitalWrite and delay functions are implemented, using IOCTL
Next step is to make direct memory access to the GPIO, via /dev/mem
 
/*
 *       --a--
 *      |     |
 *      f     b
 *      |     |
 *       --g--
 *      |     |
 *      e     c
 *      |     |
 *       --d--  p
 *
 *
 * connecting into the Pi as follows:
Display Digits are connected to GPIO #*/
DIGIT   GPIO PIN
D1      4
D2      17
D3      27
Display Segments are connected to GPIO #*/
SEGMENT GPIOPIN
A       16
B       7
C       25
D       23
E       18
F       12
G       8
P       24
 
Usage:
To build:
make clean all tar
Install:
copy/extract the tar archive copy lib/libgpio_control.so to /usr/local/lib
./sbin/display

echo "12.5" >> /var/tmp/displayctl

