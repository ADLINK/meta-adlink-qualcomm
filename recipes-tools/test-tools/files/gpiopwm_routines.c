/* Basic functions to configure pins on 40 pin I/O
 * connector of adlink ipi smarc board. This configures
 * sx1509q I/O expander for pwm operation - Arun Kumar*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <fcntl.h>
#include "gpiopwm_routines.h"

static unsigned char regIon[16]   = {0x2A, 0x2D, 0x30, 0x33, 0x36, 0x3B, 0x40, 0x45, 0x4A};
static unsigned int  IonValue[16]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static unsigned int pinmap[9] = {494, 495, 496, 497, 498, 499, 500, 501, 502};

void print_valid_pinno() {
	printf("Valid pin numbers - \n");
	printf("29, 31, 32, 33, 35, 36, 37, 38, 40\n");
}

int get_pwm_index(int pinno) {
	switch(pinno) {
		case 29:
			return (pinno - 29);
			break;
		case 31:
		case 32:
		case 33:
			return (pinno - 30);
		case 35:
		case 36:
		case 37:
		case 38:
			return (pinno - 31);
		case 40:
			return (pinno - 32);
		default:
			print_valid_pinno();
	}
	return -1;
}

int get_egpio_num(int index) {
	return pinmap[index];
}

int gpio_unexport(int egpio) {
	char buffer[100] = {0};
	int sysfd, i;

	sysfd = open("/sys/class/gpio/unexport", O_WRONLY);
	if(sysfd < 0) {
		perror("error gpio/unexport");
		return -1;
	}
	
	i = sprintf(buffer,"%d", egpio);
	write(sysfd, buffer, i);
	close(sysfd);

	return 0;
}

int gpio_export(int egpio) {
	char buffer[100] = {0};
	int sysfd, i;
	
	sysfd = open("/sys/class/gpio/export", O_WRONLY);
	if(sysfd < 0) {
		perror("error gpio/export");
		return -1;
	}

	i = sprintf(buffer,"%d", egpio);
	write(sysfd, buffer, i);
	close(sysfd);

	return 0;
}

int gpio_direction_out(int egpio) {
	char buffer[100] = {0};
	int sysfd;
	
	sprintf(buffer,"/sys/class/gpio/gpio%d/direction", egpio);
	sysfd = open(buffer, O_WRONLY);
	write(sysfd, "out", 3);
	close(sysfd);

	return 0;
}

int gpio_set_value(int egpio, int val) {
	char buffer[100] = {0};
	int sysfd;

	sprintf(buffer,"/sys/class/gpio/gpio%d/value", egpio);
	sysfd = open(buffer, O_WRONLY);
	if(!val) write(sysfd, "0", 1);
	else write(sysfd, "1", 1);
	close(sysfd);

	return 0;
}

struct pwm_device* pwm_device_init(int pinno, char *i2cdev) {
	int fd, i, add;
	struct pwm_device* dev = (struct pwm_device*)malloc(sizeof(struct pwm_device));
	unsigned char rx_tx_buf[10] = {0};

	dev->index = get_pwm_index(pinno);
	if(dev->index < 0) {
		printf("Error: invalid pinno \n");
		return NULL;
	}

	dev->egpio = get_egpio_num(dev->index);

	/*SX1509 init starts here*/
	if((fd = open(i2cdev, O_RDWR)) < 0) {
		perror("error:");
		return NULL;
	}

	if(ioctl(fd, I2C_SLAVE_FORCE, SX150Q_SLAVE) < 0) {
		perror("error:");
		return NULL;
	}
	dev->fd = fd;

	//configure clock and misc register for pwm feature
	rx_tx_buf[0] = 0x1E;
	if(write(fd, &(rx_tx_buf[0]), 1) == 1) {
		if(read(fd, &(rx_tx_buf[1]), 1) == 1) {
			rx_tx_buf[1] |= (1 << 6);
			rx_tx_buf[1] |= ~(1 << 5);
			if(write(fd, &(rx_tx_buf[0]), 2) != 2) {
				perror("Error:");
        			return NULL;
			}
		}
	}
	else {
		perror("Error:");
		return NULL;
	}

	rx_tx_buf[0] = 0x1F;
	if(write(fd, &(rx_tx_buf[0]), 1) == 1) {
		if(read(fd, &(rx_tx_buf[1]), 1) == 1) {
			rx_tx_buf[1] &= ~(1 << 7);
			rx_tx_buf[1] &= ~(1 << 3);
			rx_tx_buf[1] &= ~((0x7) << 4);
			rx_tx_buf[1] |= ((1 & 0x7) << 4);
			if(write(fd, &(rx_tx_buf[0]), 2) != 2) {
				perror("Error:");
				return NULL;
			}
		}
	}
	/*SX1509 init ends here*/

	/*SX1509 pwm init starts here*/
	add = (dev->index < 8) ? 1 : 0; 
	rx_tx_buf[0] = 0x0 + add;
	rx_tx_buf[2] = 0x6 + add;
	rx_tx_buf[4] = 0xE + add;
	rx_tx_buf[6] = 0x20 + add;
	rx_tx_buf[8] = 0x10 + add;
	
	for(i = 0; i < 9; i += 2) {
		if(write(fd, &(rx_tx_buf[i]), 1) != 1) {
			perror("Error:");
			return NULL;
                }
		if(read(fd, &(rx_tx_buf[i + 1]), 1) != 1) {
			return NULL;
		}

		if((i == 2) || (i == 6))
			rx_tx_buf[i+1] |= (1 << (dev->index % 8));
		else
			rx_tx_buf[i+1]  &= ~(1 << (dev->index % 8));
		if(write(fd, &(rx_tx_buf[i]), 2) != 2) {
			perror("Error:");
			return NULL;
		}
        }
	/*SX1509 pwm init ends here*/

	/*initialize sysfs gpio that maps to I/O expander pin*/
	gpio_unexport(dev->egpio); //unexport if already exist
	if(gpio_export(dev->egpio) < 0)
		return NULL;
	gpio_direction_out(dev->egpio);
	gpio_set_value(dev->egpio, 1);
	usleep(10000);
	gpio_set_value(dev->egpio, 0);

	return dev;
}

int pwm_set_duty_cycle(struct pwm_device *dev, int duty) {
	if(dev == NULL) {
		printf("Error! pwm device not initialized\n");
		return -1;
	}
	dev->duty = duty;
        return 0;
}

int pwm_enable(struct pwm_device *dev, int enable) {
	unsigned char rx_tx_buf[3] = {0};
	int fd = dev->fd;

	if(dev == NULL) {
		printf("Error! pwm device not initialized\n");
		return -1;
	}

	if(enable) {
		IonValue[dev->index] = ((float)(100-dev->duty)/100) * 255;
	} else {
		IonValue[dev->index] = 0xFF;
		gpio_set_value(dev->egpio, 0);
		gpio_unexport(dev->egpio);
	}

	rx_tx_buf[0] = regIon[dev->index];
	rx_tx_buf[1] = IonValue[dev->index];
	if(write(fd, &(rx_tx_buf[0]), 2) != 2) {
		printf("Error! pwm enable: %d failed\n", enable);
		return -1;
	}
	close(fd);
	return 0;
}
