#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QTime>
#include "ams_radon_reader.h"
#include "uart.h"

#define SET_MESSAGE_TYPE(buf, value) buf[0] = value 
#define SET_MESSAGE_LENGTH(buf, value) do { buf[2] = value & 0xFF; buf[1] = (value >> 8) & 0xFF; } while ( 0 )
#define SET_MESSAGE_CRC(buf, value) do { buf[4] = value & 0xFF; buf[3] = (value >> 8) & 0xFF; } while ( 0 )
#define SET_MESSAGE_STATUS(buf, value) buf[5] = value
#define SET_MESSAGE_PAYLOAD(buf, index, value) do { buf[index] = value; } while ( 0 )
#define GET_MESSAGE_TYPE(buf) buf[0]
#define GET_MESSAGE_LENGTH(buf) buf[1] << 8 | buf[2] 
#define GET_MESSAGE_CRC(buf) buf[3] << 8 | buf[4] 
#define GET_MESSAGE_STATUS(buf) buf[5]
#define GET_MESSAGE_PAYLOAD(buf, index) buf[index]

#define ERR_NONE 	 0
#define ERR_NOMEM 	-1
#define ERR_BUSY 		-2
#define ERR_IO 		-3
#define ERR_TIMEOUT	-4
#define ERR_REQUEST	-5
#define ERR_NOMSG	-6
#define ERR_PARAM	-7
#define ERR_PROTO	-8
#define CRC_ERROR       -33
#define TIMEOUT_ERROR	-34
#define CRC16_PRELOAD 0xFFFF

unsigned short crc16OffsetTable[256] = {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

#define COM_CTRL_CMD_RESET		  		0x65
#define COM_CTRL_CMD_FW_INFORMATION		0x66
#define COM_CTRL_CMD_FW_NUMBER			0x67
#define COM_CTRL_CMD_ENTER_BOOTLOADER 	0x6B
#define COM_WRITE_REG				 	0x68
#define COM_READ_REG				 	0x69
#define CMD_READER_CONFIG               0
#define CMD_ANTENNA_POWER               1
#define CMD_CHANGE_FREQ                 2
#define CMD_GEN2_SETTINGS               3
#define CMD_CONFIG_TX_RX                4
#define CMD_INVENTORY_GEN2              5
#define CMD_SELECT_TAG                  6
#define CMD_WRITE_TO_TAG                7
#define CMD_READ_FROM_TAG               8
#define CMD_LOCK_UNLOCK_TAG             9
#define CMD_KILL_TAG                    10
#define CMD_GET_TAG_DATA	            11
#define CMD_START_STOP                  12
#define CMD_TUNER_TABLE                 13
#define CMD_AUTO_TUNER                  14
#define CMD_ANTENNA_TUNER               15
#define CMD_INVENTORY_6B                17
#define CMD_READ_FROM_TAG_6B            18
#define CMD_WRITE_TO_TAG_6B             19
#define CMD_GENERIC_CMD_ID              20
#define CMD_RSSI_MEAS_CMD_ID            21
#define CMD_READER_CONFIG_RESP          22
#define CMD_ANTENNA_POWER_RESP        	23
#define CMD_CHANGE_FREQ_RESP            24
#define CMD_GEN2_SETTINGS_RESP          25
#define CMD_CONFIG_TX_RX_RESP           26
#define CMD_INVENTORY_GEN2_RESP         27
#define CMD_SELECT_TAG_RESP             28
#define CMD_WRITE_TO_TAG_RESP           29
#define CMD_READ_FROM_TAG_RESP          30
#define CMD_LOCK_UNLOCK_TAG_RESP     	31
#define CMD_KILL_TAG_RESP               32
#define CMD_START_STOP_RESP             33
#define CMD_TUNER_TABLE_RESP            34
#define CMD_AUTO_TUNER_RESP             35
#define CMD_ANTENNA_TUNER_RESP        	36
#define CMD_INVENTORY_6B_RESP           37
#define CMD_READ_FROM_TAG_6B_RESP   	38
#define CMD_WRITE_TO_TAG_6B_RESP      	39
#define CMD_GENERIC_CMD_ID_RESP        	40
#define CMD_RSSI_MEAS_CMD_ID_RESP     	41
#define CMD_GET_TAG_DATA_RESP     		42
#define COM_CTRL_CMD_FW_NUMBER_RESP		43
#define COM_CTRL_CMD_RESET_RESP		  	44
#define COM_CTRL_CMD_FW_INFORMATION_RESP	45
#define COM_CTRL_CMD_ENTER_BOOTLOADER_RESP 	46
#define COM_WRITE_REG_RESP				 	47
#define COM_READ_REG_RESP				 	48
#define WAIT_FOR_RESPONSE_TIME 				25
#define WAIT_FOR_INVENTORY_RESPONSE_TIME 	300
#define WAIT_FOR_TAG_DATA_RESPONSE_TIME 	300
#define WAIT_FOR_AUTOTUNE_2_RESPONSE_TIME 	6000
#define WAIT_FOR_AUTOTUNE_1_RESPONSE_TIME 	300
#define WRITE_TO_TAG_WAIT_FOR_RESPONSE_TIME 400

AMSRadonReader::AMSRadonReader(string uartFileName)
{
	uart = new UART(uartFileName);	
}
short AMSRadonReader::initialize()
{
	return uart->initialize();
}
short AMSRadonReader::resetPIC()
{
	char cmdMsgBuffer[7];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, COM_CTRL_CMD_RESET);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 1);  //reset pic  = 1, reset as3993 = 2;
	unsigned short crc = calculateCRC(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 7);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::resetAS3993()
{
	char cmdMsgBuffer[7];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, COM_CTRL_CMD_RESET);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 2);  //reset pic  = 1, reset as3993 = 2;
	unsigned short crc = calculateCRC(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 7);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::enterBootloader()
{
	char cmdMsgBuffer[6];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, COM_CTRL_CMD_ENTER_BOOTLOADER);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 6);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 6);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 6);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getFirmwareVersion(int &firmwareVersion)
{
	char cmdMsgBuffer[6];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, COM_CTRL_CMD_FW_NUMBER);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 6);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 6);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 6);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	firmwareVersion = respMsgBuffer[6] << 16 | respMsgBuffer[7] << 8 | respMsgBuffer[8];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getFirmwareInformation(string &firmwareInfo)
{
	char cmdMsgBuffer[6];
	char respMsgBuffer[100];
	SET_MESSAGE_TYPE(cmdMsgBuffer, COM_CTRL_CMD_FW_INFORMATION);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 6);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 6);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 6);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	firmwareInfo = &respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::writeToAS3993Reg(char regAddr, char regValue, char &status)
{
	char cmdMsgBuffer[8];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, COM_WRITE_REG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, regAddr);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, regValue);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 8);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	status = respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::readAS3993Reg(char regAddr, char *regValue)
{
	char cmdMsgBuffer[8];
	char respMsgBuffer[75];
	SET_MESSAGE_TYPE(cmdMsgBuffer, COM_READ_REG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, regAddr);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 8);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	*regValue = respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::readAllAS3993Regs(char *regValues)
{
	char cmdMsgBuffer[8];
	char respMsgBuffer[75];
	SET_MESSAGE_TYPE(cmdMsgBuffer, COM_READ_REG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 8);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	for(short i = 0; i < count - 6; i++)
	{
		regValues[i] = respMsgBuffer[6 + i];
	}
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setReaderConfiguration(char powerMode, char *readerConfig)
{
	char cmdMsgBuffer[8];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_READER_CONFIG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, powerMode);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 8);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	for(short i = 0; i < count - 6; i++)
	{
		readerConfig[i] = respMsgBuffer[6 + i];
	}
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getReaderConfiguration(char *readerConfig)
{
	char cmdMsgBuffer[8];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_READER_CONFIG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 8);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	for(short i = 0; i < count - 6; i++)
	{
		readerConfig[i] = respMsgBuffer[6 + i];
	}
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::antennaPower(char state, char &status)
{
	char cmdMsgBuffer[8];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_ANTENNA_POWER);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, state);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 8);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	status = respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getReflectedPowerLevel(int freq, char tunerSettings, char &IChannel, char &QChannel)
{
	// tunerSettings = 1: apply tuning settings from table
	// tunerSettings = 0: don't
	char cmdMsgBuffer[11];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CHANGE_FREQ);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 11);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x02);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, freq & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, freq >> 8 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, freq >> 16 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, tunerSettings);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 11);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 11);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	IChannel = respMsgBuffer[6];
	QChannel = respMsgBuffer[7];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::addHoppingFreq(int freq, char clearList, char profileID, char &status)
{
	char cmdMsgBuffer[12];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CHANGE_FREQ);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x04);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, freq & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, freq >> 8 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, freq >> 16 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, clearList);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, profileID);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 12);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	status = respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getFreqListParams(char &profileID, int &minFreq, int &maxFreq, char &currNumFreqs, char &hostNumFreqs)
{
	char cmdMsgBuffer[7];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CHANGE_FREQ);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x05);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 7);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	profileID = respMsgBuffer[6];
	minFreq = 0;
	minFreq += (int)respMsgBuffer[7];
	minFreq += (int)respMsgBuffer[8] << 8;
	minFreq += (int)respMsgBuffer[9] << 16;
	maxFreq = 0;
	maxFreq += (int)respMsgBuffer[10];
	maxFreq += (int)respMsgBuffer[11] << 8;
	maxFreq += (int)respMsgBuffer[12] << 16;
	currNumFreqs = respMsgBuffer[13];
	hostNumFreqs = respMsgBuffer[14];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setFreqHoppingParams(short listeningTime, short maxSendingTime, short idleTime, signed char  rssi, char &status)
{
	char cmdMsgBuffer[14];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CHANGE_FREQ);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 14);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x08);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, listeningTime & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, listeningTime >> 8 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, maxSendingTime & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, maxSendingTime >> 8 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, idleTime & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, idleTime >> 8 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, rssi);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 14);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 14);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	status = respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getFreqHoppingParams(short &listeningTime, short &maxSendingTime, short &idleTime)
{
	char cmdMsgBuffer[7];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CHANGE_FREQ);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x09);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 7);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	listeningTime = 0;
	listeningTime += (int)respMsgBuffer[6];
	listeningTime += (int)respMsgBuffer[7] << 8;
	maxSendingTime = 0;
	maxSendingTime += (int)respMsgBuffer[8];
	maxSendingTime += (int)respMsgBuffer[9] << 8;
	idleTime = 0;
	idleTime += (int)respMsgBuffer[10];
	idleTime += (int)respMsgBuffer[11] << 8;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::performContinuousModulationTest(int freq, short duration, char random, char *randomData)
{
	char cmdMsgBuffer[23];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CHANGE_FREQ);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 23);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x10);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, freq & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, freq >> 8 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, freq >> 16 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, duration & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, duration >> 8 & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, random);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, randomData[0]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, randomData[1]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, randomData[2]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, randomData[3]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, randomData[4]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, randomData[5]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, randomData[6]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, randomData[7]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, randomData[8]);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 22, randomData[9]);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 23);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 23);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setLinkFrequency(char linkFreq, char &storedLinkFreq)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, linkFreq);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedLinkFreq = respMsgBuffer[7];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setCoding(char coding, char &storedCoding)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, coding);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedCoding = respMsgBuffer[9];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setSession(char session, char &storedSession)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, session);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedSession = respMsgBuffer[11];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setTrext(char trext,char &storedTrext)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, trext);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedTrext = respMsgBuffer[13];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setTari(char tari, char &storedTari)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, tari);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedTari = respMsgBuffer[15];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setQBegin(char qBegin, char &storedQBegin)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, qBegin);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedQBegin = respMsgBuffer[17];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setSel(char set, char &storedSet)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, set);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedSet = respMsgBuffer[19];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setTarget(char target, char &storedTarget)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, target);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedTarget = respMsgBuffer[21];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getGen2Settings(char &linkFreq, char &coding, char &session, char &trext, char &tari, char &qBegin, char &set, char &target)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GEN2_SETTINGS);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	linkFreq = respMsgBuffer[7];
	coding = respMsgBuffer[9]; 
	session = respMsgBuffer[11]; 
	trext = respMsgBuffer[13]; 
	tari = respMsgBuffer[15]; 
	qBegin = respMsgBuffer[17]; 
	set = respMsgBuffer[19]; 
	target = respMsgBuffer[21]; 
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setAntennaSensitivity(signed char sensitivity, signed char &storedSensitivity)
{
	char cmdMsgBuffer[10];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CONFIG_TX_RX);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 10);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, sensitivity);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 10);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 10);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedSensitivity = respMsgBuffer[7];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getAntennaSensitivity(char &sensitivity)
{
	char cmdMsgBuffer[10];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CONFIG_TX_RX);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 10);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 10);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 10);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	sensitivity = respMsgBuffer[7];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setAntennaID(char antennaID, char &storedAntennaID)
{
	char cmdMsgBuffer[10];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CONFIG_TX_RX);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 10);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, antennaID);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 10);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 10);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedAntennaID = respMsgBuffer[9];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getAntennaID(char &antennaID)
{
	char cmdMsgBuffer[10];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_CONFIG_TX_RX);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 10);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 10);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 10);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	antennaID = respMsgBuffer[9];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::performGen2Inventory(char autoAck, char tidAndFast, char rssi)
{
	char cmdMsgBuffer[9];
	char respMsgBuffer[100];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_INVENTORY_GEN2);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 9);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, autoAck); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, tidAndFast); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, rssi); 
	unsigned short crc = calculateCRC(cmdMsgBuffer, 9);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 9);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_INVENTORY_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getTagData(vector<TagData> &tags, char &inventoryType, char &inventoryResult, char &numberOfTagsFound)
{
	char cmdMsgBuffer[6];
	char respMsgBuffer[1000];
	TagData tag;
	char tagNumber;
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GET_TAG_DATA);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 6);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 6);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 6);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_TAG_DATA_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	if(msgStatus != 0)
		return msgStatus;
	inventoryResult = respMsgBuffer[6];
	inventoryType = respMsgBuffer[7];
	if(inventoryResult == 0)
	{
		numberOfTagsFound = respMsgBuffer[8];
		tagNumber = numberOfTagsFound;
		unsigned short index = 9;
		while(tagNumber > 0)
		{
			tag.setReaderAGC(respMsgBuffer[index]);
			tag.setReaderRSSI(respMsgBuffer[++index]);
			tag.setCommFrequency(&respMsgBuffer[++index]);
			char EPCLen = respMsgBuffer[index += 3] - 2;
			tag.setPC(&respMsgBuffer[++index]);
			tag.setEPCAndEPCLength(&respMsgBuffer[index += 2], EPCLen);
			index += EPCLen;
			if(inventoryType & 0x02)
			{
				char TIDLen = respMsgBuffer[(int)index];
				tag.setTIDAndTIDLength(&respMsgBuffer[++index], TIDLen);
				index += TIDLen;
				tag.setTempCalibrationParams(&respMsgBuffer[(int)index]);
				index += 8;
			}
			if(inventoryType & 0x04)
			{
				tag.setMMS(&respMsgBuffer[(int)index]);
				tag.setVFC(&respMsgBuffer[index += 2]);
				tag.setTEMP(&respMsgBuffer[index += 2]);
				index += 2;
			}
			tags.push_back(tag);
			tag.clear();
			tagNumber--;
		}
	}
	return msgStatus;
}
short AMSRadonReader::clearListOfSelectCommands()
{
	char cmdMsgBuffer[8];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_SELECT_TAG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 8);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 8);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::singulateATag(char clear, char target, char action, char memBank, short address, char maskLen, char truncate, char *mask, short maskSize)
{
	short bufferLen = 6 + 8 + maskSize;
	char *cmdMsgBuffer = new char[bufferLen];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_SELECT_TAG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, bufferLen);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, clear);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, target);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, action);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, memBank);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, address & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, (address >>8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, maskLen);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, truncate);
	for(short i = 0; i < maskSize; i++)
		cmdMsgBuffer[14 + i] = mask[i];
	unsigned short crc = calculateCRC(cmdMsgBuffer, bufferLen);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], bufferLen);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::writeToTag(char memBank, int address, int accessPW, char *data, short dataLen, short &numOfWordsWritten, char &tagErrorCode)
{
	short bufferLen = 6 + 9 + dataLen;
	char *cmdMsgBuffer = new char[bufferLen];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_WRITE_TO_TAG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, bufferLen);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, memBank);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, address & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, (address >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, (address >> 16) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, (address >> 24) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, accessPW & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, (accessPW >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, (accessPW >> 16) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, (accessPW >> 24) & 0xFF);
	for(short i = 0; i < dataLen; i++)
		cmdMsgBuffer[15 + i] = data[i];
	unsigned short crc = calculateCRC(cmdMsgBuffer, bufferLen);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], bufferLen);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WRITE_TO_TAG_WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	numOfWordsWritten = respMsgBuffer[6];
	tagErrorCode = respMsgBuffer[7];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::readFromTag(char memBank, int address, int password, char *data, char &dataLen)
{
	char cmdMsgBuffer[16];
	char respMsgBuffer[100];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_READ_FROM_TAG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 16);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, memBank);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, address & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, (address >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, (address >> 16) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, (address >> 24) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, password & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, (password >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, (password >> 16) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, (password >> 24) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, dataLen);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 16);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 16);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	dataLen = msgLength - 6;
	for(short i = 0; i < dataLen; i++)
	{
		data[i] = respMsgBuffer[6 + i];
	}
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::lockUnlockTag(int maskAndAction, int accessPassword, char &tagCode)
{
	char cmdMsgBuffer[12];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_LOCK_UNLOCK_TAG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, maskAndAction & 0xFF); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, (maskAndAction >> 8) & 0xFF); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, (maskAndAction >> 16) & 0xFF); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, accessPassword & 0xFF); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, (accessPassword >> 8) & 0xFF); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, (accessPassword >> 16) & 0xFF); 
	unsigned short crc = calculateCRC(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 12);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	tagCode = respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::killTag(int killPassword, char recom, char &status)
{
	char cmdMsgBuffer[11];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_KILL_TAG);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 11);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, killPassword & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, (killPassword >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, (killPassword >> 16) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, (killPassword >> 24) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, recom);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 11);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 11);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	status = respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::startStop(char update, char start, char autoAck, char tidAndFast, char rssi, char &currStartValue)
{
	char cmdMsgBuffer[11];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_START_STOP);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 11);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, update);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, start);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, autoAck);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, tidAndFast);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, rssi);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 11);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 11);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	currStartValue = respMsgBuffer[6];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getCurrentTuningTableSize(char &maxTuningTableSizeSupported, char &currTuningTableSize)
{
	char cmdMsgBuffer[7];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_TUNER_TABLE);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00); 
	unsigned short crc = calculateCRC(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 7);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	maxTuningTableSizeSupported = respMsgBuffer[7];
	currTuningTableSize = respMsgBuffer[8];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::deleteCurrentTuningTable(char &maxTuningTableSizeSupported)
{
	char cmdMsgBuffer[7];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_TUNER_TABLE);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x01); 
	unsigned short crc = calculateCRC(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 7);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	maxTuningTableSizeSupported = respMsgBuffer[7];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::addToTuningTable(int freq, 
		char ant1TuneEnable, 
		char ant1Cin, 
		char ant1Clen, 
		char ant1Cout, 
		short ant1I_Q, 
		char ant2TuneEnable,
		char ant2Cin,
		char ant2Clen,
		char ant2Cout,
		short ant2I_Q,
		char &remainingSizeInTuningTable)
{
	char cmdMsgBuffer[22];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_TUNER_TABLE);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x02); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, freq & 0xFF); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, (freq >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, (freq >> 16) & 0xFF); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, ant1TuneEnable); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, ant1Cin);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, ant1Clen); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, ant1Cout); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, ant1I_Q & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 15, (ant1I_Q >> 8) & 0xFF); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 16, ant2TuneEnable); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 17, ant2Cin);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 18, ant2Clen); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 19, ant2Cout); 
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 20, ant2I_Q & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 21, (ant2I_Q >> 8) & 0xFF);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 22);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 22);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	remainingSizeInTuningTable = respMsgBuffer[7];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::performAutoTuning(char autoTune)
{
	char cmdMsgBuffer[7];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_AUTO_TUNER);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, autoTune);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 7);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 7);
	short waitTimeForResponse;
	if(autoTune == 0x02)
		waitTimeForResponse = WAIT_FOR_AUTOTUNE_2_RESPONSE_TIME;  //wait more than 3 seconds for tuning to finish
	else
		waitTimeForResponse = WAIT_FOR_AUTOTUNE_1_RESPONSE_TIME;
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < waitTimeForResponse));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setAntennaCin(char cin, char &storedCin)
{
	char cmdMsgBuffer[12];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_ANTENNA_TUNER);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, cin);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 12);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedCin = respMsgBuffer[7];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setAntennaClen(char clen, char &storedClen)
{
	char cmdMsgBuffer[12];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_ANTENNA_TUNER);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, clen);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 12);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedClen = respMsgBuffer[9];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::setAntennaCout(char cout, char &storedCout)
{
	char cmdMsgBuffer[12];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_ANTENNA_TUNER);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x01);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, cout);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 12);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	storedCout = respMsgBuffer[11];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::getAntennaTunerParams(char &cin, char &clen, char &cout)
{
	char cmdMsgBuffer[12];
	char respMsgBuffer[25];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_ANTENNA_TUNER);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, 0x00);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, 0x00);
	unsigned short crc = calculateCRC(cmdMsgBuffer, 12);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], 12);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	cin = respMsgBuffer[7];
	clen = respMsgBuffer[9];
	cout = respMsgBuffer[11];
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
short AMSRadonReader::sendCommand(int password, 
		short lengthTransmitData, 
		short lengthRecieveData, 
		char directCommand, 
		char noResponseTime, 
		char *transmitData,
		char &status,
		char &dataLength,
		char *receivedData)
{
	short bufferLen = 6 + 9 + lengthTransmitData;
	char *cmdMsgBuffer = new char[bufferLen];
	char respMsgBuffer[100];
	SET_MESSAGE_TYPE(cmdMsgBuffer, CMD_GENERIC_CMD_ID);
	SET_MESSAGE_LENGTH(cmdMsgBuffer, bufferLen);
	SET_MESSAGE_CRC(cmdMsgBuffer, 0);
	SET_MESSAGE_STATUS(cmdMsgBuffer, 0);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 6, password & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 7, (password >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 8, (password >> 16) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 9, lengthTransmitData & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 10, (lengthTransmitData >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 11, lengthRecieveData & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 12, (lengthRecieveData >> 8) & 0xFF);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 13, directCommand);
	SET_MESSAGE_PAYLOAD(cmdMsgBuffer, 14, noResponseTime);
	unsigned short crc = calculateCRC(cmdMsgBuffer, bufferLen);
	SET_MESSAGE_CRC(cmdMsgBuffer, crc);
	for(short i = 0; i < lengthTransmitData; i++)
		cmdMsgBuffer[15 + i] = transmitData[i];
	uart->flush();
	uart->sendMessage(&cmdMsgBuffer[0], bufferLen);
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&respMsgBuffer[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(respMsgBuffer);		
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	unsigned short msgCRC  = GET_MESSAGE_CRC(respMsgBuffer);
	SET_MESSAGE_CRC(respMsgBuffer, 0);
	unsigned short calculatedCRC = calculateCRC(respMsgBuffer, msgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	status = respMsgBuffer[6];
	dataLength = respMsgBuffer[7];
	for(short i = 0; i < dataLength; i++)
	{
		receivedData[i] = respMsgBuffer[8 + i];
	}
	char msgStatus  = GET_MESSAGE_STATUS(respMsgBuffer);
	return msgStatus;
}
unsigned short AMSRadonReader::calculateCRC(const void *buf, unsigned short len)
{
	short counter;
	unsigned short crc = CRC16_PRELOAD;
	signed char *sbuf = (signed char *)buf;
	for (counter = 0; counter < len; counter++)
		crc = (crc << 8) ^ crc16OffsetTable[((crc >> 8) ^ *sbuf++) & 0x00FF];
	return crc;
}
TagData::TagData()
{
	clear();	
}
void TagData::clear()
{
	readerAGC = 0;
	readerRSSI = 0;
	commFrequency = 0;
	EPCLength = 0;
	EPC = "";
	PC = 0;
	TIDLength = 0;
	TID = "";
	tempCalibrationParams = "";
	MMS = 0;
	VFC = 0;
	TEMP = 0;
}
void TagData::setReaderAGC(char readerAGC)
{
	this->readerAGC = readerAGC;
}
void TagData::setReaderRSSI(char readerRSSI)
{
	this->readerRSSI = readerRSSI;	
}
void TagData::setCommFrequency(char *commFrequency)
{
	this->commFrequency = 0;
	this->commFrequency += commFrequency[0];
	this->commFrequency += commFrequency[1] << 8;
	this->commFrequency += commFrequency[2] << 16;
}
void TagData::setEPCAndEPCLength(char *EPC, char EPCLength)
{
	this->EPCLength = EPCLength;
	if(EPCLength > 0)
	{
		this->EPC  = "0x";
		char EPCChar[3];
		for(int c = 0; c < EPCLength; c++)
		{
			sprintf(EPCChar, "%02x", EPC[c]);
			this->EPC += EPCChar;
		}
	}
}
void TagData::setPC(char *PC)
{
	this->PC = 0;
	this->PC += PC[0];
	this->PC += PC[1] << 8;
}
void TagData::setTIDAndTIDLength(char *TID, char TIDLength)
{
	this->TIDLength = TIDLength;
	if(TIDLength > 0)
	{
		this->TID  = "0x";
		char TIDChar[3];
		for(int c = 0; c < TIDLength; c++)
		{
			sprintf(TIDChar, "%02x", TID[c]);
			this->TID += TIDChar;
		}
	}
}
void TagData::setTempCalibrationParams(char *tempCalibrationParams)
{
	this->tempCalibrationParams  = "0x";
	char CalChar[3];
	for(int c = 0; c < 8; c++)
	{
		sprintf(CalChar, "%02x", tempCalibrationParams[c]);
		this->tempCalibrationParams += CalChar;
	}	
}
void TagData::setMMS(char *MMS)
{
	this->MMS = 0;
	this->MMS += MMS[1];
	this->MMS += MMS[0] << 8;
}
void TagData::setVFC(char *VFC)
{
	this->VFC = 0;
	this->VFC += VFC[1];
	this->VFC += VFC[0] << 8;
}
void TagData::setTEMP(char *TEMP)
{
	this->TEMP = 0;
	this->TEMP += TEMP[1];
	this->TEMP += TEMP[0] << 8;
}
unsigned short TagData::getReaderAGC()
{
	return readerAGC;
}
unsigned short TagData::getReaderRSSI()
{
	return readerRSSI;
}
unsigned int TagData::getCommFrequency()
{
	return commFrequency;
}
unsigned short TagData::getEPCLength()
{
	return EPCLength;
}
string TagData::getEPC()
{
	return EPC;
}
unsigned short TagData::getPC()
{
	return PC;
}
unsigned short TagData::getTIDLength()
{
	return TIDLength;
}
string TagData::getTID()
{
	return TID;
}
string TagData::getTempCalibrationParams()
{
	return tempCalibrationParams;
}
unsigned short TagData::getMMS()
{
	return MMS;
}
unsigned short TagData::getVFC()
{
	return VFC;
}
unsigned short TagData::getTEMP()
{
	return TEMP;
}
