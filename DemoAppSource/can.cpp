#include "can.h"
#include <QDebug>

CAN::CAN(string ifname, unsigned int canId) 
{
	receiveFrameAvailable = false;
	receiveFrameIndex = 0;
	this->canId = canId;
	this->ifname = ifname;
}
int CAN::initialize() {
	fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if(!(fd < 0))
	{
		strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
		if(!ifr.ifr_ifindex)
		{
			qDebug("error: if_nametoindex\n");
			return -1;
		}
		addr.can_family = AF_CAN;
		addr.can_ifindex = ifr.ifr_ifindex;
		fdFlags = fcntl(fd, F_GETFL, 0);
		fdFlags |= O_NONBLOCK;
		fcntl(fd, F_SETFL, fdFlags);
		if(bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		{
			qDebug("failed to bind\n");
			return -3;
		}
		return 0;
	}
	return -4;
}
int CAN::sendMessage(char *buffer, int numberOfBytes) {
	struct can_frame frame;
	int count;
	frame.can_id = canId;
	short c = 0;
	for(short i = 0; i < numberOfBytes; i++)
	{
		frame.data[c] = buffer[i];
		c++;
		if(c == 8 || i == (numberOfBytes - 1))
		{
			frame.can_dlc = c;
			count = write(fd, &frame, sizeof(struct can_frame));
			if(count != sizeof(struct can_frame)) 
			{
				return -2;
			}
			c = 0;
		}
	}
	return 0;
}
int CAN::receiveMessage(char *buffer, int numberOfBytes) 
{
	int count = 0;
	short c = 0;
	short i = 0;
	int bytesRead = numberOfBytes;
	while(numberOfBytes > 0)
	{
		if(receiveFrameAvailable == false)
		{	
			count = read(fd, &receiveFrame, sizeof(struct can_frame));
			if(count <= 0)
			{
				return count;
			}
		}
		else
		{
			c = receiveFrameIndex;
		}
		while(numberOfBytes  > 0)
		{
			buffer[i] = receiveFrame.data[c];
			c++; i++; numberOfBytes--;
			if(c == receiveFrame.can_dlc)
			{
				c = 0;
				if(receiveFrameAvailable == true)
				{
					receiveFrameAvailable = false;
				}
				break;
			}
		}
	}
	if(c > 0)
	{
		receiveFrameAvailable = true;
		receiveFrameIndex = c;
	}
	return bytesRead;
}
int CAN::release() {
	int ret = close(fd);
	fd = 0;
	return ret;
}
