/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// uart.h
/// This class implements a UART driver for the UART interface of the BeagleBone
/// Black  It allows you to initialize and release the UART resource and send
/// and receive messages through UART.  
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _UART_H_
#define _UART_H_

#include <termios.h>
#include  <string>

using namespace std;

class UART {
	private:
		int fd;
		struct termios options;
		string fileName;
	public:
		UART(string fileName);
		int initialize();
		int sendMessage(char *buffer, int numberOfBytes);
		int receiveMessage(char *buffer, int numberOfBytes);
		void flush();
		int release();
};
#endif
