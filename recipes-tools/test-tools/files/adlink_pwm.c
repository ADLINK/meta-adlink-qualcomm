
/*sample driver code to use gpiopwm_routines.c functions
 * to enable pwm on expansion pins - Arun Kumar*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpiopwm_routines.h"

void print_help(void) {
	printf("Usage adlink_pwm <expansion-pin> <enable> <duty>\n");
	printf("<enable>: ON or OFF\n");
	printf("<duty>: duty percentage\n\n");
	return;
}

int main(int argc, char **argv)
{
	int pinno; //pin no. on expansion
	int ret, duty, enable;
	struct pwm_device *dev = NULL;

	/*start of reading arguments*/
	if(argc < 3) {
		printf("Error!\n");
		print_help();
		exit(-1);
	}

	pinno = atoi(argv[1]);
	if(!pinno) {
		printf("Error: Invalid pin value\n");
		print_help();
		exit(-1);
	}

	if(strcmp(argv[2], "ON") == 0) enable = 1;
	else if(strcmp(argv[2], "OFF") == 0) enable = 0;
	else {
		printf("Error: Invalid enable value\n");
		print_help();
		exit(-1);
	}

	if(enable && argc < 4) {
		printf("Error: duty value missing\n");
		print_help();
		exit(-1);
	}

	if(enable) {
		duty = atoi(argv[3]);
		if(duty<0 || duty>100) {
			printf("Invalid duty percentage\n");
			print_help();
			exit(-1);
		}
	}
	/*end of reading arguments*/

	dev = pwm_device_init(pinno, SX150Q_I2C_DEVICE);
	if(dev == NULL) {
		printf("Error! pwm init for %s failed\n", SX150Q_I2C_DEVICE);
		exit(-1);
	}

	if(enable == 0) {
		if(pwm_enable(dev, enable) < 0) {
			printf("Pin %d pwm disable failed\n", pinno);
			exit(-1);
		}
		else printf("Pin %d pwm disabled\n", pinno);
		return 0;
	}

	ret = pwm_set_duty_cycle(dev, duty);
	if(ret < 0) exit(-1);

	ret = pwm_enable(dev, enable);
	if(ret < 0) {
		printf("Pin %d pwm enable failed\n", pinno);
		exit(-1);
	}
	printf("Pin %d pwm enabled with duty %d%%\n", pinno, duty);

	return 0;
}
