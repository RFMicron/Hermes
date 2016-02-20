/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// i2c_bridge.h
/// This class implements a driver for UART #2 of the BeagleBone Black which is 
/// connected to an I2C- or SPI- UART bridge.  The bridge allows the BeagleBone
/// to send and receive I2C or SPI messages through the UART.  This driver allows
/// you to initialize and release the UART and send and receive messages of different
/// sizes.
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <QTime>

class TCPServer {
	private:
		int fd;
		int fdFlags;
		int clientFdFlags;
		int clientFd;
		int portNumber;
		socklen_t clilen;
		struct sockaddr_in serv_addr;
		struct sockaddr_in cli_addr;
		QTime timer;
	public:
		TCPServer(int portNumber);
		int initialize();
		int acceptConnection();
		bool isConnected();
		int disconnectFromClient();
		void restartConnTimeoutTimer();
		int sendMessage(char *buffer, int numberOfBytes);
		int receiveMessage(char *buffer, int numberOfBytes);
		int release();
		int releaseClient();
};
#endif
