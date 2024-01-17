#include<stdio.h> 			/*printf()/scanf()*/ 
#include <stdlib.h>			/* system() */
#include<string.h>			/*strcpy()*/
#include <unistd.h> 		/*usleep*/
#include <sys/stat.h>		/*open()*/
#include <fcntl.h>  		/* open()*/
#include <linux/i2c.h>		/* struct i2c_rdwr_ioctl_data */ 		
#include <linux/i2c-dev.h> 	/*struct i2c_msg*/
#include <sys/ioctl.h>		/*ioctl()*/

#ifdef __x86__
#include <sys/io.h>   	/*in()/out()*/
#endif
#include<signal.h>

#include "ad-litbmc-fwupd-src.h"  	/* all macro's used in this application are defined */

unsigned int __updatefilesize = 0;
int g_iDevHandle = 0;
unsigned char g_ucIsFileSelect = 0;
unsigned char g_ucIsUpdateRecovery = 0;
/**
 ************************************************************************************
 *       \FUNCTION       int SetUpdateFileSize(unsigned int)
 *       \DESCRIPTION    This function is used to update global variable with file(.bin)
 *       		size.
 ************************************************************************************
 **/
void SetUpdateFileSize(unsigned int in_uiFileSize)
{
	__updatefilesize = in_uiFileSize;
}

/**
 ************************************************************************************
 *       \FUNCTION       int GetUpdateFileSize(void)
 *       \DESCRIPTION    This function is used to know the file(.bin) size.
 ************************************************************************************
 **/
unsigned int GetUpdateFileSize(void)
{
	return __updatefilesize;
}

/**
 ************************************************************************************
 *       \FUNCTION       int OpenI2CDevice(void)
 *       \DESCRIPTION    This function is used to open a I2C device file and 
 *       		return handler which need to be used in all other functions
 *       		This function should be called in begining of application.
 ************************************************************************************
 **/
int OpenI2CDevice(void)
{
	unsigned char ucBusNo = 0;	
	int iHandle;
	char cBuff[50] = {0};
	int iRetVal = 0;
	int iIndex = 0;
	int iCount = 0;
	FILE *ptr;
	char version1[BMC_V1_MAX_LEN+2] = {0};

	/* verifiying how many i2c buses are there */
	ptr = popen("i2cdetect -l | wc -l", "r");
	fread(cBuff, 1, 1, ptr);
	iCount = atoi(cBuff);

	for(iIndex = 0; iIndex < iCount; iIndex++)
	{
		cBuff[sprintf(cBuff, "/dev/i2c-%d", iIndex)] = 0;
		if((iHandle = open(cBuff, O_RDWR)) < 0)
		{
			return -1;
		}

		g_iDevHandle = iHandle;

		/* This is just for validating that opened i2c bus is actually connected to our slave(here:0x28) address or not */
		iRetVal = Sema_LiteBMCGetVersion(version1, SEMA_CMD_RD_VERSION1);
		if(iRetVal == SEMA_FAILURE)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	/* No I2C device is attached to our device*/
	if(iIndex == iCount)
	{
		iHandle = -1;
	}

	g_iDevHandle = iHandle;

	return iHandle;
}

/**
 ************************************************************************************************************************************
 *       \FUNCTION       int Sema_UpdateLiteBMC(char* in_cFileName)
 *   	\DESCRIPTION 	This function is used to update Lite BMC, by writing into firmware file(.bin).  
 *
 ************************************************************************************************************************************
 **/
int Sema_UpdateLiteBMC(char* in_cFileName)
{
	FILE *fp = NULL;
	struct  stat fileInfo;
	int fileLength = 0;
	unsigned int update_filesize = GetUpdateFileSize();
	int	address = 0;
	int iRetVal = 0;
	unsigned int    uiIndex = 0;
	int	writeoffset = 0;
	char log[100];
	char buffer[SMB_TRANS_BUFFER_LEN] = { 0 };

	fp = fopen(in_cFileName, "rb");
	if(fp == NULL)
	{
		return EC_FILE_NOT_FOUND;
	}

	/* get full information of file for future purpose */
	if (stat(in_cFileName, &fileInfo) != 0)
	{
		fclose(fp);
		sprintf(log,"ERROR: Cannot get file information for file \"%s\".\n", in_cFileName);
		printf("\n%s", log);
		return EC_FILE_ACCESS_ERROR;
	}

	fileLength = fileInfo.st_size;
	update_filesize = fileLength;
	SetUpdateFileSize(fileLength);

	if(Sema_LiteBMCEnterBootLoader() != SEMA_SUCCESS)
	{
		return EC_FAILED;
	}

	address = SEMA_ADDRESSTO_DOWNLOAD_BL;
	buffer[0] = SEMA_CMD_BL_DOWNLOAD;
	buffer[1] = (char)(address >> 24);
	buffer[2] = (char)(address >> 16);
	buffer[3] = (char)(address >> 8);
	buffer[4] = (char)(address);
	buffer[5] = (char)(fileLength >> 24);
	buffer[6] = (char)(fileLength >> 16);
	buffer[7] = (char)(fileLength >> 8);
	buffer[8] = (char)(fileLength);

	if(Sema_LiteBMCSendCommand(buffer, 9) < 0)
	{
		return EC_FAILED;
	}

	writeoffset = 0;
	fflush(stdout);
	printf("\n");
	do
	{
		char bytessend;
		buffer[0] = SEMA_CMD_BL_SEND_DATA;
		//printf("%08ld", (long)fileLength);
		//printf("%3d%%\r", (int)((writeoffset*100.0) / update_filesize));
		printf("Updating...%3d%%", (int)((writeoffset*100.0) / update_filesize));
		fflush(stdout);

		SleepMS(SEND_EREASE_DELAY);
		
		printf("\r");

		if (fileLength >= 0x1C)
		{
			for (uiIndex = 0; uiIndex<0x1C; ++uiIndex)
			{
				if (!feof(fp))
				{
					buffer[1 + uiIndex] = fgetc(fp);
				}
			}
			writeoffset += 0x1C;
			fileLength -= 0x1C;
			bytessend = 0x1C + 1;
		}
		else
		{
			for (uiIndex = 0; uiIndex<fileLength; ++uiIndex)
			{
				if (!feof(fp))
				{
					buffer[1 + uiIndex] = fgetc(fp);
				}
			}
			writeoffset += fileLength;
			bytessend = fileLength + 1;
			fileLength = 0;
		}
		//SleepMS(SEND_COMMAND_DELAY);
		if(Sema_LiteBMCSendCommand(buffer, bytessend)< 0)
		{
			return EC_FAILED;
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b");
		fflush(stdout);
	}while(fileLength);

	printf("Updating...%3d%%\n", (int)((writeoffset*100.0) / update_filesize));
	printf("Flashing done!\n");
	SleepMS(SEND_EREASE_DELAY);
	address = SEMA_ADDRESSTO_RUN_BL;

	buffer[0] = SEMA_CMD_BL_RUN;
	buffer[1] = (char)(address >> 24);
	buffer[2] = (char)(address >> 16);
	buffer[3] = (char)(address >> 8);
	buffer[4] = (char)(address);

	iRetVal = Sema_LiteBMCSendPacket(buffer, 5, 1);
	/*if(iRetVal)
	  {
	  printf("\nBoot loader run command send failed!\n");
	  }*/

	return SEMA_SUCCESS;
}

/**
 ********************************************************************************************************************************
 *       \FUNCTION       int Sema_LiteBMCSendCommand(char *in_cBuffer, unsigned char in_ucBufLength)
 *   	\DESCRIPTION 	This function is used to send data to BMC 
 *
 ********************************************************************************************************************************
 */
int Sema_LiteBMCSendCommand(char *in_cBuffer, unsigned char in_ucBufLength)
{
	int result;
	char status;
	char temp[SMB_TRANS_BUFFER_LEN] = { 0 };

	if ((result = Sema_LiteBMCSendPacket(in_cBuffer, in_ucBufLength, 1)) != SEMA_SUCCESS)
	{
		return result;
	}

	status = SEMA_CMD_BL_GET_STATUS;

	if ((result = Sema_LiteBMCSendPacket(&status, 1, 1)) != SEMA_SUCCESS)
	{
		return result;
	}

	in_ucBufLength = sizeof(status);
	if ((result = Sema_LiteBMCGetPacket(temp, &in_ucBufLength)) != SEMA_SUCCESS)
	{
		// printf("\nFailed to get status packet!\n");
		return result;
	}

	return SEMA_SUCCESS;
}

/**
 **************************************************************************************************************************************************
 *       \FUNCTION       int Sema_LiteBMCEnterBootLoader(void)
 *   	\DESCRIPTION 	This function is used to leave application section and enter into boot loader section.
 *
 **************************************************************************************************************************************************
 **/
int Sema_LiteBMCEnterBootLoader(void)
{
	char buffer[SMB_TRANS_BUFFER_LEN] = {0};
	int temp;
	char size = 1;
	unsigned int update_filesize = GetUpdateFileSize();

	temp = Sema_SendSlaveCommand(SEMA_SLAVE_ADDR, SEMA_CMD_LEAVEAPP, 1, buffer);     // 1. Leave application (->return to boot loader)
#ifndef __x86__ /* for x86 board need to more delay than x86 after issue of Leave application command */
	SleepMS(400);
#else	
	SleepMS(600);
#endif	

	buffer[0] = SEMA_CMD_BL_GET_STATUS;
	if(Sema_LiteBMCSendPacket(buffer, 1, 1)<0)
	{
		return SEMA_FAILURE;
	}

	if(Sema_LiteBMCGetPacket(buffer, &size) <0)
	{
		return SEMA_FAILURE;
	}

	return SEMA_SUCCESS;
}

/**
 **************************************************************************************************************************************************
 *       \FUNCTION       SendCommand(unsigned char in_ucAddress, unsigned char in_ucCommand, unsigned char in_ucBufLength, char* in_cBuffer)
 *   	\DESCRIPTION 	This function is used to frame the packet and send to BMC with i2c.
 *
 **************************************************************************************************************************************************
 **/
int Sema_SendSlaveCommand(unsigned char in_ucAddress, unsigned char in_ucCommand, unsigned char in_ucBufLength, char* in_cBuffer)
{
	struct i2c_msg  msgs[2];
	struct i2c_rdwr_ioctl_data msgset;
	char   recv_buf[MAX_RECV_BYTES];
	int  ret;
       	int iReadCnt = 0;	
	int iIndex = 0;
	struct i2c_smbus_ioctl_data smbus_data;
        union  i2c_smbus_data       data;

	if(g_iDevHandle == -1)
	{
		return 0;
	}

	if(in_ucBufLength > 32)
	{
		in_ucBufLength = 32;
	}

	if(in_ucAddress & 0x01)
	{
		/* For ARM with I2C transactions tested */
#ifndef __x86__
		msgs[0].addr  = in_ucAddress >> 1;           // Mask RW-Bit
		msgs[0].flags = 0;                                      // Direction: write
		msgs[0].buf   = (char *)&in_ucCommand;
		msgs[0].len   = 1;
		msgs[1].addr  = in_ucAddress >> 1;
		msgs[1].flags = I2C_M_RD ;               // Set READ flag
		msgs[1].buf   = recv_buf;
		msgs[1].len   = MAX_RECV_BYTES - 1;      //  in_ucBufLength value with +1 for length byte

		msgset.msgs  = msgs;
		msgset.nmsgs = 2;

		SleepMS(1);
		ret = ioctl(g_iDevHandle, I2C_RDWR, &msgset);
		if((signed)ret == -1)
		{			
			return 0;
		}
		memcpy(in_cBuffer, recv_buf+1, recv_buf[0]);
		iReadCnt = (int)(recv_buf[0] & 0xFF);
#else
		/* For X86 with SMBUS transactions tested */
                smbus_data.data = &data;
                smbus_data.command = in_ucCommand;
                ret = ioctl(g_iDevHandle, I2C_SLAVE, in_ucAddress >> 1); // Mask RW-Bit
                smbus_data.read_write = I2C_SMBUS_READ; // Direction: read
                smbus_data.size = I2C_SMBUS_BLOCK_DATA;
		SleepMS(1);
                ret = ioctl(g_iDevHandle, I2C_SMBUS, &smbus_data);
                if(ret < 0)
                {
                        return 0;
                }
		memcpy(in_cBuffer, &smbus_data.data->block[1], smbus_data.data->block[0]);
		iReadCnt = (int)(data.block[0] & 0xFF);
#endif
		return iReadCnt;	
	}
	else
	{
#ifndef __x86__
		/* For ARM with I2C transactions tested */
		memcpy(recv_buf+2, in_cBuffer, in_ucBufLength);
		recv_buf[0] = in_ucCommand;
		recv_buf[1] =  in_ucBufLength;
		msgs[0].addr  = in_ucAddress >> 1; // Mask RW-Bit
		msgs[0].flags = 0;
		msgs[0].buf   = recv_buf;
		msgs[0].len   =  in_ucBufLength+2 ;
		msgset.msgs  = msgs;
		msgset.nmsgs = 1;

		ret = ioctl(g_iDevHandle, I2C_RDWR, &msgset);
		if((signed)ret == -1)
		{
			return 0;
		}
		SleepMS(1);
#else
		/* For X86 with SMBUS transactions tested */
		smbus_data.read_write = I2C_SMBUS_WRITE;
                smbus_data.command = in_ucCommand;
                smbus_data.size = I2C_SMBUS_BLOCK_DATA;
		for (iIndex = 1; iIndex <= in_ucBufLength; iIndex++)
		{
        		data.block[iIndex] = in_cBuffer[iIndex - 1];
		}
		data.block[0] = in_ucBufLength;
		smbus_data.data = &data;
		
		ret = ioctl(g_iDevHandle, I2C_SMBUS, &smbus_data);
                if(ret < 0)
                {
                        return 0;
                }
                SleepMS(1);
#endif
		return  in_ucBufLength;
	}

}

/**
 **************************************************************************************************************************************************
 *       \FUNCTION       int Sema_LiteBMCSendPacket(char* in_cBuffer, unsigned char in_ucBufLength, unsigned char in_ucGetAck)
 *   	\DESCRIPTION 	This function is used to frame the
 *
 **************************************************************************************************************************************************
 **/
int Sema_LiteBMCSendPacket(char* in_cBuffer, unsigned char in_ucBufLength, unsigned char in_ucGetAck)
{

	char checksum = 0;
	char temp = 0;
	char temp2 = 0;
	char retry = 0;
	char acked[SMB_TRANS_BUFFER_LEN] = { 0 };

	unsigned int update_filesize = GetUpdateFileSize();

	checksum = Sema_UpdateLiteBMCChecksum(in_cBuffer, in_ucBufLength);
	temp = in_ucBufLength + 2;
	SleepMS(SEND_EREASE_DELAY);

	Sema_SendSlaveCommand(SEMA_SLAVE_ADDR, SEMA_CMD_BOOTSYNC, 1, &temp);

	SleepMS(SEND_EREASE_DELAY);
	Sema_SendSlaveCommand(SEMA_SLAVE_ADDR, SEMA_CMD_BOOTSYNC, 1, &checksum);

	SleepMS(SEND_EREASE_DELAY);
	temp = in_ucBufLength;
	temp2 = in_cBuffer[0];

	Sema_SendSlaveCommand(SEMA_SLAVE_ADDR, SEMA_CMD_BOOTSYNC, temp, in_cBuffer);

	SleepMS(SEND_EREASE_DELAY);
	if(!in_ucGetAck)
	{
		return SEMA_SUCCESS;
	}

	do
	{
		if(temp2 == SEMA_CMD_BL_DOWNLOAD)
		{
			SleepMS((update_filesize/0x400 + 1) * EREASE_DELAY);
		}
		/*Read data from BMC */
		if(Sema_SendSlaveCommand((SEMA_SLAVE_ADDR|0x1), 0xFF, 0, acked) == 0)
		{
			//printf("\n\t\t Sema address failed");
			return EC_FAILED;
		}
		SleepMS(SEND_EREASE_DELAY); // Some BMC need the delay
	}while((acked[1] & 0xFF) == 0);

	if((acked[1]&0xFF) != 0xcc)
	{
		//printf("\n\t\t 1. Sema address failed %X", acked[1]);
		return EC_FAILED;
	}

	return SEMA_SUCCESS;
}

/**
 **************************************************************************************************************************************************
 *       \FUNCTION       char Sema_UpdateLiteBMCChecksum(char* in_cBuffer, unsigned char in_ucBuffLen)
 *   	\DESCRIPTION 	This function is used to calculate checksum
 *
 **************************************************************************************************************************************************
 **/
char Sema_UpdateLiteBMCChecksum(char* in_cBuffer, unsigned char in_ucBuffLen)
{
	unsigned int uiIndex = 0;
	char chkSum = 0;
	for(uiIndex = 0; uiIndex< in_ucBuffLen; ++uiIndex)
	{
		chkSum += in_cBuffer[uiIndex];
	}	

	return chkSum;
}

int Sema_LiteBMCGetPacket(char* in_cBuffer, char *in_cBuffLength)
{
	char checksum[SMB_TRANS_BUFFER_LEN] = { 0 };
	char size[SMB_TRANS_BUFFER_LEN] = { 0 };
	char temp;
	do
	{
		/*read data from BMC */	
		if(Sema_SendSlaveCommand(SEMA_SLAVE_ADDR | 0x1, 0xFF, 2, size) == 0)
		{
			return EC_FAILED;
		}
	} while(size[0] == 0);

	/* Read Data from BMC */
	if(Sema_SendSlaveCommand(SEMA_SLAVE_ADDR | 0x1, 0xFF, 2, checksum) == 0)
	{
		return EC_FAILED;
	}
	*in_cBuffLength = size[0] - 2;

	/* Read Data from BMC */
	if(Sema_SendSlaveCommand(SEMA_SLAVE_ADDR | 0x1, 0xFF, 2, in_cBuffer) == 0)
	{
		*in_cBuffLength = 0;
		return EC_FAILED;
	}

	/* Validate checksum with existing Checksum in BMC */
	if (Sema_UpdateLiteBMCChecksum(in_cBuffer, *in_cBuffLength) != checksum[0])
	{
		*in_cBuffLength = 0;
		temp = 0x33;
		Sema_SendSlaveCommand(SEMA_SLAVE_ADDR, SEMA_CMD_BOOTSYNC, 1, &temp);
		return EC_FAILED;
	}

	temp = 0xcc;
	Sema_SendSlaveCommand(SEMA_SLAVE_ADDR, SEMA_CMD_BOOTSYNC, 1, &temp);

	return SEMA_SUCCESS;
}

int Sema_LiteBMCGetVersionFromFile(char *in_cFileName)
{
	int Index = 0, fd = 0;
	char buffer[BIN_FILE_READ_SECTOR], Bmc_name[BIN_FILE_BMC_VERSION] = {0};
	int iOffset = 0;
	
	fd = open(in_cFileName, O_RDONLY);
	if(fd < 0)
	{
		printf("File open error\n");
		return EC_FILE_NOT_FOUND;
	}

#if 1  
	while(read(fd, buffer, 512) > BIN_FILE_SEEK_SET)
	{
		lseek(fd, -BIN_FILE_SEEK_SET, SEEK_CUR);
		//iOffset += BIN_FILE_READ_SECTOR - BIN_FILE_SEEK_SET;
		iOffset += BIN_FILE_READ_SECTOR;
		for(Index = 0; Index < 512; Index++)
		{
			if(strncmp(&(buffer[Index]), "ADLINK", strlen("ADLINK")) == 0)
			{
				iOffset = iOffset - BIN_FILE_READ_SECTOR + Index;
				lseek(fd, 0, SEEK_END);
				break;
			}
		}
		iOffset -= BIN_FILE_SEEK_SET;
	}

	memset(buffer, 0, 512);
	lseek(fd, iOffset - BIN_FILE_BMC_VERSION, SEEK_SET);
	read(fd, buffer, 150);

	for(Index = 0; Index < 150; Index++)
	{
		if(strncmp(&(buffer[Index]), "BMC", strlen("BMC")) == 0)
		{
			break;
		}
	}
	close(fd);
	memcpy(Bmc_name, &buffer[Index], iOffset - (iOffset - Index));

	printf("Firmware version on your bin file: %s\n", Bmc_name);
#endif
	return 0;
}


int ValidateBinFile(char *in_cFileName, char *in_cBoardversion, unsigned char in_ucFile)
{
	int Index = 0, fd = 0;
	char cIsBin[10] = {0};
	char buffer[BIN_FILE_READ_SECTOR], Bmc_name[BIN_FILE_BMC_VERSION] = {0};
	int iOffset = 0;
	int iReadCnt = 0;
	unsigned char g_ucIsDataMatch = 0;
	unsigned char ucFileId = 0;
	unsigned char ucTargetId = 0;
	int iCnt = 0;
	char cModuleName[20] = {0};

	char brdStr[10];

	iCnt = strlen(in_cFileName);
	if(iCnt < 4)
	{
		iCnt = 4;
	}
	strcpy(cIsBin, &in_cFileName[iCnt - 4]);
	if(strncmp(cIsBin, ".bin", strlen(".bin")) != 0)
	{
		printf("\nInvalid firmware file extension, please provide firmware file with .bin extension.\n");
		return EC_FAILED;
	}

	fd = open(in_cFileName, O_RDONLY);
	if(fd < 0)
	{
		printf("\nFile open error\n");
		return EC_FILE_NOT_FOUND;
	}
	while(read(fd, buffer, 512) > BIN_FILE_SEEK_SET)
        {
                lseek(fd, -BIN_FILE_SEEK_SET, SEEK_CUR);
                iOffset += BIN_FILE_READ_SECTOR;
                for(Index = 0; Index < 512; Index++)
                {
                        if(strncmp(&(buffer[Index]), "ADLINK", strlen("ADLINK")) == 0)
                        {
				g_ucIsDataMatch = 1;
                                iOffset = iOffset - BIN_FILE_READ_SECTOR + Index;
                                lseek(fd, 0, SEEK_END);
                                break;
                        }
                }
                iOffset -= BIN_FILE_SEEK_SET;
        }

	if(!g_ucIsDataMatch)
	{
        	close(fd);
		printf("\nFirmware file is not valid. please provide valid firmware file to update BMC.\n");
		return EC_FAILED;
	}
	g_ucIsDataMatch = 0;

        memset(buffer, 0, 512);
        lseek(fd, iOffset - 64, SEEK_SET);
        read(fd, buffer, 150);

        for(Index = 0; Index < 150; Index++)
        {
                if(strncmp(&(buffer[Index]), "BMC", strlen("BMC")) == 0)
                {
			g_ucIsDataMatch = 1;
                        break;
                }
        }
	
	if(!g_ucIsDataMatch)
	{
        	close(fd);
		printf("\nFirmware file is not valid. please provide valid firmware file to update BMC.\n");
		return EC_FAILED;
	}
	
	/* to get version from file this validation not required */
	iCnt = 0;
	if(in_ucFile != 1)
	{
		int iLoop = Index + strlen("BMC ");
		while(buffer[iLoop] != ' ')
		{
			cModuleName[iCnt] = buffer[iLoop];
			iCnt++;
			iLoop++;
		}

		for(Index = 0; Index < 150; Index++)
		{
			if(strncmp(&(buffer[Index]), in_cBoardversion, strlen(in_cBoardversion)) == 0)
			{
				ucTargetId = 1;
				break;
			}
		}

		if(!ucTargetId)
		{
			close(fd);
			printf("\nFirmware file is not suitable for current BMC, Tool expecting %s. But User provided firmware file is for %s.\n", in_cBoardversion, cModuleName);
			return EC_FAILED;
		}
	}
        close(fd);
	return 0;	
}

int main(int argc, char *argv[])
{
	char *cFileName;
	char cBuffer[SMB_TRANS_BUFFER_LEN];
	char version1[BMC_V1_MAX_LEN] = {0};
	char version2[BMC_V2_MAX_LEN] = {0};
	char boardversion[BOARD_STR_MAX_LEN] = {0};
	unsigned char ucIsBoot = 0;
	unsigned int uiI2CDeviceNo = 0;
	int iHandle = 0;
	int iRetVal = 0;
	int iData = 0;
	int Index = 0;
	int iLoop = 0;
	int iCnt = 0;
	char cModuleName[BMC_V1_MAX_LEN] = {0};
	unsigned char ucMatch = 0;
	unsigned int uiBootDelay = 0;
	int iChoice = 0;

	char *AppInfo[] = {"-u", "-d" "-f", "-t"};

#if 1
	iHandle = OpenI2CDevice();
	if(iHandle < 0)
	{
		printf("\nInitialization error\n");
		return iHandle;
	}

	iChoice = PassArgs(argc, argv);
	switch(iChoice)
	{
		case 0:
			ShowHelp();
			break;
		case 1:
			ShowAboutApp();
			break;
		case 2: /* Get BMC version from target board */
			iRetVal = Sema_LiteBMCGetVersion(version1, SEMA_CMD_RD_VERSION1);
		        iRetVal = Sema_LiteBMCGetVersion(version2, SEMA_CMD_RD_VERSION2);
		        strcpy(boardversion, version1);
		        strcat(boardversion, version2);
			for(Index = 0; Index < BOARD_STR_MAX_LEN; Index++)
                	{
                        	if(strncmp(&(boardversion[Index]), "ADLINK", strlen("ADLINK")) == 0)
                        	{
					ucMatch = 1;
                                	break;
                        	}
                	}
			if(!ucMatch)
			{
				printf("Firmware is corrupted, please update the firmware in recovery mode.\n");
				break;
			}
		        printf("Firmware version on target device: %s\n", boardversion);
			break;
		case 3: 
			/* Get BMC version from file */
			cFileName = argv[3];
			g_ucIsFileSelect = 1;
			iRetVal = ValidateBinFile(cFileName, NULL, 1);
			if(!iRetVal)
			{
				iRetVal = Sema_LiteBMCGetVersionFromFile(cFileName);
			}
			break;
		case 4:
			if(!g_ucIsUpdateRecovery) /* for firmware updation in normal mode */
			{	
				iRetVal = Sema_LiteBMCGetVersion(version1, SEMA_CMD_RD_VERSION1);
				iRetVal = Sema_LiteBMCGetVersion(version2, SEMA_CMD_RD_VERSION2);
				strcpy(boardversion, version1);
				strcat(boardversion, version2);
				for(Index = 0; Index < BOARD_STR_MAX_LEN; Index++)
				{
					if(strncmp(&(boardversion[Index]), "BMC", strlen("BMC")) == 0)
					{
						ucMatch = 1;
						break;
					}
				}
				if(ucMatch)
				{
					iLoop = Index + strlen("BMC ");
					while(boardversion[iLoop] != ' ')
					{
						cModuleName[iCnt] = boardversion[iLoop];
						iCnt++;
						iLoop++;
					}
				}
				else
				{
					printf("Firmware is corrupted, please update the firmware in recovery mode.\n");
					break;
				}
			}
			
			printf("Firmware file validation is in progress...");
			fflush(stdout);
			cFileName = argv[2];
			iRetVal = ValidateBinFile(cFileName, cModuleName, g_ucIsUpdateRecovery);
			if(iRetVal)
			{
				break;
			}
			else
			{
				printf("\nFirmware file validation done successfully...");
			}
			printf("\nFirmware updating in %s mode.", g_ucIsUpdateRecovery ? "recovery" :"normal");
			printf("\nFirmware flashing is in progress, Please don't abort the application.");
			fflush(stdout);
			
			iRetVal = Sema_UpdateLiteBMC(cFileName);
			if(!iRetVal)
			{
				SleepMS(500);
				Sema_LiteBMCGetVersion(version1, SEMA_CMD_RD_VERSION1);
				Sema_LiteBMCGetVersion(version2, SEMA_CMD_RD_VERSION2);
				strcpy(boardversion, version1);
				strcat(boardversion, version2);	
		        	printf("Firmware version on target device: %s\n", boardversion);
				iRetVal = Sema_LiteBMCGetVersionFromFile(cFileName);
				printf("Updated successfully and please reboot the system\n");
			}
			else
			{
				printf("\nUpdation failed\n");
			}
			break;
		default:
			ShowHelp();
			break;
	}

	close(g_iDevHandle);
#endif	
	return 0;
}

/**
 **************************************************************************************************************************************************
 *       \FUNCTION      	int GetBMCVersion(char *in_cBuffer, unsigned int in_uiCommand)
 *   	\DESCRIPTION 	This function is used to get BMC version 
 *
 **************************************************************************************************************************************************
 **/
int Sema_LiteBMCGetVersion(char *in_cBuffer, unsigned int in_uiCommand)
{
	int iReadCnt = 0;
	int iRetVal = 0;
	SleepMS(10);
	iReadCnt = Sema_SendSlaveCommand(SEMA_SLAVE_ADDR|0x1, in_uiCommand, 0, in_cBuffer);
	if(iReadCnt > 0)
	{
		in_cBuffer[iReadCnt]= 0;
		iRetVal = SEMA_SUCCESS;
	}
	else
	{
		iRetVal = SEMA_FAILURE;
	}

	return iRetVal;
}


int PassArgs(int argc, char* argv[])
{
	int useroption = 0;
	int StrCnt = 0;
	int iTemp = 0;

	/* -u for update normal mode, -r -update recovery mode, -t -target version, -f -firmware file version */
	char *ucChoice[] = {"-u", "-r",  "-d", "-t", "-f"};
	switch(argc)
	{
		case 1:
			useroption = 1;
			break;
		case 2:
			useroption = 0;
			break;
		case 3: 
			StrCnt = strlen(argv[1]);
			if(StrCnt > 2)
			{
				useroption = 0;
				break;
			}
			iTemp = strcmp(ucChoice[0], argv[1]);
			
			if(iTemp)
			{
				iTemp = strcmp(ucChoice[1], argv[1]);
				if(!iTemp)
				{
					g_ucIsUpdateRecovery = 1;
					useroption = 4;
					break;
				}	

				StrCnt = strlen(argv[2]);
				if(StrCnt > 2)
				{
					useroption = 0;
					break;
				}
				if((!strcmp(ucChoice[2], argv[1])) && (!strcmp(ucChoice[3], argv[2])))
				{
					useroption = 2; /*display firmware version of target device */
					break;
				}
				else
				{
					useroption = 0;
					break;
				}
			}
			g_ucIsUpdateRecovery = 0;
			useroption = 4; /* update the firmware version */
			break;
		case 4:
			StrCnt = strlen(argv[1]);
                        if(StrCnt > 2)
                        {
                                useroption = 0;
                                break;
                        }
			StrCnt = strlen(argv[2]);
                        if(StrCnt > 2)
                        {
                                useroption = 0;
                                break;
                        }
			
			if((!strcmp(ucChoice[2], argv[1])) && (!strcmp(ucChoice[4], argv[2])))
			{
				useroption = 3; /* disaply firmware version from file */
				break;
			}
			else
			{
				useroption = 0;
				break;
			}
		break;
		default:
			useroption = 0;
			break;
	}

	return useroption;
}
void ShowHelp(void)
{
        printf("Usage:\n");
        printf("\t-h Display this screen\n\n");
        printf("\t-u Update BMC firmware in normal mode\n");
        printf("\t  -u <filename>\n");
        printf("\t\t<filename>\tfull path of firmware image file\n\n");

        printf("\t-r Update BMC firmware in recovery mode\n");
        printf("\t  -r <filename>\n");
        printf("\t\t<filename>\tfull path of firmware image file\n\n");

        printf("\t-d Display firmware version\n");
        printf("\t  -d <-t/-f> <filename>\n");
        printf("\t\t-t\t\ttarget device             \n");
        printf("\t\t-f\t\tfirmware file                 \n");
        printf("\t\t<filename>\tfull path of firmware image file\n");

}

void ShowAboutApp(void)
{
#ifndef __x86__
	printf( "Function : Lite BMC Firmware Flash Utility on ADLINK ARM platform\n");
#else
	printf( "Function : Lite BMC Firmware Flash Utility on ADLINK x86-64 platform\n");
#endif	

	printf( "Version : %s.%s\n", LITEBMC_MAJOR_VER, LITEBMC_MINOR_VER);

	printf( "Usage :\n");
	printf( "1. Update Firmware in normal mode: ad-litbmc-fwupd -u filename.bin\n");
	printf( "2. Update Firmware in recovery mode: ad-litbmc-fwupd -r filename.bin\n");
	printf( "3. Display Firmware version on Target device : ad-litbmc-fwupd -d -t\n");
	printf( "4. Display Firmware version in bin file : ad-litbmc-fwupd -d -f filename.bin\n");
	printf( "Options :\n");
        printf( "  -u start to update the firmware in normal mode\n");
        printf( "  -r start to update the firmware in recovery mode\n");
        printf( "  -d -t Display the firmware version on your target device or module\n");
        printf( "  -d -f Display the firmware version on your bin file\n");
	printf( "  -h|? Display command line help information\n");
}

