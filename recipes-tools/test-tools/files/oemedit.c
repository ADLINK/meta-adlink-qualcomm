#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define PATH_SIZE 256
#define MAX_DEV_LIST 256
#define MAX_BUF_SIZE 256

#define SEMA_I2C_ADDR 0x28

int open_i2c_host(void)
{
	int fd, fd_, ret;
	char path_name[PATH_SIZE] = {0};
	long addr = SEMA_I2C_ADDR << 1;
	char tx_data[5] = {0};
	char rx_data[MAX_BUF_SIZE] = {0};

	struct i2c_msg  msgs[2];
	struct i2c_rdwr_ioctl_data msgset;

	tx_data[0] = 0x30;

	msgs[0].addr  = SEMA_I2C_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf   = tx_data;
	msgs[0].len   = 1;
	msgs[1].addr  = SEMA_I2C_ADDR;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf   = rx_data;
	msgs[1].len   = MAX_BUF_SIZE - 1;

	msgset.msgs  = msgs;
	msgset.nmsgs = 2;

	for(fd_ = 0; fd_ < MAX_DEV_LIST; fd_++)
	{
		memset(path_name, 0, PATH_SIZE);
		sprintf(path_name, "/dev/i2c-%d", fd_);
		if((fd = open(path_name, O_RDWR)) == -1)
		{
			if(errno == EACCES)
			{
				return -EACCES;
			}
			continue;
		}

		if((ret = ioctl(fd, I2C_RDWR, &msgset)) != -1)
		{
			return fd;
		}
		close(fd);
	}

	return -1;
}

int check_is_admin_mode(int fd)
{
	char tx_data[5] = {0};
	char rx_data[MAX_BUF_SIZE] = {0};

	struct i2c_msg  msgs[2];
	struct i2c_rdwr_ioctl_data msgset;

	if(fd < 0)
	{
		return -1;
	}

	tx_data[0] = 0x50;
	tx_data[1] = 0x1;

	msgs[0].addr  = SEMA_I2C_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf   = tx_data;
	msgs[0].len   = 2;
	msgs[1].addr  = SEMA_I2C_ADDR;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf   = rx_data;
	msgs[1].len   = MAX_BUF_SIZE - 1;

	msgset.msgs  = msgs;
	msgset.nmsgs = 2;

	if(ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		return -1;
	}

	if(rx_data[0] == 1)
	{
		return rx_data[1];
	}

	return -1;
}

int enter_admin_mode(int fd)
{
	char tx_data[5] = {0};
	char rx_data[MAX_BUF_SIZE] = {0};

	struct i2c_msg  msgs[2];
	struct i2c_rdwr_ioctl_data msgset;

	if(fd < 0)
	{
		return -1;
	}

	if(check_is_admin_mode(fd) == 1)
	{
		return 0;
	}

	tx_data[0] = 0x50;
	tx_data[1] = 0x4;
	tx_data[2] = 0xAE;
	tx_data[3] = 0xCD;
	tx_data[4] = 0x17;
	tx_data[5] = 0x51;

	msgs[0].addr  = SEMA_I2C_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf   = tx_data;
	msgs[0].len   = 6;

	msgset.msgs  = msgs;
	msgset.nmsgs = 1;

	if(ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		return -1;
	}

	if(check_is_admin_mode(fd) == 1)
	{
		return 0;
	}

	return -1;
}

int leave_admin_mode(int fd)
{
	char tx_data[5] = {0};
	char rx_data[MAX_BUF_SIZE] = {0};

	struct i2c_msg  msgs[2];
	struct i2c_rdwr_ioctl_data msgset;

	if(fd < 0)
	{
		return -1;
	}

	if(check_is_admin_mode(fd) == 0)
	{
		return 0;
	}

	tx_data[0] = 0x5F;
	memset(rx_data, 0, sizeof(rx_data));

	msgs[0].addr  = SEMA_I2C_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf   = tx_data;
	msgs[0].len   = 1;
	msgs[1].addr  = SEMA_I2C_ADDR;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf   = rx_data;
	msgs[1].len   = MAX_BUF_SIZE - 1;

	msgset.msgs  = msgs;
	msgset.nmsgs = 2;

	if(ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		return -1;
	}

	if(check_is_admin_mode(fd) == 0)
	{
		return 0;
	}

	return -1;
}

int write_odm_data(int fd, int offset, char *buf)
{
	int i;
	char tx_data[MAX_BUF_SIZE] = {0};
	char rx_data[MAX_BUF_SIZE] = {0};

	struct i2c_msg  msgs[2];
	struct i2c_rdwr_ioctl_data msgset;

	if(fd < 0)
	{
		return -1;
	}

	if(check_is_admin_mode(fd) == 0)
	{
		return 0;
	}

	if(buf == NULL)
	{
		return -1;
	}

	tx_data[0] = 0x40;
	tx_data[1] = 0x3;
	tx_data[2] = 0x04;
	tx_data[3] = 0x10 * offset;
	tx_data[4] = 0x10;

	msgs[0].addr  = SEMA_I2C_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf   = tx_data;
	msgs[0].len   = 5;

	msgset.msgs  = msgs;
	msgset.nmsgs = 1;

	if(ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		return -1;
	}

	tx_data[0] = 0x41;
	tx_data[1] = 0x10;

	for(i = 0; i < 16; i++)
	{
		tx_data[2+i] = buf[i];
	}

	msgs[0].addr  = SEMA_I2C_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf   = tx_data;
	msgs[0].len   = 0x10 + 2;

	msgset.msgs  = msgs;
	msgset.nmsgs = 1;

	if(ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		return -1;
	}
	return 0;
}

int read_odm_data(int fd, int offset, char *buf, int len)
{
	char tx_data[5] = {0};
	char rx_data[MAX_BUF_SIZE] = {0}, i;

	struct i2c_msg  msgs[2];
	struct i2c_rdwr_ioctl_data msgset;

	if(fd < 0)
	{
		return -1;
	}

	if(check_is_admin_mode(fd) == 0)
	{
		return 0;
	}

	if(buf == NULL)
	{
		return -1;
	}

	tx_data[0] = 0x40;
	tx_data[1] = 0x3;
	tx_data[2] = 0x04;
	tx_data[3] = 0x10 * offset;
	tx_data[4] = 0x10;

	msgs[0].addr  = SEMA_I2C_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf   = tx_data;
	msgs[0].len   = 5;

	msgset.msgs  = msgs;
	msgset.nmsgs = 1;

	if(ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		return -1;
	}

	memset(rx_data, 0, sizeof(rx_data));
	tx_data[0] = 0x42;
	tx_data[1] = 0x10;

	msgs[0].addr  = SEMA_I2C_ADDR;
	msgs[0].flags = 0;
	msgs[0].buf   = tx_data;
	msgs[0].len   = 2;
	msgs[1].addr  = SEMA_I2C_ADDR;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf   = rx_data;
	msgs[1].len   = MAX_BUF_SIZE - 1;

	msgset.msgs  = msgs;
	msgset.nmsgs = 2;

	if(ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		return -1;
	}

	for(i = 0; i < 0x10 && i < len ; i++)
	{
		buf[i] = rx_data[i+1];
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int fd, offset = 1, i, j;

	char buffer[100] = {0};
	unsigned char mac[16] = {0};

	if((fd = open_i2c_host()) < 0)
	{
		if(fd == -EACCES)
		{
			printf("sudo privilege missing\n");
			return -EACCES;
		}
		printf("BMC not found\n");
	}

	if(argc < 2)
	{
		printf("Usage:\n\noemedit entradmin\n\tread\t  <index>\n\twrite\t  <index>  <buffer>\n\texitadmin\n\n");
		printf("index description:\n\t0. Hardware Revision\n\t1. Serial Number\n\t2. Last Repair Date\n\t3. Manufacture Data\n\t4. Second Hardware Revision\n\t5. Second Serial Number\n\t6. MAC Address\n\t7. Reserved.\n");
		return -1;
	}

	if(strcmp(argv[1], "entradmin") == 0)
	{
		if(check_is_admin_mode(fd) == 1)
		{
			printf("BMC is already in Admin mode\n");
			return 0;
		}

		if(enter_admin_mode(fd) != 0)
		{
			printf("Error in entering admin mode\n");
			return -1;
		}

		return 0;
	}

	if(strcmp(argv[1], "exitadmin") == 0)
	{
		if(check_is_admin_mode(fd) == 0)
		{
			printf("BMC is not is admin mode\n");
			return 0;
		}


		if(leave_admin_mode(fd) != 0)
		{
			printf("Error in leaving admin mode\n");
			return -1;
		}

		return 0;
	}

	if(strcmp(argv[1], "write") == 0)
	{
		if(argc != 4)
		{
			printf("Usage:\n\noemedit write  <index>  <buffer>\n\n");
			printf("index description:\n\t0. Hardware Revision\n\t1. Serial Number\n\t2. Last Repair Date\n\t3. Manufacture Data\n\t4. Second Hardware Revision\n\t5. Second Serial Number\n\t6. MAC Address\n\t7. Reserved.\n");
			return -1;
		}

		if(isdigit(argv[2][0]) && strlen(argv[2]) == 1)
		{
			offset = atoi(argv[2]);
		}
		else
		{
			printf("Usage:\n\noemedit write  <0-7>  <buffer>\n\n");
			printf("index description:\n\t0. Hardware Revision\n\t1. Serial Number\n\t2. Last Repair Date\n\t3. Manufacture Data\n\t4. Second Hardware Revision\n\t5. Second Serial Number\n\t6. MAC Address\n\t7. Reserved.\n");
			return -1;
		}

		if(offset < 0 || offset > 7)
		{
			printf("Usage:\n\noemedit write  <0-7>  <buffer>\n\n");
			printf("index description:\n\t0. Hardware Revision\n\t1. Serial Number\n\t2. Last Repair Date\n\t3. Manufacture Data\n\t4. Second Hardware Revision\n\t5. Second Serial Number\n\t6. MAC Address\n\t7. Reserved.\n");
			return -1;
		}

		if(enter_admin_mode(fd) != 0)
		{
			printf("Error in entering admin mode\n");
			return -1;
		}

		int len = strlen(argv[3]);

		for(i = 0, j = 0; i < len; i++, j++)
		{
			int temp = argv[3][i+2];
			argv[3][i+2] = 0;
			sscanf(&(argv[3][i]), "%c", &mac[j]);
			argv[3][i+2] = temp;
		}

		if(write_odm_data(fd, offset, mac) != 0)
		{
			printf("Error in writing ODM data\n");
			return -1;
		}
		else
		{
			printf("Data written successfully\n");
		}

		if(leave_admin_mode(fd) != 0)
		{
			printf("Error in leaving admin mode\n");
			return -1;
		}

		return 0;
	}

	if(strcmp(argv[1], "read") == 0)
	{
		if(argc != 3)
		{
			printf("Usage:\n\noemedit read  <index>\n\n");
			printf("index description:\n\t0. Hardware Revision\n\t1. Serial Number\n\t2. Last Repair Date\n\t3. Manufacture Data\n\t4. Second Hardware Revision\n\t5. Second Serial Number\n\t6. MAC Address\n\t7. Reserved.\n");
			return -1;
		}

		if(isdigit(argv[2][0]) && strlen(argv[2]) == 1)
		{
			offset = atoi(argv[2]);
		}
		else
		{
			printf("Usage:\n\noemedit read  <0-7>\n\n");
			printf("index description:\n\t0. Hardware Revision\n\t1. Serial Number\n\t2. Last Repair Date\n\t3. Manufacture Data\n\t4. Second Hardware Revision\n\t5. Second Serial Number\n\t6. MAC Address\n\t7. Reserved.\n");
			return -1;
		}

		if(offset < 0 || offset > 7)
		{
			printf("Usage:\n\noemedit read  <0-7>\n\n");
			printf("index description:\n\t0. Hardware Revision\n\t1. Serial Number\n\t2. Last Repair Date\n\t3. Manufacture Data\n\t4. Second Hardware Revision\n\t5. Second Serial Number\n\t6. MAC Address\n\t7. Reserved.\n");
			return -1;
		}

		if(enter_admin_mode(fd) != 0)
		{
			printf("Error in entering admin mode\n");
			return -1;
		}

		if(read_odm_data(fd, offset, buffer, sizeof(buffer)) == 0)
		{
			printf("%s\n",buffer);
		}

		if(leave_admin_mode(fd) != 0)
		{
			printf("Error in leaving admin mode\n");
			return -1;
		}

		return 0;
	}
	return -1;
}
