/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// can.h
/// This class implements a CAN driver for the CAN interface of the BeagleBone
/// Black  It allows you to initialize and release the CAN resource and send
/// and receive messages through CAN.
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _CAN_H_
#define _CAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/socket.h>

using namespace std;

class CAN {
	private:
		struct sockaddr_can addr;
		struct ifreq ifr;
		string ifname;
		int fd;
		unsigned int canId;
		struct can_frame receiveFrame;
		bool receiveFrameAvailable;
		short receiveFrameIndex;
		int fdFlags;
	public:
		CAN(string ifname, unsigned int canId);
		int initialize();
		int sendMessage(char *buffer, int numberOfBytes);
		int receiveMessage(char *buffer, int numberOfBytes);
		int release();
};
#endif
