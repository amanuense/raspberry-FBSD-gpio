/* Display Digits are connected to GPIO #*/
#define	D1	4
#define	D2	17
#define	D3	27
/* Display Segments are connected to GPIO #*/
#define A 16
#define B 7
#define C 25
#define D 23
#define E 18
#define F 12
#define G 8
#define P 24

#define NUM_GPIO_PINS_USED 11

#define PIPE_FILE "/var/tmp/displayctl"
#define MAX_DIGITS 3
/*Array for of above definitions*/

static const uint8_t digitPins [] =
    {
        4,17,27
    };


/*Display Segments Pins vector mapping*/
/*This Mapping does not include the point (P) */
static const uint8_t segmentPins [] =
    {
        16,7,25,23,18,12,8,24
    };

/*Array holding all the pins to be used For initialization purposes*/
static const uint8_t allPins [] =
    {
        27,17,4,16,7,25,23,18,12,8,24
    };

static const uint8_t segmentDigits [] =
{
// a  b  c  d  e  f  g  p	Segments
   1, 1, 1, 1, 1, 1, 0, 0,	// 0
   0, 1, 1, 0, 0, 0, 0, 0,	// 1
   1, 1, 0, 1, 1, 0, 1, 0,	// 2
   1, 1, 1, 1, 0, 0, 1, 0,	// 3
   0, 1, 1, 0, 0, 1, 1, 0,	// 4
   1, 0, 1, 1, 0, 1, 1, 0,	// 5
   1, 0, 1, 1, 1, 1, 1, 0,	// 6
   1, 1, 1, 0, 0, 0, 0, 0,	// 7
   1, 1, 1, 1, 1, 1, 1, 0,	// 8
   1, 1, 1, 1, 0, 1, 1, 0,	// 9
   1, 1, 1, 0, 1, 1, 1, 0,	// A
   0, 0, 1, 1, 1, 1, 1, 0,	// b
   1, 0, 0, 1, 1, 1, 0, 0,	// C
   0, 1, 1, 1, 1, 0, 1, 0,	// d
   1, 0, 0, 1, 1, 1, 1, 0,	// E
   1, 0, 0, 0, 1, 1, 1, 0,	// F
   0, 0, 0, 0, 0, 0, 0, 0,	// blank
} ;
 

// digits:
//	A global variable which is written to by the main program and
//	read from by the thread that updates the display. Only the first
//	4 digits (ascii) are used.

char digits[3];
