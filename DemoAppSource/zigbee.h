/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// spi_bridge.h
/// This class implements a driver for UART #2 of the BeagleBone Black which is 
/// connected to an I2C- or SPI- UART bridge.  The bridge allows the BeagleBone
/// to send and receive I2C or SPI messages through the UART.  This driver allows
/// you to initialize and release the UART and send and receive messages of different
/// sizes.
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _ZIGBEE_H_
#define _ZIGBEE_H_

#include <termios.h>
#include  <string>

using namespace std;

class ZigBee {
	private:
		int fd;
		struct termios options;
		string fileName;
	public:
		ZigBee(string fileName);
		int initialize();
		int sendMessage(char *buffer, int numberOfBytes);
		int receiveMessage(char *buffer, int numberOfBytes);
		void flush();
		int release();
};
#endif
