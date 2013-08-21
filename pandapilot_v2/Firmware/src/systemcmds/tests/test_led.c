#include <nuttx/config.h>

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <debug.h>

#include <arch/board/board.h>

#include <drivers/drv_led.h>

static int led_toggle(int leds, int led);
static int led_on(int leds, int led);
static int led_off(int leds, int led);

/****************************************************************************
* Name: test_led
****************************************************************************/

int test_led(int argc, char *argv[])
{
	int	fd;
	int	ret = 0;

	int leds = open(LED_DEVICE_PATH, 0);

	if (leds < 0) {
		printf("\tLED: open fail\n");
		return ERROR;
		}

	int i ;
	
	printf ("\nBOTH LEDs OFF !!\n");
	led_off(leds, LED_GREEN);
	led_off(leds, LED_AMBER);
	usleep (2000000) ;

	printf ("\nGREEN LED ON !!\n");
	led_on(leds, LED_GREEN);
	usleep (2000000) ;

	printf ("\nGREEN LED OFF !!\n");
	led_off(leds, LED_GREEN);
	usleep (2000000) ;
				
	printf ("\nAMBER LED ON !!\n");
	led_on(leds, LED_AMBER);
	usleep (2000000) ;

	printf ("\nAMBER LED OFF !!\n");
	led_off(leds, LED_AMBER);
	usleep (2000000) ;
			
 	printf ("\nBOTH LEDs BLINKING !!\n");

	char ledon = 1 ;
	for (i = 0; i < 50; i++) {
		if (ledon) {
			ioctl(leds, LED_ON, LED_GREEN);
			ioctl(leds, LED_OFF, LED_AMBER);

		} else {
			ioctl(leds, LED_OFF, LED_GREEN);
			ioctl(leds, LED_ON, LED_AMBER);
		}

		ledon = !ledon;
		usleep(60000);
		}

printf("\t LED test completed, no errors.\n");

return ret;
}


static int led_toggle(int leds, int led)
{
	static int last_blue = LED_ON;
	static int last_amber = LED_ON;

	if (led == LED_BLUE) last_blue = (last_blue == LED_ON) ? LED_OFF : LED_ON;

	if (led == LED_AMBER) last_amber = (last_amber == LED_ON) ? LED_OFF : LED_ON;

	return ioctl(leds, ((led == LED_BLUE) ? last_blue : last_amber), led);
}

static int led_off(int leds, int led)
{
	return ioctl(leds, LED_OFF, led);
}

static int led_on(int leds, int led)
{
	return ioctl(leds, LED_ON, led);
}
