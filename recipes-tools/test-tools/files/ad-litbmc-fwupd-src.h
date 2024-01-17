#ifndef SEMA_UPDATE_H 
#define SEMA_UPDATE_H


#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#define __x86__
#else
#undef __x86__
#endif

#define LITEBMC_MAJOR_VER  "1"
#define LITEBMC_MINOR_VER  "01"

#define BIN_FILE_READ_SECTOR	512
#define BIN_FILE_SEEK_SET	64
#define BIN_FILE_BMC_VERSION	64


#define SEMA_ADDRESSTO_DOWNLOAD_BL		0x00002000			/* please change this carefully if needed */
#define SEMA_ADDRESSTO_RUN_BL			0x00002004			/* please change this carefully if needed */

#define SEMA_CMD_BOOTSYNC               0x21            ///< Synchronize to bootloader
#define SEMA_CMD_LEAVEAPP               0x51            ///< Leave application (enter bootloader)
#define SEMA_CMD_SENDDATA               0x25            ///< Send data (bootloader and firmware update)
#define SEMA_CMD_BL_GET_STATUS     		0x23
#define SEMA_CMD_RD_VERSION1            0x30            ///< Read version string 1
#define SEMA_CMD_RD_VERSION2            0x31            ///< Read version string 2
#define SEMA_CMD_BL_DOWNLOAD      		0x21
#define SEMA_CMD_BL_SEND_DATA      		0x24
#define SEMA_CMD_BL_RUN            		0x22
#define SEMA_CMD_CAPABILITY				0x2F


#define SMB_TRANS_BUFFER_LEN            33

#define EREASE_DELAY    100
#define SEND_EREASE_DELAY    5

#define SEMA_FAILURE		-1
#define SEMA_SUCCESS		0

#define EC_FAILED		-10
#define EC_FILE_NOT_FOUND       -301
#define EC_FILE_ACCESS_ERROR    -302




#define MAX_RECV_BYTES  	0x22  // Max length 32Bytes + 1 Byte Length +1 Byte cmd = 34Bytes

#define SEMA_SLAVE_ADDR		0x50
#define BMC_V1_MAX_LEN		66
#define BMC_V2_MAX_LEN 		33
#define BOARD_STR_MAX_LEN 	100

#define SleepMS(msecs)  usleep(1000*msecs)

#define SEMA_VERSION_MAJOR      2
#define SEMA_VERSION_MINOR      5
#define SEMA_VERSION_ADDON      "R4"
#define SEMA_VENDOR             "LiPPERT ADLINK Technology GmbH, Germany"
#define SEMA_COPYRIGHT_YEAR     "(c)2009-2021" 
#define SEND_COMMAND_DELAY	150




int OpenI2CDevice(void);
int Sema_LiteBMCGetVersionFromFile(char* in_cFileName);
int Sema_LiteBMCGetVersion(char *in_cBuffer, unsigned int in_uiCommand);
int Sema_UpdateLiteBMC(char* in_cFileName);
int Sema_LiteBMCEnterBootLoader(void);
int Sema_LiteBMCSendCommand(char *in_cBuffer, unsigned char in_ucBufLength);
int Sema_LiteBMCSendPacket(char* in_cBuffer, unsigned char in_ucBufLength, unsigned char in_ucGetAck);
int Sema_LiteBMCGetPacket(char* in_cBuffer, char *in_cBuffLength);
char Sema_UpdateLiteBMCChecksum(char* in_cBUffer, unsigned char in_ucBuffLen);
int Sema_SendSlaveCommand(unsigned char in_ucAddress, unsigned char in_ucCommand, unsigned char in_ucBufLength, char* in_cBuffer);

int ValidateBinFile(char *in_cFileName, char *in_cBoardVersion, unsigned char);
int PassArgs(int argc, char* argv[]);
void ShowAboutApp(void);
void ShowHelp(void);


void Linux_SMBus_SMITemp_Start( void );
#define PortTalk_InByte(PortAddress)                    inb((unsigned short)(PortAddress))
#define PortTalk_OutByte(PortAddress, Value)    outb((unsigned char )(Value), (unsigned short)(PortAddress))
#define PortTalk_InDWord(PortAddress)                   inl((unsigned int )(PortAddress))
#define PortTalk_OutDWord(PortAddress, Value)   outl((unsigned long)Value, (unsigned short)(PortAddress))

#endif

