/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// interfaces.h
/// This class allows you to interface Hermes through an external device.
/// It allows Hermes to receive commands and send responses through one generic
/// interface to an external device connected to it through UART, TCP, CAN, I2C,
/// SPI.  The commands that Hermes accepts for this Remote User Interface are 
/// listed below (#defines).
///
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _INTERFACES_H_
#define _INTERFACES_H_

#include "interface_enums.h"
#include "uart.h"
#include "tcp_server.h"
#include "can.h"
#include "i2c_bridge.h"
#include "spi_bridge.h"

#define SEARCH_FOR_TEMP_TAGS 			1
#define SEARCH_FOR_TEMP_TAGS_RESP		2
#define SEARCH_FOR_MOISTURE_TAGS 		3
#define SEARCH_FOR_MOISTURE_TAGS_RESP	4
#define MEASURE_TEMP_TAGS				5
#define MEASURE_TEMP_TAGS_RESP			6
#define MEASURE_MOISTURE_TAGS			7
#define MEASURE_MOISTURE_TAGS_RESP		8
#define GET_TEMP_DEMO_SETTINGS			9
#define GET_TEMP_DEMO_SETTINGS_RESP		10
#define GET_MOISTURE_DEMO_SETTINGS		11
#define GET_MOISTURE_DEMO_SETTINGS_RESP	12
#define SET_TEMP_DEMO_SETTINGS			13
#define SET_TEMP_DEMO_SETTINGS_RESP		14
#define SET_MOISTURE_DEMO_SETTINGS		15
#define SET_MOISTURE_DEMO_SETTINGS_RESP	16

class Interfaces 
{
	private:
		Interface::InterfaceType interface;
		UART *uart;
		TCPServer *tcpServer;
		CAN *can;
		I2C_Bridge *i2c_bridge;
		SPI_Bridge *spi_bridge;
	public:
		Interfaces();
		short setType(Interface::InterfaceType interface);
		Interface::InterfaceType getType();
		short connectToClient();
		bool isConnectedToClient();
		void keepConnectionToClient();
		short stopTCPServer();
		short disconnectFromClient();
		short getCommand(char &command, char **payload, short &payloadLength);
		short receiveCmdThruUART(char *cmdMsg, short &cmdMsgLength);
		short receiveCmdThruTCP(char *cmdMsg, short &cmdMsgLength);
		short receiveCmdThruCAN(char *cmdMsg, short &cmdMsgLength);
		short receiveCmdThruI2C(char *cmdMsg, short &cmdMsgLength);
		short receiveCmdThruSPI(char *cmdMsg, short &cmdMsgLength);
		short checkCRC(char *cmdMsg, short &cmdMsgLength);
		short sendResponse(char command, short status, char *message, short msgLength);
		unsigned short calculateCRC(const void *buf, unsigned short len);
};
#endif
