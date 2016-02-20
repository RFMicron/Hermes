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
#define CRC16_PRELOAD 0xFFFF
#define MSG_HEADER_SIZE 6
#define TIMEOUT_ERROR -1
#define CRC_ERROR -2
#define MSG_LENGTH_ERROR -2
#define WAIT_FOR_RESPONSE_TIME 30
#define MAX_CMD_MSG_SIZE 25

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
	}
	return status;
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
	if(receiveCmdRet == 0)
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
short Interfaces::checkCRC(char *cmdMsg, short &cmdMsgLength)
{
	unsigned short msgCRC  = GET_MESSAGE_CRC(cmdMsg);
	SET_MESSAGE_CRC(cmdMsg, 0);
	unsigned short calculatedCRC = calculateCRC(cmdMsg, cmdMsgLength);
	if(msgCRC != calculatedCRC)
		return CRC_ERROR;
	return 0;
}
short Interfaces::sendResponse(char command, short status, char *message, short msgLength)
{
	unsigned short crc;
	if(interface == Interface::CAN)
	{	
		for(short i = 0; i < msgLength; i += 8)
		{
			can->sendMessage(&message[i], 8);
			QThread::msleep(10);
		}
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
