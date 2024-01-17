#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>


int GPIOSetDirection( int gpio,int Direction)
{
	int fd,ret;
	char sysfile[256];

	if(Direction)
       	{
		sprintf(sysfile, "/sys/class/gpio/gpio%d/direction", gpio);
		fd = open(sysfile, O_RDWR);
  		if(fd < 0){
			return -1;
		}

		ret = write(fd, "in", 3);	
	
		if (ret)
			close(fd);
		else
       		{
			close(fd);
			return -1;
		}

	}
	else{
		sprintf(sysfile, "/sys/class/gpio/gpio%d/direction", gpio);
		  fd = open(sysfile, O_RDWR);
                if(fd < 0){
                	return -1;
                }

        	ret = write(fd, "out", 4);
        	if (ret)
               		 close(fd);
		else
		{
                	close(fd);
                	return -1;
       		 }
        }	
        return 0;
}

int GPIOSetLevel( int gpio, int value)
{
        int fd,ret;
        char sysfile[256];

        if(value)
        {
                sprintf(sysfile, "/sys/class/gpio/gpio%d/value", gpio);
                fd = open(sysfile, O_RDWR);
                if(fd < 0){
                        return -1;
                }

                ret = write(fd, "1", 2);

                if (ret)
                        close(fd);
                else
                {
                        close(fd);
                        return -1;
                }
		

        }
	 else{
                sprintf(sysfile, "/sys/class/gpio/gpio%d/value", gpio);
                  fd = open(sysfile, O_RDWR);
                if(fd < 0){
                        return -1;
                }

                ret = write(fd, "0", 2);
                if (ret)
                         close(fd);
                else
                {
                        close(fd);
                        return -1;
                 }
        }
        return 0;
}

int GPIOGetDirection( int gpio)
{
	
	int ret,fd;
	char sysfile[256];
	char value[256];
	

	sprintf(sysfile, "/sys/class/gpio/gpio%d/direction", gpio);
			
	fd = open(sysfile, O_RDONLY);
	if(fd < 0)
		return -1;

	ret = read(fd, value, sizeof(value));	
	if (ret)
		close(fd);
	else {
		close(fd);
		return -1;
	}



	if(strncmp(value, "in", strlen("in")) == 0) {
			printf("Direction: Input\n");
			}
	if(strncmp(value, "out", strlen("out")) == 0) {
			 printf("Direction: Output\n");
			}

        return 0;
}
int GPIOGetLevel( int gpio)
{

        int ret,fd;
        char sysfile[256];
        char value[256];


        sprintf(sysfile, "/sys/class/gpio/gpio%d/value", gpio);

        fd = open(sysfile, O_RDONLY);
        if(fd < 0)
                return -1;

        ret = read(fd, value, sizeof(value));
        if (ret)
                close(fd);
        else {
                close(fd);
                return -1;
        }

        if(strncmp(value, "1", strlen("1")) == 0) 
		printf("Value : 1\n");
			
	if(strncmp(value, "0", strlen("0")) ==0)
		printf("Value : 0\n");

        return 0;
}

void  initialize_gpio(int gpio)
{
		char export[256];
		DIR *dr;
		sprintf(export, "/sys/class/gpio/gpio%d", gpio);
		
		dr = opendir(export);
		if (dr == NULL) 
		{		
			sprintf(export, "echo %d > /sys/class/gpio/export", gpio);
			system(export);
		}
		else
		{
			closedir(dr);
		}
	

}

void GpioPinUpdate (int *gpio)
{
	int bit;
	bit= *gpio;
	switch (bit)
	{
		case 7:
			*gpio =367;
			break;

		case 12:
			*gpio=420;
			break;

		case 11:
			*gpio=454;
			break;

		 case 13:
			*gpio=455;
			break;

		 case 15:
			*gpio=457;
			break;

		 case 16:
			*gpio=459;
			break;
		
		 case 18:
			*gpio=461;
			break;

		 case 22:
			*gpio=463;
			break;

		 case 29:
			*gpio=280;
			break;

		 case 31:
			*gpio=281;
			break;

		 case 32:
			*gpio=282;
			break;

		 case 33:
			*gpio=283;
			break;

		case 35:
			*gpio=284;
			break;

		 case 36:
			*gpio=285;
			break;

		 case 37:
			*gpio=286;
			break;

		 case 38:
			*gpio=287;
			break;

		case 40:
			*gpio=288;
			break;
		
		default:
			printf(" Not present\n");
			break;		


	}

}	

void uninitiaize_gpio(int gpio)
{
	char unexport[256];
	 sprintf(unexport, "echo %d > /sys/class/gpio/unexport", gpio); 
	 system(unexport);

}


void ShowHelp(void)
{
        printf("GPIO:\n");
                printf("  gpio  get_direction     <GPIO Bit>\n");
                printf("  gpio  set_direction     <GPIO Bit> <0 - Output or 1 - Input>\n");
                printf("  gpio  get_level         <GPIO Bit>\n");
                printf("  gpio  set_level         <GPIO Bit> <0 - Low or 1 - High>\n");

}


int ParseArgs(int argc, char* argv[])
{
	
	int eRet = 1;
	if (argc == 1)
	{
		ShowHelp();
		
	}


	else if (strcasecmp(argv[1], "gpio") == 0)
	{
		  
		int ret=0,gpio;	

		 if (argc < 4)
        	{
                ShowHelp();
       		return -1; 
		}
		// Library Initializing
		 gpio = atoi(argv[3]);
		 GpioPinUpdate(&gpio);
       		  initialize_gpio(gpio);
        	 	 

		 if (argc == 4 && (strcasecmp(argv[2], "get_direction") == 0))
		{
			int bitmask=0;
                        bitmask = atoi(argv[3]);

               		GpioPinUpdate(&bitmask);
                        ret =GPIOGetDirection( bitmask);
                        if (ret<0)
                        {
                        printf("get information failed\n");
                        }
		
		}
		else if (argc == 5 && (strcasecmp(argv[2], "set_direction") == 0))
		{
			
                        int bitmask=0, dir=0;
                        bitmask = atoi(argv[3]);
                        dir = atoi(argv[4]);
			 GpioPinUpdate(&bitmask);
                        ret =GPIOSetDirection( bitmask, dir);
                        if (ret<0)
                        printf("get information failed\n");
                        else
			printf("GPIOSetDirection success\n");

		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_level") == 0))
		{
			int bitmask=0;
                        bitmask = atoi(argv[3]);
			 GpioPinUpdate(&bitmask);
                        ret =GPIOGetLevel(bitmask);
                        if (ret<0)
                        {
                        printf("get information failed\n");
                        }
                       
		}
		else if (argc == 5 && (strcasecmp(argv[2], "set_level") == 0))
		{
			int bitmask=0, val=0;
                        bitmask = atoi(argv[3]);
                        val = atoi(argv[4]);
			 GpioPinUpdate(&bitmask);
                        ret =GPIOSetLevel( bitmask,val);
                        if (ret<0)
                        printf("get information failed\n");
                        else
                         printf("GPIOSetLevel success\n");
		}
		else
		{
			 ShowHelp();;
		}
	}
	return 0;
}


int main(int argc , char* argv[])
{

	int gpio = atoi(argv[3]);
	if (ParseArgs(argc, argv) < 0)
	{
		printf("Argument Failed \n");
	
	}
	 GpioPinUpdate(&gpio);
	 uninitiaize_gpio(gpio);	
	return 0;
}
