/*

*/

//Constant Defs
// Pin modes

#define	INPUT			 0
#define	OUTPUT			 1
#define	PWM_OUTPUT		 2
#define	GPIO_CLOCK		 3
#define	SOFT_PWM_OUTPUT		 4
#define	SOFT_TONE_OUTPUT	 5
#define	PWM_TONE_OUTPUT		 6

#define	LOW			 0
#define	HIGH			 1

// Pull up/down/none

#define	PUD_OFF			 0
#define	PUD_DOWN		 1
#define	PUD_UP			 2

// PWM

#define	PWM_MODE_MS		0
#define	PWM_MODE_BAL		1


//Variable definitions:
//Gpio device
char ctlfile[] = _PATH_DEVGPIOC "0";



//Prototype Definitions

extern int digitalWrite(int fd, int gpio_pin, int value);
extern void delayMicrosecondsHard (unsigned int howLong);
extern void delayMicroseconds (unsigned int howLong);
extern int openGPIO(void);
