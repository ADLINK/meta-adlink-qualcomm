/*PWM gpio's are from sx150q I/O expander*/
#define SX150Q_I2C_DEVICE "/dev/i2c-4"
#define SX150Q_SLAVE 0x3E

struct pwm_device {
	int index; //array entry index
	int egpio; //mapped gpio number
	int fd; //device in /dev/
	int duty;
};

struct pwm_device* pwm_device_init(int, char*);
int pwm_set_duty_cycle(struct pwm_device*, int);
int pwm_enable(struct pwm_device*, int);
