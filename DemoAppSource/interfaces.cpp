#include "interfaces.h"
#include <stdio.h>
#include <QTime>
#include <QThread>

#define UART_NUMBER "/dev/ttyO2"
#define TCP_SERVER_PORT_NUMBER 5000
#define CAN_ID 0x122
#define CAN_IF_NAME "can0"
#define I2C_BRIDGE_FILE_NAME "/dev/ttyO2"
#define SPI_BRIDGE_FILE_NAME "/dev/ttyO2"
#define ZIGBEE_FILE_NAME "/dev/ttyO2"
#define CRC16_PRELOAD 0xFFFF
#define MSG_HEADER_SIZE 6
#define TIMEOUT_ERROR -1
#define CRC_ERROR -2
#define MSG_LENGTH_ERROR -2
#define WAIT_FOR_RESPONSE_TIME 30
#define MAX_CMD_MSG_SIZE 100
#define CHECKSUM_ERROR -2
#define ZIGBEE_FRAME_HEADER_SIZE 3
#define ZIGBEE_FRAME_START_DELIMITER 0x7E
#define ZIGBEE_AT_COMMAND 0x08
#define EXPLICIT_ADDRESSING_COMMAND_FRAME 0x11
#define ANNOUNCE_DEVICE_FRAME 0x0013
#define REPORT_ACTIVE_ENDPOINTS_FRAME 0x8005
#define SIMPLE_DESCRIPTOR_RESPONSE_FRAME 0x8004
#define TEMPERATURE_MEASUREMENT_RESPONSE_FRAME 0x8402

#define SET_ZIGBEE_FRAME_START_DELIMITER(buf, value) buf[0] = value 
#define SET_ZIGBEE_FRAME_LENGTH(buf, value) do { buf[2] = value & 0xFF; buf[1] = (value >> 8) & 0xFF; } while ( 0 )
#define SET_ZIGBEE_FRAME_TYPE(buf, value) buf[3] = value 
#define SET_ZIGBEE_FRAME_PAYLOAD(buf, index, value) do { buf[index] = value; } while ( 0 )
#define SET_ZIGBEE_FRAME_CHECKSUM(buf, index, value) do { buf[index] = value; } while ( 0 )
#define GET_ZIGBEE_FRAME_TYPE(buf) buf[3] 
#define GET_ZIGBEE_FRAME_LENGTH(buf) buf[1] << 8 | buf[2]
#define GET_ZIGBEE_FRAME_CHECKSUM(buf, index) buf[index] 
#define GET_ZIGBEE_FRAME_CLUSTER_ID(buf) buf[17] << 8 | buf[18]

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

extern unsigned short crc16OffsetTable[256];

Interfaces::Interfaces()
{
	interface = Interface::NO_INTERFACE_SET;
	uart = NULL;
	tcpServer = NULL;
	can = NULL;
	i2c_bridge = NULL;
	spi_bridge = NULL;
	zigbee = NULL;
}
short Interfaces::setType(Interface::InterfaceType interface)
{
	short status = 0;
	if(interface == Interface::UART)
	{
		if(uart == NULL)
		{
			uart = new UART(UART_NUMBER);
			if(uart->initialize() == 0)
			{
				this->interface = interface;
			}
			else
				status = -1;
		}
		else
			this->interface = interface;
	}
	else if(interface == Interface::TCP)
	{
		if(tcpServer == NULL)
		{
			tcpServer = new TCPServer(TCP_SERVER_PORT_NUMBER);
			if(tcpServer->initialize() == 0)
			{
				this->interface = interface;
			}
			else
				status = -1;
		}
		else
			this->interface = interface;
	}
	else if(interface == Interface::CAN)
	{
		if(can == NULL)
		{
			can = new CAN(CAN_IF_NAME, CAN_ID);
			if(can->initialize() == 0)
			{
				this->interface = interface;
			}
			else
			{
				status = -1;
			}
		}
		else
			this->interface = interface;
	}
	else if(interface == Interface::I2C)
	{
		if(i2c_bridge == NULL)
		{
			i2c_bridge = new I2C_Bridge(I2C_BRIDGE_FILE_NAME);
			if(i2c_bridge->initialize() == 0)
			{
				this->interface = interface;
			}
			else
			{
				status = -1;
			}
		}
		else
			this->interface = interface;
	}
	else if(interface == Interface::SPI)
	{
		if(spi_bridge == NULL)
		{
			spi_bridge = new SPI_Bridge(SPI_BRIDGE_FILE_NAME);
			if(spi_bridge->initialize() == 0)
			{
				this->interface = interface;
			}
			else
			{
				status = -1;
			}
		}
		else
			this->interface = interface;
	}
	else if(interface == Interface::ZIGBEE)
	{
		if(zigbee == NULL)
		{
			zigbee = new ZigBee(ZIGBEE_FILE_NAME);
			if(zigbee->initialize() == 0)
			{
				this->interface = interface;
			}
			else
			{
				status = -1;
			}
		}
		else
			this->interface = interface;
	}
	return status;
}
string Interfaces::sendATCommand(string command, short numberOfBytesToReceive)
{
	char response[20];
	
	zigbee->flush();
	zigbee->sendMessage((char *)command.c_str(), strlen(command.c_str()));
	if(command == "+++")
	{
		QThread::msleep(1500);
	}
	QThread::msleep(50);
	short count = 0;
	QTime timer;
	timer.start();
	do
	{
		if(zigbee->receiveMessage(&response[count], 1) > 0)
		{
			count++;
			timer.restart();
		}
	}while ((count < numberOfBytesToReceive) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	//qDebug("count = %i, number of bytes to receive = %i\n", count, numberOfBytesToReceive);
	if(count != numberOfBytesToReceive)
		return "TIMEOUT ERROR";
	response[count] = '\0';
	return string(response);
}
short Interfaces::configureZigBee()
{
	string response;
	
	QThread::msleep(1500);
	
	response = sendATCommand("+++", 2);
	qDebug("Enter command mode, response = %s\n", response.c_str());
	//if(response != "OK")
	//	return -1;
	response = sendATCommand("ATID 0\r", 2);
	qDebug("Set ID to 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATSC 7FFF\r", 2);
	qDebug("Set SC 7FFF, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATSD 3\r", 2);
	qDebug("Set SD 3, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATZS 2\r", 2);
	qDebug("Set ZS 2, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATNJ 5A\r", 2);
	qDebug("Set NJ 5A, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATNW 0\r", 2);
	qDebug("Set NW 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATJV 0\r", 2);
	qDebug("Set JV 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATJN 0\r", 2);
	qDebug("Set JN 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATCE 0\r", 2);
	qDebug("Set CE 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATDH 0\r", 2);
	qDebug("Set DH 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATDL 0\r", 2);
	qDebug("Set DL 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATNI Hermes\r", 2);
	qDebug("Set NI to Hermes, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATNH 1E\r", 2);
	qDebug("Set NH 1E, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATBH 0\r", 2);
	qDebug("Set BH 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATAR FF\r", 2);
	qDebug("Set AR FF, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATDD A0000\r", 2);
	qDebug("Set DD A0000, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATNT 3C\r", 2);
	qDebug("Set NT 3C, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATNO 3\r", 2);
	qDebug("Set NO 3, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATCR 3\r", 2);
	qDebug("Set CR 3, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATSE E8\r", 2);
	qDebug("Set SE E8, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATDE E8\r", 2);
	qDebug("Set DE E8, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATCI 11\r", 2);
	qDebug("Set CI 11, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATPL 4\r", 2);
	qDebug("Set PL 4, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATEE 1\r", 2);
	qDebug("Set EE 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATEO 1\r", 2);
	qDebug("Set EO 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATKY 5A6967426565416C6C69616E63653039\r", 2);
	qDebug("Set KY, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATNK\r", 2);
	qDebug("Set NK, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATBD 3\r", 2);
	qDebug("Set BD 3, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATNB 0\r", 2);
	qDebug("Set NB 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATSB 0\r", 2);
	qDebug("Set SB 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATRO 3\r", 2);
	qDebug("Set RO 3, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD7 0\r", 2);
	qDebug("Set D7 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD6 0\r", 2);
	qDebug("Set D6 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATAP 1\r", 2);
	qDebug("Set AP 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATAO 3\r", 2);
	qDebug("Set AO 3, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATCT 64\r", 2);
	qDebug("Set CT 64, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATGT 3E8\r", 2);
	qDebug("Set GT 3E8, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATCC 2B\r", 2);
	qDebug("Set CC 2B, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATSP 20\r", 2);
	qDebug("Set SP 20, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATSN 1\r", 2);
	qDebug("Set SN 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATSM 0\r", 2);
	qDebug("Set SM 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATST 1388\r", 2);
	qDebug("Set ST 1388, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATSO 0\r", 2);
	qDebug("Set SO 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD0 1\r", 2);
	qDebug("Set D0 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD2 0\r", 2);
	qDebug("Set D2 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD3 0\r", 2);
	qDebug("Set D3 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD4 0\r", 2);
	qDebug("Set D4 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD5 1\r", 2);
	qDebug("Set D5 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD8 1\r", 2);
	qDebug("Set D8 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATD9 1\r", 2);
	qDebug("Set D9 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATP0 1\r", 2);
	qDebug("Set P0 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATP1 0\r", 2);
	qDebug("Set P1 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATP2 0\r", 2);
	qDebug("Set P2 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATP3 1\r", 2);
	qDebug("Set P3 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATP4 1\r", 2);
	qDebug("Set P4 1, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATPR 1FFF\r", 2);
	qDebug("Set PR 1FFF, response = %s\n", response.c_str());	
	if(response != "OK")
		return -1;
	response = sendATCommand("ATPD 1FBF\r", 2);
	qDebug("Set PD 1FBF, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATLT 0\r", 2);
	qDebug("Set LT 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATRP 28\r", 2);
	qDebug("Set RP 28, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATDO 0\r", 2);
	qDebug("Set DO 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATIR 0\r", 2);
	qDebug("Set IR 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATIC 0\r", 2);
	qDebug("Set IC 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATV+ 0\r", 2);
	qDebug("Set V+ 0, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATWR\r", 2);
	qDebug("Set ATWR, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	response = sendATCommand("ATCN\r", 2);
	qDebug("Set ATCN, response = %s\n", response.c_str());
	if(response != "OK")
		return -1;
	return 0;
}
Interface::InterfaceType Interfaces::getType()
{
	return this->interface;
}
short Interfaces::connectToClient()
{
	return tcpServer->acceptConnection();
}
void Interfaces::keepConnectionToClient()
{
	tcpServer->restartConnTimeoutTimer();
}
short Interfaces::stopTCPServer()
{
	tcpServer->release();
	delete tcpServer;
	tcpServer = NULL;
	return 0;
}
short Interfaces::disconnectFromClient()
{
	return tcpServer->releaseClient();
}
bool Interfaces::isConnectedToClient()
{
	return tcpServer->isConnected();
}
short Interfaces::getCommand(char &command, char **payload, short &payloadLength)
{
	char cmdMsg[100];
	char respMsg[100];
	short cmdMsgLength;
	short receiveCmdRet;
	if(interface == Interface::UART)
		receiveCmdRet = receiveCmdThruUART(cmdMsg, cmdMsgLength);
	else if(interface == Interface::TCP)
		receiveCmdRet = receiveCmdThruTCP(cmdMsg, cmdMsgLength);
	else if(interface == Interface::CAN)
		receiveCmdRet = receiveCmdThruCAN(cmdMsg, cmdMsgLength);
	else if(interface == Interface::I2C)
		receiveCmdRet = receiveCmdThruI2C(cmdMsg, cmdMsgLength);
	else if(interface == Interface::SPI)
		receiveCmdRet = receiveCmdThruSPI(cmdMsg, cmdMsgLength);
	else if(interface == Interface::ZIGBEE)
		receiveCmdRet = receiveCmdThruZIGBEE(cmdMsg, cmdMsgLength);
	if(receiveCmdRet == 0)
	{
		if(interface != Interface::ZIGBEE)
		{
			if(interface != Interface::CAN)
				if(checkCRC(cmdMsg, cmdMsgLength) != 0)
				{
					sendResponse(GET_MESSAGE_TYPE(cmdMsg), CRC_ERROR, respMsg, 6);
					return -1;
				}
			command = GET_MESSAGE_TYPE(cmdMsg);
			payloadLength = cmdMsgLength - MSG_HEADER_SIZE;
			if(payloadLength > 0)
			{
				*payload = new char[payloadLength];
				for(short i = 0; i < payloadLength; i++)
					(*payload)[i] = cmdMsg[6 + i];
			}
		}
		else
		{
			if(checkCheckSum(cmdMsg, cmdMsgLength) != 0)
				return -1;
			command = GET_ZIGBEE_FRAME_TYPE(cmdMsg);
			payloadLength = cmdMsgLength - ZIGBEE_FRAME_HEADER_SIZE - 1;
			if(payloadLength > 0)
			{
				*payload = new char[payloadLength];
				for(short i = 0; i < payloadLength; i++)
					(*payload)[i] = cmdMsg[3 + i];
			}	
		}
		return 0;
	}
	return -1;
}
short Interfaces::receiveCmdThruUART(char *cmdMsg, short &cmdMsgLength)
{
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(uart->receiveMessage(&cmdMsg[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(cmdMsg);
				if(msgLength > MAX_CMD_MSG_SIZE)
				{ 
					return MSG_LENGTH_ERROR;
				}
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	cmdMsgLength = msgLength;
	return 0;
}
short Interfaces::receiveCmdThruTCP(char *cmdMsg, short &cmdMsgLength)
{
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(tcpServer->receiveMessage(&cmdMsg[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(cmdMsg);
				if(msgLength > MAX_CMD_MSG_SIZE)
				{ 
					return MSG_LENGTH_ERROR;
				}
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	cmdMsgLength = msgLength;
	return 0;
}
short Interfaces::receiveCmdThruCAN(char *cmdMsg, short &cmdMsgLength)
{
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(can->receiveMessage(&cmdMsg[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = 8;
				if(msgLength > MAX_CMD_MSG_SIZE)
				{ 
					return MSG_LENGTH_ERROR;
				}
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	cmdMsgLength = msgLength;
	return 0;
}
short Interfaces::receiveCmdThruI2C(char *cmdMsg, short &cmdMsgLength)
{
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(i2c_bridge->receiveMessage(&cmdMsg[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(cmdMsg);
				if(msgLength > MAX_CMD_MSG_SIZE)
				{ 
					return MSG_LENGTH_ERROR;
				}
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	cmdMsgLength = msgLength;
	return 0;
}
short Interfaces::receiveCmdThruSPI(char *cmdMsg, short &cmdMsgLength)
{
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(spi_bridge->receiveMessage(&cmdMsg[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_MESSAGE_LENGTH(cmdMsg);
				if(msgLength > MAX_CMD_MSG_SIZE)
				{ 
					return MSG_LENGTH_ERROR;
				}
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	cmdMsgLength = msgLength;
	return 0;
}
short Interfaces::receiveCmdThruZIGBEE(char *cmdMsg, short &cmdMsgLength)
{
	short count = 0;
	QTime timer;
	unsigned short msgLength = 3;
	timer.start();
	do
	{
		if(zigbee->receiveMessage(&cmdMsg[count], 1) > 0)
		{
			count++;
			if(count == 3)
			{
				msgLength = GET_ZIGBEE_FRAME_LENGTH(cmdMsg);
				msgLength += 4; //add start delimiter, length bytes, and checksum
				if(msgLength > MAX_CMD_MSG_SIZE)
				{ 
					return MSG_LENGTH_ERROR;
				}
			}
			timer.restart();
		}
	}while ((count < msgLength) && (timer.elapsed() < WAIT_FOR_RESPONSE_TIME));
	if(count != msgLength)
		return TIMEOUT_ERROR;
	cmdMsgLength = msgLength;
	return 0;
}
short Interfaces::checkCRC(char *cmdMsg, short &cmdMsgLength)
{
	unsigned short msgCRC  = GET_MESSAGE_CRC(cmdMsg);
	SET_MESSAGE_CRC(cmdMsg, 0);
	unsigned short calculatedCRC = calculateCRC(cmdMsg, cmdMsgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	return 0;
}
short Interfaces::checkCheckSum(char *cmdMsg, short &cmdMsgLength)
{
	unsigned short msgCheckSum = GET_ZIGBEE_FRAME_CHECKSUM(cmdMsg, cmdMsgLength - 1);
	SET_ZIGBEE_FRAME_CHECKSUM(cmdMsg, cmdMsgLength - 1, 0);
	unsigned short calculatedCheckSum = calculateCheckSum(cmdMsg, cmdMsgLength);
	if(msgCheckSum != calculatedCheckSum)
		return CHECKSUM_ERROR;
	return 0;
}
short Interfaces::sendResponse(char command, short status, char *message, short msgLength)
{
	unsigned short crc;
	unsigned char checkSum;
	if(message == NULL && msgLength == 0)
		return -1;
	if(interface == Interface::CAN)
	{	
		for(short i = 0; i < msgLength; i += 8)
		{
			can->sendMessage(&message[i], 8);
			QThread::msleep(10);
		}
	}
	else if(interface == Interface::ZIGBEE)
	{
		short frameDataLength = msgLength;
		short frameLength = frameDataLength + ZIGBEE_FRAME_HEADER_SIZE + 1;  // ZIGBEE_FRAME_HEADER_SIZE + 1 = delimiter, frame data length, checksum
		SET_ZIGBEE_FRAME_START_DELIMITER(message, ZIGBEE_FRAME_START_DELIMITER);
		SET_ZIGBEE_FRAME_LENGTH(message, frameDataLength);
		SET_ZIGBEE_FRAME_TYPE(message, EXPLICIT_ADDRESSING_COMMAND_FRAME);
		SET_ZIGBEE_FRAME_CHECKSUM(message, frameLength - 1, 0);
		checkSum = calculateCheckSum(message, frameLength);
		SET_ZIGBEE_FRAME_CHECKSUM(message, frameLength - 1, checkSum);
		zigbee->flush();
		zigbee->sendMessage(&message[0], frameLength);
	}
	else
	{
		if(command == SEARCH_FOR_TEMP_TAGS)
			SET_MESSAGE_TYPE(message, SEARCH_FOR_TEMP_TAGS_RESP);
		else if(command == SEARCH_FOR_MOISTURE_TAGS)
			SET_MESSAGE_TYPE(message, SEARCH_FOR_MOISTURE_TAGS_RESP);
		else if(command == MEASURE_TEMP_TAGS)
			SET_MESSAGE_TYPE(message, MEASURE_TEMP_TAGS_RESP);
		else if(command == MEASURE_MOISTURE_TAGS)
			SET_MESSAGE_TYPE(message, MEASURE_MOISTURE_TAGS_RESP);
		else if(command == GET_TEMP_DEMO_SETTINGS)
			SET_MESSAGE_TYPE(message, GET_TEMP_DEMO_SETTINGS_RESP);
		else if(command == GET_MOISTURE_DEMO_SETTINGS)
			SET_MESSAGE_TYPE(message, GET_MOISTURE_DEMO_SETTINGS_RESP);
		else if(command == SET_TEMP_DEMO_SETTINGS)
			SET_MESSAGE_TYPE(message, SET_TEMP_DEMO_SETTINGS_RESP);
		else if(command == SET_MOISTURE_DEMO_SETTINGS)
			SET_MESSAGE_TYPE(message, SET_MOISTURE_DEMO_SETTINGS_RESP);
		else
		{
			qDebug("invalid command: %i\n", command);
			return -1;
		}
		SET_MESSAGE_LENGTH(message, msgLength);
		SET_MESSAGE_CRC(message, 0);
		SET_MESSAGE_STATUS(message, status);
		crc = calculateCRC(message, msgLength);
		SET_MESSAGE_CRC(message, crc);
		if(interface == Interface::UART)
		{
			uart->flush();
			uart->sendMessage(&message[0], msgLength);
		}
		else if(interface == Interface::TCP)
		{
			tcpServer->sendMessage(&message[0], msgLength);
		}
		else if(interface == Interface::I2C)
		{
			i2c_bridge->flush();
			i2c_bridge->sendMessage(&message[0], msgLength);
		}
		else if(interface == Interface::SPI)
		{
			spi_bridge->flush();
			spi_bridge->sendMessage(&message[0], msgLength);
		}
	}
	delete[] message;
	message = NULL;
	return 0;
}
unsigned short Interfaces::calculateCRC(const void *buf, unsigned short len)
{
	short counter;
	unsigned short crc = CRC16_PRELOAD;
	signed char *sbuf = (signed char *)buf;
	for (counter = 0; counter < len; counter++)
		crc = (crc << 8) ^ crc16OffsetTable[((crc >> 8) ^ *sbuf++) & 0x00FF];
	return crc;
}
unsigned short Interfaces::calculateCheckSum(const void *buf, unsigned short len)
{
	short checkSum = 0;
	for(short i = 3; i <= len - 2; i++)
		checkSum += ((char *)buf)[i];
	checkSum &= 0xFF;
	checkSum = 0xFF - checkSum;
	return checkSum;
}
