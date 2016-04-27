#include "rui_thread.h"
#include <string.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <QThread>

#define SET_MESSAGE_PAYLOAD(buf, index, value) do { buf[index] = value; } while ( 0 )
#define GET_MESSAGE_PAYLOAD(buf, index) buf[index]
#define MAX_SEARCH_TIME 30000 //search for tags for upto 30 seconds
#define MSG_HEADER_SIZE 6
#define EPCMAXLENGTH 32
#define TIDMAXLENGTH 12
#define EPCLEN_LENGTH 1
#define TIDLEN_LENGTH 1
#define TEMPCALC1_LENGTH 4
#define TEMPCALT1_LENGTH 4
#define TEMPCALC2_LENGTH 4
#define TEMPCALT2_LENGTH 4
#define CRCVALID_LENGTH  1
#define FREQ_LENGTH 4
#define ONCHIPRSSICODE_LENGTH 4
#define TEMPCODE_LENGTH 4
#define SENSORCODE_LENGTH 4
#define EPC_LEN_RESP 1
#define EPC_RESP 2       
#define TID_LEN_RESP 3
#define TID_RESP 4
#define TEMP_CAL_C1_RESP 5
#define TEMP_CAL_T1_RESP 6
#define TEMP_CAL_C2_RESP 7
#define TEMP_CAL_T2_RESP 8
#define CRCVALID_RESP 9 
#define TEMP_VALUE_RESP 10
#define SENSOR_VALUE_RESP 11
#define ONCHIPRSSI_VALUE_RESP 12
#define DONE_RESP 13
#define GET_ZIGBEE_DATA_FRAME_STATUS(buf)  buf[1]
#define GET_ZIGBEE_DATA_FRAME_DELIVERY_ID(buf) buf[1]
#define GET_ZIGBEE_DATA_FRAME_DELIVERY_STATUS(buf) buf[5]
#define GET_ZIGBEE_DATA_FRAME_CLUSTER_ID(buf) buf[13] << 8 | buf[14]
#define GET_ZIGBEE_DATA_FRAME_ENDPOINT_REQUEST_ID(buf) buf[18]
#define GET_ZIGBEE_DATA_FRAME_SIMPLE_DESCRIPTOR_REQUEST_ID(buf) buf[18]
#define GET_ZIGBEE_TEMP_MEASUREMENT_REQUEST_CONTROL_FRAME(buf) buf[18]
#define GET_ZIGBEE_TEMP_MEASUREMENT_REQUEST_FRAME_ID(buf) buf[19]
#define SET_ZIGBEE_FRAME_PAYLOAD(buf, index, value) do { buf[index] = value; } while ( 0 )
#define MODEM_STATUS_FRAME 0x8A
#define TRANSMIT_STATUS_FRAME  0x8B
#define EXPLICIT_RX_INDICATOR_FRAME 0x91
#define ACTIVE_ENDPOINTS_REQUEST_FRAME 0x0005
#define SIMPLE_DESCRIPTOR_REQUEST_FRAME 0x0004
#define TEMPERATURE_MEASUREMENT_REQUEST_FRAME 0x0402
#define ZIGBEE_HAVE_JOINED_NETWORK 0x02
#define ZIGBEE_FRAME_HEADER_SIZE 3 
#define ANNOUNCE_DEVICE_FRAME_SIZE  32
#define REPORT_ENDPOINT_FRAME_SIZE 26
#define SIMPLE_DESCRIPTOR_REQUEST_FRAME_SIZE 40
#define TEMP_MEASUREMENT_REQUEST_FRAME_SIZE 26
#define ZIGBEE_CHECKSUM_SIZE 1
#define ZIGBEE_TRANSMISSION_SUCCESSFUL 0

RUIThread::RUIThread(QObject *parent) : QThread(parent)
{
	abort = false;
	mode = Interface::NORMAL;
}
void RUIThread::initialize(KitController *controller, KitModel *model, GPIO *xBeeResetLine)
{
	this->controller = controller;
	this->model = model;
	this->xBeeResetLine = xBeeResetLine;
}
short RUIThread::setType(Interface::InterfaceType interface)
{
	if(!isRunning())
	{
		return this->interface.setType(interface);
	}
	else
		return -1;
}
short RUIThread::setMode(Interface::Mode mode)
{
	if(!isRunning())
	{
		this->mode = mode;
		return 0;
	}
	else
		return -1;
}
short RUIThread::startInterface()
{
	if(interface.getType() == Interface::NO_INTERFACE_SET)
		return -1;
	qDebug("in start interface, abort = %i\n", abort);
	if(!isRunning())
	{
		qDebug("starting interface\n");
		abort = false;
		model->setAbort(false);
		start(LowPriority);
	}
	else
		qDebug("interface already running\n");
	return 0;
}
void RUIThread::stopInterface()
{
	mutex.lock();
	abort = true;
	model->setAbort(true);
	mutex.unlock();
	wait();	
	if(interface.getType() == Interface::TCP)
	{
		interface.disconnectFromClient();
		interface.stopTCPServer();
	}
}
void RUIThread::run()
{
	char command;
	char *payload = NULL;
	short payloadLength;
	char *message = NULL;
	short msgLength;
	short status;
	char msg[80];
	bool notConnectedMsgDisplayed = false;
	qDebug("RUI Thread running...\n");
	Interface::InterfaceType interfaceType = interface.getType();
	if(interfaceType == Interface::ZIGBEE)
	{
		if(interface.configureZigBee() != 0)
		{
			qDebug("unable to configure ZigBee!\n");
			sprintf(msg, "Unable to configure ZigBee, interface failed!\n");
			emit outputToConsole(QString(msg), QString("Red"));
			return;
		}
		else
		{
			qDebug("configured ZigBee! Resetting XBEE module...\n");
			sprintf(msg, "Configured ZigBee! Resetting XBEE module...\n");
			emit outputToConsole(QString(msg), QString("Red"));
			xBeeResetLine->setValue(GPIO::LOW);
			QThread::msleep(100);
			xBeeResetLine->setValue(GPIO::HIGH);
			qDebug("XBEE module reset!\n");
			sprintf(msg, "XBEE module reset!\n");
			emit outputToConsole(QString(msg), QString("Red"));
		}
	}
	sprintf(msg, "Interface running, listening for commands...\n");
	emit outputToConsole(QString(msg), QString("Red"));
	while(abort != true)
	{
		if(interfaceType == Interface::TCP)
		{
			if(interface.isConnectedToClient() == false)
			{
				if(notConnectedMsgDisplayed == false)
				{
					sprintf(msg, "Not connected to client\n");
					emit outputToConsole(QString(msg), QString("Red"));
					notConnectedMsgDisplayed = true;
				}
				
				if(interface.connectToClient() != 0)
				{
					continue;
				}
				else
				{
					sprintf(msg, "Connected to client\n");
					emit outputToConsole(QString(msg), QString("Red"));
					notConnectedMsgDisplayed = false;
				}
			}
		}
		if(interface.getCommand(command, &payload, payloadLength) == 0)
		{
			if(interfaceType == Interface::TCP)
				interface.keepConnectionToClient();
			if(interfaceType == Interface::CAN)
				status = processCANCommand(command, payload, payloadLength, &message, msgLength);
			else if(interfaceType == Interface::ZIGBEE)
				status = processZigBeeCommand(command, payload, payloadLength, &message, msgLength);
			else
				status = processCommand(command, payload, payloadLength, &message, msgLength);
			QThread::yieldCurrentThread();	
			interface.sendResponse(command, status, message, msgLength);
		}
	}
	sprintf(msg, "Interface stopped\n");
	emit outputToConsole(QString(msg), QString("Red"));
	return;
}
short RUIThread::processZigBeeCommand(char command, char *payload, short &payloadLength, char **message, short &msgLength)
{
	short status;
	short payloadIndex;
	char msg[80];
	char modemStatus;
	char deliveryStatus;
	short clusterID;
	float currentTempOfATag;
	switch(command)
	{
		case MODEM_STATUS_FRAME:
				qDebug("received modem status\n");
				modemStatus = GET_ZIGBEE_DATA_FRAME_STATUS(payload);
				qDebug("modem status = 0x%x\n", modemStatus);
				if(modemStatus == ZIGBEE_HAVE_JOINED_NETWORK)
				{
					qDebug("joined network, modemStatus = %i\n", modemStatus);
					sprintf(msg, "Joined ZigBee network, obtaining network addresses...\n");
					emit outputToConsole(QString(msg), QString("Red"));
					QThread::msleep(1200);
					string response = interface.sendATCommand("+++", 3);
					QThread::msleep(1200);
					response = interface.sendATCommand("ATMY\r", 4);
					qDebug("response = %s\n", response.c_str());
					if(response != "TIMEOUT ERROR")
					{
						this->my16BitNetworkAddr = strtol(response.c_str(), NULL, 16);
						qDebug("my 16-bit network addr = 0x%4x\n", my16BitNetworkAddr);
					
						response = interface.sendATCommand("ATSH\r", 6);
						qDebug("response = %s\n", response.c_str());
						if(response != "TIMEOUT ERROR")
						{
							this->my64BitNetworkAddrHigh = strtol(response.c_str(), NULL, 16);
							qDebug("my 64-bit network addr high = 0x%x\n", my64BitNetworkAddrHigh);
						
							response = interface.sendATCommand("ATSL\r", 8);
							qDebug("response = %s\n", response.c_str());
							if(response != "TIMEOUT ERROR")
							{
								this->my64BitNetworkAddrLow = strtol(response.c_str(), NULL, 16);
								qDebug("my 64-bit network addr low = 0x%x\n", my64BitNetworkAddrLow);
								
								response = interface.sendATCommand("ATCN\r", 2);
								qDebug("response = %s\n", response.c_str());
							}
						}
					}
					if(response == "OK")
					{
						*message = new char[ZIGBEE_FRAME_HEADER_SIZE + ANNOUNCE_DEVICE_FRAME_SIZE + ZIGBEE_CHECKSUM_SIZE];
						payloadIndex = 4;
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x01); //frame ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0xFF); //16 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0xFC); //16 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //source endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //dest endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //cluster ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x13); //cluster ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //profile ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //profile ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //broadcast radius
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //transmit options
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0xAB); //a frame ID - unimportant field
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my16BitNetworkAddr & 0xFF); //our 16-bit network address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my16BitNetworkAddr >>8 & 0xFF); //our 16-bit network address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my64BitNetworkAddrLow & 0xFF); //our 64-bit address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my64BitNetworkAddrLow >> 8 & 0xFF); //our 64-bit address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my64BitNetworkAddrLow >> 16 & 0xFF); //our 64-bit address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my64BitNetworkAddrLow >> 24 & 0xFF); //our 64-bit address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my64BitNetworkAddrHigh & 0xFF); //our 64-bit address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my64BitNetworkAddrHigh >> 8 & 0xFF); //our 64-bit address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my64BitNetworkAddrHigh >> 16 & 0xFF); //our 64-bit address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my64BitNetworkAddrHigh >> 24 & 0xFF); //our 64-bit address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x04); //capability: Home Automation
						msgLength = ANNOUNCE_DEVICE_FRAME_SIZE;
						qDebug("Announcing device to Hub...\n");
						sprintf(msg, "Announcing device to Hub...\n");
						emit outputToConsole(QString(msg), QString("Red"));
					}
					else
					{
						*message = NULL;
						msgLength = 0;
						qDebug("failed to obtain network addresses\n");
						sprintf(msg, "failed to obtain network addresses\n");
						emit outputToConsole(QString(msg), QString("Red"));	
					}
				}
				else
				{
					*message = NULL;
					msgLength = 0;
					qDebug("have not joined a ZigBee network\n");
					sprintf(msg, "Have not joined a ZigBee network\n");
					emit outputToConsole(QString(msg), QString("Red"));	
				}
			break;
		case TRANSMIT_STATUS_FRAME:
				qDebug("received transmit status\n");
				deliveryStatus = GET_ZIGBEE_DATA_FRAME_DELIVERY_STATUS(payload);
				if(deliveryStatus == ZIGBEE_TRANSMISSION_SUCCESSFUL)
				{
					qDebug("transmission ok\n");
					short frameTransmittedID = GET_ZIGBEE_DATA_FRAME_DELIVERY_ID(payload);
					if(frameTransmittedID == 0x03)
					{
						qDebug("connected to Hub!\n");
						sprintf(msg, "Connected to Hub!\n");
						emit outputToConsole(QString(msg), QString("Red"));
					}
				}
				*message = NULL;
				msgLength = 0;
			break;
		case EXPLICIT_RX_INDICATOR_FRAME:
			qDebug("received explicit rx indicator frame\n");
			clusterID = GET_ZIGBEE_DATA_FRAME_CLUSTER_ID(payload);
			switch (clusterID)
			{
				case ACTIVE_ENDPOINTS_REQUEST_FRAME:
						qDebug("received active endpoints request frame\n");
						*message = new char[ZIGBEE_FRAME_HEADER_SIZE + REPORT_ENDPOINT_FRAME_SIZE + ZIGBEE_CHECKSUM_SIZE];
						payloadIndex = 4;
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x02); //frame ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //16 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //16 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //source endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //dest endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x80); //cluster ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x05); //cluster ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //profile ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //profile ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //broadcast radius
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //transmit options
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, GET_ZIGBEE_DATA_FRAME_ENDPOINT_REQUEST_ID(payload));
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //status 00=OK
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my16BitNetworkAddr & 0xFF); //our 16-bit network address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my16BitNetworkAddr >> 8 & 0xFF); //our 16-bit network address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x01); //number of endpoints
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x08); //endpoint address
						msgLength = REPORT_ENDPOINT_FRAME_SIZE;
						qDebug("Reporting Endpoints...\n");
						sprintf(msg, "Reporting Endpoints...\n");
						emit outputToConsole(QString(msg), QString("Red"));
					break;
				case SIMPLE_DESCRIPTOR_REQUEST_FRAME:
						qDebug("received simple descriptor request frame\n");
						*message = new char[ZIGBEE_FRAME_HEADER_SIZE + SIMPLE_DESCRIPTOR_REQUEST_FRAME_SIZE + ZIGBEE_CHECKSUM_SIZE];
						payloadIndex = 4;
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x03); //frame ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //16 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //16 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //source endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //dest endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x80); //cluster ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x04); //cluster ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //profile ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //profile ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //broadcast radius
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //transmit options
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, GET_ZIGBEE_DATA_FRAME_SIMPLE_DESCRIPTOR_REQUEST_ID(payload));
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //status 00=OK
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my16BitNetworkAddr & 0xFF); //our 16-bit network address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, this->my16BitNetworkAddr >> 8 & 0xFF); //our 16-bit network address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x0E); //number of bytes sent after this one
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x08); //endpoint address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x04); //home automation endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x01); //home automation endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x02); //on/off output
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //on/off output
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x30); //version numbers
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x03); //number of clusters we can accept in
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //first cluster type
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //first cluster type
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x03); //second cluster type
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //second cluster type
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x06); //third cluster type
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //third cluster type
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //number of clusters we send out
						msgLength = SIMPLE_DESCRIPTOR_REQUEST_FRAME_SIZE;
						qDebug("Sending Simple Descriptor...\n");
						sprintf(msg, "Sending Simple Descriptor...\n");
						emit outputToConsole(QString(msg), QString("Red"));						
					break;
				case TEMPERATURE_MEASUREMENT_REQUEST_FRAME:
						qDebug("received temperature measurement request frame\n");
						sprintf(msg, "Received temperature measurement request, reading tag...\n");
						emit outputToConsole(QString(msg), QString("Red"));
						*message = new char[ZIGBEE_FRAME_HEADER_SIZE + TEMP_MEASUREMENT_REQUEST_FRAME_SIZE + ZIGBEE_CHECKSUM_SIZE];
						payloadIndex = 4;
						currentTempOfATag = getTempOfATag();
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x04); //frame ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //64 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //16 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //16 bit dest address
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //source endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //dest endpoint
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x84); //cluster ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x02); //cluster ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0xC1); //profile ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x04); //profile ID
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //broadcast radius
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0x00); //transmit options
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, GET_ZIGBEE_TEMP_MEASUREMENT_REQUEST_CONTROL_FRAME(payload));
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, GET_ZIGBEE_TEMP_MEASUREMENT_REQUEST_FRAME_ID(payload));
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0); //measured value identifier
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, 0); //measured value identifier
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, (int)(currentTempOfATag * 10) & 0xFF); //temperature measured, low byte
						SET_ZIGBEE_FRAME_PAYLOAD((*message), payloadIndex++, ((int)(currentTempOfATag * 10) >> 8) & 0xFF); //temperature measured, high byte
						msgLength = TEMP_MEASUREMENT_REQUEST_FRAME_SIZE;
						if(currentTempOfATag == 0xE3)
						{
							qDebug("did not find a tag!\n");
							sprintf(msg, "Did not find a tag!\n");
							emit outputToConsole(QString(msg), QString("Red"));
						}
						else if(currentTempOfATag == 0xE2)
						{
							qDebug("could not measure temperature of tag\n");
							sprintf(msg, "Could not measure temperature of tag!\n");
							emit outputToConsole(QString(msg), QString("Red"));
						}
						else
						{
							qDebug("Sending Temperature Measurement of %3.2f degrees C...\n", currentTempOfATag);
							sprintf(msg, "Sending temperature measurement of %3.2f degrees C...\n", currentTempOfATag);
							emit outputToConsole(QString(msg), QString("Red"));
						}
					break;
				default:
						*message = NULL;
						msgLength = 0;
						qDebug("received cluster = 0x%x. Not handled.\n", clusterID);
					break;
			}
			break;
		default:
				*message = NULL;
				msgLength = 0;
				qDebug("received command = 0x%x. Not handled.\n", command);
			break;
	}
	return status;
}
short RUIThread::processCommand(char command, char *payload, short &payloadLength, char **message, short &msgLength)
{
	short status;
	char msg[80];
	short numberOfTagsFound;
	short payloadIndex;
	short size;
	short tagsSensorReadHistorySizeTotal;
	short tagsSensorReadHistorySize;
	int currentFreqBand;
	int tempAutoPower;
	int tempMaxPower;
	int TempTargetOnChipRssiMin;
	int TempTargetOnChipRssiMax;
	int tempMinSamplesPerMeas;
	int moistAutoPower;
	int moistMaxPower;
	int MoistTargetOnChipRssiMin;
	int MoistTargetOnChipRssiMax;
	int moistMinSamplesPerMeas;
	switch(command)
	{
		case SEARCH_FOR_TEMP_TAGS:
			qDebug("Received SEARCH FOR TEMP TAGS\n");
			sprintf(msg, "Received SEARCH FOR TEMP TAGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			controller->searchForTempTags(MAX_SEARCH_TIME);
			numberOfTagsFound = model->TempTagList.size();
			size = MSG_HEADER_SIZE + numberOfTagsFound * (EPCMAXLENGTH*2 + TIDMAXLENGTH*2 + EPCLEN_LENGTH + TIDLEN_LENGTH +
					TEMPCALC1_LENGTH + TEMPCALT1_LENGTH + TEMPCALC2_LENGTH + TEMPCALT2_LENGTH 
					+ CRCVALID_LENGTH + 2) + 1;
			*message = new char[size];
			if(!((*message)))
			{
				qDebug("failed to allocate memory\n");
				*message = NULL;
				msgLength = 0;
				status = -1;
				break;
			}
			payloadIndex = 6;
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, numberOfTagsFound);
			sprintf(msg, "Number of tags found: %i\n", numberOfTagsFound);
			emit outputToConsole(QString(msg), QString("Red"));
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				sprintf(msg, "Tag number: %i\n", i + 1);
				emit outputToConsole(QString(msg), QString("Red"));
				string epcStr = model->TempTagList[i].getEpc().toStdString();
				string tidStr = model->TempTagList[i].getTid().toStdString();
				short epcLength =  epcStr.length();
				short tidLength = tidStr.length();
				int tempCalC1 = model->TempTagList[i].getTempCalC1();
				float tempCalT1 = model->TempTagList[i].getTempCalT1();
				int tempCalC2 = model->TempTagList[i].getTempCalC2();
				float tempCalT2 = model->TempTagList[i].getTempCalT2();
				bool crcValid = model->TempTagList[i].getCrcValid(); 
				short tagsDataLength = epcLength + EPCLEN_LENGTH +
					tidLength + TIDLEN_LENGTH +
					TEMPCALC1_LENGTH +
					TEMPCALT1_LENGTH +
					TEMPCALC2_LENGTH +
					TEMPCALT2_LENGTH +
					CRCVALID_LENGTH;
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tagsDataLength);
				sprintf(msg, "tag's data length: %i\n", tagsDataLength);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength);
				sprintf(msg, "EPC length: %i\n", epcLength);
				emit outputToConsole(QString(msg), QString("Red"));
				for(short c = 0; c < epcLength; c++)
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcStr.at(c));
				sprintf(msg, "EPC: 0x");
				char temp[2];
				temp[1] = '\0';
				for(short j = 0; j < epcLength; j++)
				{
					sprintf(temp, "%c", epcStr.at(j));
					strcat(msg, temp);
				}
				strcat(msg, "\n");
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength);
				sprintf(msg, "TID length: %i\n", tidLength);
				emit outputToConsole(QString(msg), QString("Red"));
				for(short k = 0; k < tidLength; k++)
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidStr.at(k));
				sprintf(msg, "TID: 0x");
				for(short j = 0; j < tidLength; j++)
				{
					sprintf(temp, "%c", tidStr.at(j));
					strcat(msg, temp);
				}
				strcat(msg, "\n");
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 24 & 0xFF);
				sprintf(msg, "temp cal C1: %i\n", tempCalC1);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 24 & 0xFF);
				sprintf(msg, "temp cal T1: %i\n", (int)tempCalT1);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 24 & 0xFF);
				sprintf(msg, "temp cal C2: %i\n", tempCalC2);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 24 & 0xFF);
				sprintf(msg, "temp cal T2: %i\n", (int)tempCalT2);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, crcValid);
				sprintf(msg, "crc valid: %i\n", crcValid);
				emit outputToConsole(QString(msg), QString("Red"));
			}
			msgLength = payloadIndex + MSG_HEADER_SIZE;
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		case SEARCH_FOR_MOISTURE_TAGS:
			qDebug("Received SEARCH FOR MOISTURE TAGS\n");
			sprintf(msg, "Received SEARCH FOR MOISTURE TAGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			controller->searchForMoistureTags(MAX_SEARCH_TIME);
			numberOfTagsFound = model->MoistTagList.size();
			size = MSG_HEADER_SIZE + numberOfTagsFound * (EPCMAXLENGTH*2 + TIDMAXLENGTH*2 + EPCLEN_LENGTH + TIDLEN_LENGTH +
					TEMPCALC1_LENGTH + TEMPCALT1_LENGTH + TEMPCALC2_LENGTH + TEMPCALT2_LENGTH 
					+ CRCVALID_LENGTH + 2) + 1;
			*message = new char[size];
			if(!((*message)))
			{
				qDebug("failed to allocate memory\n");
				*message = NULL;
				msgLength = 0;
				status = -1;
				break;
			}
			payloadIndex = 6;
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, numberOfTagsFound);
			sprintf(msg, "Number of tags found: %i\n", numberOfTagsFound);
			emit outputToConsole(QString(msg), QString("Red"));
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				sprintf(msg, "Tag number: %i\n", i + 1);
				emit outputToConsole(QString(msg), QString("Red"));
				string epcStr = model->MoistTagList[i].getEpc().toStdString();
				string tidStr = model->MoistTagList[i].getTid().toStdString();
				short epcLength =  epcStr.length();
				short tidLength = tidStr.length();
				int tempCalC1 = model->MoistTagList[i].getTempCalC1();
				float tempCalT1 = model->MoistTagList[i].getTempCalT1();
				int tempCalC2 = model->MoistTagList[i].getTempCalC2();
				float tempCalT2 = model->MoistTagList[i].getTempCalT2();
				bool crcValid = model->MoistTagList[i].getCrcValid(); 
				short tagsDataLength = epcLength + EPCLEN_LENGTH +
					tidLength + TIDLEN_LENGTH +
					TEMPCALC1_LENGTH +
					TEMPCALT1_LENGTH +
					TEMPCALC2_LENGTH +
					TEMPCALT2_LENGTH +
					CRCVALID_LENGTH;
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tagsDataLength);
				sprintf(msg, "tag's data length: %i\n", tagsDataLength);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength);
				sprintf(msg, "EPC length: %i\n", epcLength);
				emit outputToConsole(QString(msg), QString("Red"));
				for(short c = 0; c < epcLength; c++)
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcStr.at(c));
				sprintf(msg, "EPC: 0x");
				char temp[2];
				temp[1] = '\0';
				for(short j = 0; j < epcLength; j++)
				{
					sprintf(temp, "%c", epcStr.at(j));
					strcat(msg, temp);
				}
				strcat(msg, "\n");
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength);
				sprintf(msg, "TID length: %i\n", tidLength);
				emit outputToConsole(QString(msg), QString("Red"));
				for(short k = 0; k < tidLength; k++)
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidStr.at(k));
				sprintf(msg, "TID: 0x");
				for(short j = 0; j < tidLength; j++)
				{
					sprintf(temp, "%c", tidStr.at(j));
					strcat(msg, temp);
				}
				strcat(msg, "\n");
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 24 & 0xFF);
				sprintf(msg, "temp cal C1: %i\n", tempCalC1);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 24 & 0xFF);
				sprintf(msg, "temp cal T1: %i\n", (int)tempCalT1);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 24 & 0xFF);
				sprintf(msg, "temp cal C2: %i\n", tempCalC2);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 24 & 0xFF);
				sprintf(msg, "temp cal T2: %i\n", (int)tempCalT2);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, crcValid);
				sprintf(msg, "crc valid: %i\n", crcValid);
				emit outputToConsole(QString(msg), QString("Red"));
			}
			msgLength = payloadIndex + MSG_HEADER_SIZE;
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));		
			break;
		case MEASURE_TEMP_TAGS:
			qDebug("Received MEASURE TEMP TAGS\n");
			sprintf(msg, "Received MEASURE TEMP TAGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			controller->measureTempTags();
			numberOfTagsFound = model->TempTagList.size();
			tagsSensorReadHistorySizeTotal = 0;
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				tagsSensorReadHistorySizeTotal += model->TempTagList[i].SensorReadHistory.size();
			}
			*message = new char[MSG_HEADER_SIZE + 1 + numberOfTagsFound * (1 + 2) + tagsSensorReadHistorySizeTotal * (FREQ_LENGTH + ONCHIPRSSICODE_LENGTH + 
					TEMPCODE_LENGTH)]; 
			if(!((*message)))
			{
				qDebug("failed to allocate memory\n");
				*message = NULL;
				msgLength = 0;
				status = -1;
				break;
			}
			payloadIndex = 6;
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, numberOfTagsFound);
			sprintf(msg, "Number of tags found: %i\n", numberOfTagsFound);
			emit outputToConsole(QString(msg), QString("Red"));
			qDebug("Number of tags found: %i\n", numberOfTagsFound);
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				sprintf(msg, "Tag number: %i\n", i + 1);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("Tag number: %i\n", i + 1);
				short tagsDataLength = FREQ_LENGTH +
					ONCHIPRSSICODE_LENGTH +
					TEMPCODE_LENGTH;
				tagsSensorReadHistorySize = model->TempTagList[i].SensorReadHistory.size();								     
				tagsDataLength *= tagsSensorReadHistorySize;
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tagsDataLength & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tagsDataLength >> 8 & 0xFF);
				sprintf(msg, "tag's data length: %i\n", tagsDataLength);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("tag's data length: %i\n", tagsDataLength);
				for(short c = 0; c < tagsSensorReadHistorySize; c++)
				{
					int freq = model->TempTagList[i].SensorReadHistory[c].getFrequencyKHz();
					int onChipRssiCode = model->TempTagList[i].SensorReadHistory[c].getOnChipRssiCode();
					int tempCode = model->TempTagList[i].SensorReadHistory[c].getTemperatureCode();
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, freq & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, freq >> 8 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, freq >> 16 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, freq >> 24 & 0xFF);
					sprintf(msg, "freq: %i\n", freq);
					emit outputToConsole(QString(msg), QString("Red"));
					qDebug("freq: %i\n", freq);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, onChipRssiCode & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, onChipRssiCode >> 8 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, onChipRssiCode >> 16 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, onChipRssiCode >> 24 & 0xFF);
					sprintf(msg, "on chip RSSI code: %i\n", onChipRssiCode);
					emit outputToConsole(QString(msg), QString("Red"));
					qDebug("on chip RSSI code: %i\n", onChipRssiCode);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCode & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCode >> 8 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCode >> 16 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCode >> 24 & 0xFF);
					sprintf(msg, "temp code: %i\n", tempCode);
					emit outputToConsole(QString(msg), QString("Red"));
					qDebug("temp code: %i\n", tempCode);
				}
			}
			msgLength = payloadIndex + MSG_HEADER_SIZE;
			qDebug("msg length: %i\n", msgLength);
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		case MEASURE_MOISTURE_TAGS:
			qDebug("Received MEASURE MOISTURE TAGS\n");
			sprintf(msg, "Received MEASURE MOISTURE TAGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			controller->measureMoistureTags();
			numberOfTagsFound = model->MoistTagList.size();
			tagsSensorReadHistorySizeTotal = 0;
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				tagsSensorReadHistorySizeTotal += model->MoistTagList[i].SensorReadHistory.size();
			}
			*message = new char[MSG_HEADER_SIZE + 1 + numberOfTagsFound * (1 + 2) + tagsSensorReadHistorySizeTotal * (FREQ_LENGTH + ONCHIPRSSICODE_LENGTH + 
					SENSORCODE_LENGTH)]; 
			if(!((*message)))
			{
				qDebug("failed to allocate memory\n");
				*message = NULL;
				msgLength = 0;
				status = -1;
				break;
			}
			payloadIndex = 6;
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, numberOfTagsFound);
			sprintf(msg, "Number of tags found: %i\n", numberOfTagsFound);
			emit outputToConsole(QString(msg), QString("Red"));
			qDebug("Number of tags found: %i\n", numberOfTagsFound);
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				sprintf(msg, "Tag number: %i\n", i + 1);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("Tag number: %i\n", i + 1);
				short tagsDataLength = FREQ_LENGTH +
					ONCHIPRSSICODE_LENGTH +
					SENSORCODE_LENGTH;
				tagsSensorReadHistorySize = model->MoistTagList[i].SensorReadHistory.size();								     
				tagsDataLength *= tagsSensorReadHistorySize;
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tagsDataLength & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tagsDataLength >> 8 & 0xFF);
				sprintf(msg, "tag's data length: %i\n", tagsDataLength);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("tag's data length: %i\n", tagsDataLength);
				for(short c = 0; c < tagsSensorReadHistorySize; c++)
				{
					int freq = model->MoistTagList[i].SensorReadHistory[c].getFrequencyKHz();
					int onChipRssiCode = model->MoistTagList[i].SensorReadHistory[c].getOnChipRssiCode();
					int sensorCode = model->MoistTagList[i].SensorReadHistory[c].getSensorCode();
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, freq & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, freq >> 8 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, freq >> 16 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, freq >> 24 & 0xFF);
					sprintf(msg, "freq: %i\n", freq);
					emit outputToConsole(QString(msg), QString("Red"));
					qDebug("freq: %i\n", freq);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, onChipRssiCode & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, onChipRssiCode >> 8 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, onChipRssiCode >> 16 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, onChipRssiCode >> 24 & 0xFF);
					sprintf(msg, "on chip RSSI code: %i\n", onChipRssiCode);
					emit outputToConsole(QString(msg), QString("Red"));
					qDebug("on chip RSSI code: %i\n", onChipRssiCode);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, sensorCode & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, sensorCode >> 8 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, sensorCode >> 16 & 0xFF);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, sensorCode >> 24 & 0xFF);
					sprintf(msg, "sensor code: %i\n", sensorCode);
					emit outputToConsole(QString(msg), QString("Red"));
					qDebug("sensor code: %i\n", sensorCode);
				}
			}
			msgLength = payloadIndex + MSG_HEADER_SIZE;
			qDebug("msg length: %i\n", msgLength);
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		case GET_TEMP_DEMO_SETTINGS:
			qDebug("Received GET TEMP DEMO SETTINGS\n");
			sprintf(msg, "Received GET TEMP DEMO SETTINGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue")); 
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			size = MSG_HEADER_SIZE + sizeof(model->currentFreqBand) +
				sizeof(model->tempAutoPower) +
				sizeof(model->tempMaxPower) +
				sizeof(model->TempTargetOnChipRssiMin) +
				sizeof(model->TempTargetOnChipRssiMax) +
				sizeof(model->tempMinSamplesPerMeas);
			*message = new char[size]; 
			payloadIndex = 6;
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->currentFreqBand & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->currentFreqBand >> 8 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->currentFreqBand >> 16 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->currentFreqBand >> 24 & 0xFF);
			sprintf(msg, "Current freq band: %i\n", model->currentFreqBand);
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->tempAutoPower);
			sprintf(msg, "Temp Auto Power: %i\n", model->tempAutoPower);
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->getTempMaxPowerLevel());
			sprintf(msg, "Temp Max Power: %i\n", model->getTempMaxPowerLevel());
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->TempTargetOnChipRssiMin & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->TempTargetOnChipRssiMin >> 8 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->TempTargetOnChipRssiMin >> 16 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->TempTargetOnChipRssiMin >> 24 & 0xFF);
			sprintf(msg, "Temp Target On-Chip RSSI Min: %i\n", model->TempTargetOnChipRssiMin);
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->TempTargetOnChipRssiMax & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->TempTargetOnChipRssiMax >> 8 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->TempTargetOnChipRssiMax >> 16 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->TempTargetOnChipRssiMax >> 24 & 0xFF);
			sprintf(msg, "Temp Target On-Chip RSSI Max: %i\n", model->TempTargetOnChipRssiMax);
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->tempMinSamplesPerMeas & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->tempMinSamplesPerMeas >> 8 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->tempMinSamplesPerMeas >> 16 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->tempMinSamplesPerMeas >> 24 & 0xFF);
			sprintf(msg, "Temp Min Samples Per Meas: %i\n", model->tempMinSamplesPerMeas);
			emit outputToConsole(QString(msg), QString("Red"));
			msgLength = payloadIndex + MSG_HEADER_SIZE;
			qDebug("msg length: %i\n", msgLength);
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		case SET_TEMP_DEMO_SETTINGS:
			qDebug("Received SET TEMP DEMO SETTINGS\n");
			sprintf(msg, "Received SET TEMP DEMO SETTINGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			size = MSG_HEADER_SIZE + sizeof(model->currentFreqBand) +
				sizeof(model->tempAutoPower) +
				sizeof(model->tempMaxPower) +
				sizeof(model->TempTargetOnChipRssiMin) +
				sizeof(model->TempTargetOnChipRssiMax) +
				sizeof(model->tempMinSamplesPerMeas);
			if(payloadLength == (size - MSG_HEADER_SIZE) && payload != NULL)
			{
				payloadIndex = 0;
				currentFreqBand = 0;
				currentFreqBand += GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				currentFreqBand += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 8; 
				currentFreqBand += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 16;
				currentFreqBand += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 24;
				if(currentFreqBand >= 0 && currentFreqBand <= 5)
				{
					status = controller->setBandRegion((FreqBandEnum)currentFreqBand);
					if(status != 0)
					{
						controller->turnReaderOff();
						QThread::msleep(100);
						controller->turnReaderOn();
					}
				}
				else
					status = -1;
				sprintf(msg, "Current freq band: %i\n", model->currentFreqBand);
				emit outputToConsole(QString(msg), QString("Red"));
				tempAutoPower = GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				if(tempAutoPower >= 0 && tempAutoPower <= 1)
					controller->setTempAutoPower(tempAutoPower);
				else
					status = -1;
				sprintf(msg, "Temp Auto Power: %i\n", model->tempAutoPower);
				emit outputToConsole(QString(msg), QString("Red"));
				tempMaxPower = GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				if(tempMaxPower >= 18 && tempMaxPower <= 30)
				{
					controller->setTempMaxPowerLevel(tempMaxPower);
					sprintf(msg, "Temp Max Power: %i \n", tempMaxPower);
				}
				else
				{
					status = -1;
					sprintf(msg, "Temp Max Power: not changed\n");
				}
				emit outputToConsole(QString(msg), QString("Red"));
				TempTargetOnChipRssiMin = 0;
				TempTargetOnChipRssiMin += GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				TempTargetOnChipRssiMin += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 8; 
				TempTargetOnChipRssiMin += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 16;
				TempTargetOnChipRssiMin += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 24;
				
				TempTargetOnChipRssiMax = 0;
				TempTargetOnChipRssiMax += GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				TempTargetOnChipRssiMax += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 8; 
				TempTargetOnChipRssiMax += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 16;
				TempTargetOnChipRssiMax += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 24;
				if(TempTargetOnChipRssiMax >= 0 && TempTargetOnChipRssiMax <= 31 &&
				    TempTargetOnChipRssiMin >= 0 && TempTargetOnChipRssiMin <= 31 &&
				    TempTargetOnChipRssiMax > TempTargetOnChipRssiMin)
				{
					controller->setTempOnChipRssiTargetMax(TempTargetOnChipRssiMax);
					controller->setTempOnChipRssiTargetMin(TempTargetOnChipRssiMin);
				}
				else
					status = -1;
				sprintf(msg, "Temp Target On-Chip RSSI Min: %i\n", model->TempTargetOnChipRssiMin);
				emit outputToConsole(QString(msg), QString("Red"));
				sprintf(msg, "Temp Target On-Chip RSSI Max: %i\n", model->TempTargetOnChipRssiMax);
				emit outputToConsole(QString(msg), QString("Red"));
				tempMinSamplesPerMeas = 0;
				tempMinSamplesPerMeas += GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				tempMinSamplesPerMeas += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 8; 
				tempMinSamplesPerMeas += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 16;
				tempMinSamplesPerMeas += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 24;
				if(tempMinSamplesPerMeas == 2 || tempMinSamplesPerMeas == 5 || tempMinSamplesPerMeas == 10 || tempMinSamplesPerMeas == 20)
					controller->setTempSamplesPerMeasurement(QString::number(tempMinSamplesPerMeas));
				else
					status = -1;
				sprintf(msg, "Temp Min Samples Per Meas: %i\n", model->tempMinSamplesPerMeas);
				emit outputToConsole(QString(msg), QString("Red"));
				delete[] payload;
				payload = NULL;
				payloadLength = 0;
			}
			else
				status = -1;
			*message = new char[MSG_HEADER_SIZE];
			msgLength = MSG_HEADER_SIZE;
			qDebug("msg length: %i\n", msgLength);
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		case GET_MOISTURE_DEMO_SETTINGS:
			qDebug("Received GET MOISTURE DEMO SETTINGS\n");
			sprintf(msg, "Received GET MOISTURE DEMO SETTINGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue")); 
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			size = MSG_HEADER_SIZE + sizeof(model->currentFreqBand) +
				sizeof(model->moistAutoPower) +
				sizeof(model->moistMaxPower) +
				sizeof(model->MoistTargetOnChipRssiMin) +
				sizeof(model->MoistTargetOnChipRssiMax) +
				sizeof(model->moistMinSamplesPerMeas);
			*message = new char[size]; 
			payloadIndex = 6;
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->currentFreqBand & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->currentFreqBand >> 8 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->currentFreqBand >> 16 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->currentFreqBand >> 24 & 0xFF);
			sprintf(msg, "Current freq band: %i\n", model->currentFreqBand);
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->moistAutoPower);
			sprintf(msg, "Moist Auto Power: %i\n", model->moistAutoPower);
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->getMoistMaxPowerLevel());
			sprintf(msg, "Moist Max Power: %i\n", model->getMoistMaxPowerLevel());
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->MoistTargetOnChipRssiMin & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->MoistTargetOnChipRssiMin >> 8 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->MoistTargetOnChipRssiMin >> 16 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->MoistTargetOnChipRssiMin >> 24 & 0xFF);
			sprintf(msg, "Moist Target On-Chip RSSI Min: %i\n", model->MoistTargetOnChipRssiMin);
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->MoistTargetOnChipRssiMax & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->MoistTargetOnChipRssiMax >> 8 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->MoistTargetOnChipRssiMax >> 16 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->MoistTargetOnChipRssiMax >> 24 & 0xFF);
			sprintf(msg, "Moist Target On-Chip RSSI Max: %i\n", model->MoistTargetOnChipRssiMax);
			emit outputToConsole(QString(msg), QString("Red"));
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->moistMinSamplesPerMeas & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->moistMinSamplesPerMeas >> 8 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->moistMinSamplesPerMeas >> 16 & 0xFF);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, model->moistMinSamplesPerMeas >> 24 & 0xFF);
			sprintf(msg, "Moist Min Samples Per Meas: %i\n", model->moistMinSamplesPerMeas);
			emit outputToConsole(QString(msg), QString("Red"));
			msgLength = payloadIndex + MSG_HEADER_SIZE;
			qDebug("msg length: %i\n", msgLength);
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		case SET_MOISTURE_DEMO_SETTINGS:
			qDebug("Received SET MOISTURE DEMO SETTINGS\n");
			sprintf(msg, "Received SET MOISTURE DEMO SETTINGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			size = MSG_HEADER_SIZE + sizeof(model->currentFreqBand) +
				sizeof(model->moistAutoPower) +
				sizeof(model->moistMaxPower) +
				sizeof(model->MoistTargetOnChipRssiMin) +
				sizeof(model->MoistTargetOnChipRssiMax) +
				sizeof(model->moistMinSamplesPerMeas);
			if(payloadLength == (size - MSG_HEADER_SIZE) && payload != NULL)
			{
				payloadIndex = 0;
				currentFreqBand = 0;
				currentFreqBand += GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				currentFreqBand += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 8; 
				currentFreqBand += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 16;
				currentFreqBand += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 24;
				if(currentFreqBand >= 0 && currentFreqBand <= 5)
				{
					status = controller->setBandRegion((FreqBandEnum)currentFreqBand);
					if(status != 0)
					{
						controller->turnReaderOff();
						QThread::msleep(100);
						controller->turnReaderOn();
					}
				}
				else
					status = -1;
				sprintf(msg, "Current freq band: %i\n", model->currentFreqBand);
				emit outputToConsole(QString(msg), QString("Red"));
				moistAutoPower = GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				if(moistAutoPower >= 0 && moistAutoPower <= 1)
					controller->setMoistAutoPower(moistAutoPower);
				else
					status = -1;
				sprintf(msg, "Moist Auto Power: %i\n", model->moistAutoPower);
				emit outputToConsole(QString(msg), QString("Red"));
				moistMaxPower = GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				if(moistMaxPower >= 18 && moistMaxPower <= 30)
				{
					controller->setMoistMaxPowerLevel(moistMaxPower);
					sprintf(msg, "Moist Max Power: %i \n", moistMaxPower);
				}
				else
				{
					status = -1;
					sprintf(msg, "Moist Max Power: not changed\n");
				}
				emit outputToConsole(QString(msg), QString("Red"));
				MoistTargetOnChipRssiMin = 0;
				MoistTargetOnChipRssiMin += GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				MoistTargetOnChipRssiMin += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 8; 
				MoistTargetOnChipRssiMin += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 16;
				MoistTargetOnChipRssiMin += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 24;
				
				MoistTargetOnChipRssiMax = 0;
				MoistTargetOnChipRssiMax += GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				MoistTargetOnChipRssiMax += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 8; 
				MoistTargetOnChipRssiMax += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 16;
				MoistTargetOnChipRssiMax += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 24;
				if(MoistTargetOnChipRssiMax >= 0 && MoistTargetOnChipRssiMax <= 31 &&
				    MoistTargetOnChipRssiMin >= 0 && MoistTargetOnChipRssiMin <= 31 &&
				    MoistTargetOnChipRssiMax > MoistTargetOnChipRssiMin)
				{
					controller->setMoistOnChipRssiTargetMax(MoistTargetOnChipRssiMax);
					controller->setMoistOnChipRssiTargetMin(MoistTargetOnChipRssiMin);
				}
				else
					status = -1;
				sprintf(msg, "Moist Target On-Chip RSSI Min: %i\n", model->MoistTargetOnChipRssiMin);
				emit outputToConsole(QString(msg), QString("Red"));
				sprintf(msg, "Moist Target On-Chip RSSI Max: %i\n", model->MoistTargetOnChipRssiMax);
				emit outputToConsole(QString(msg), QString("Red"));
				moistMinSamplesPerMeas = 0;
				moistMinSamplesPerMeas += GET_MESSAGE_PAYLOAD(payload, payloadIndex++);
				moistMinSamplesPerMeas += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 8; 
				moistMinSamplesPerMeas += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 16;
				moistMinSamplesPerMeas += GET_MESSAGE_PAYLOAD(payload, payloadIndex++) << 24;
				if(moistMinSamplesPerMeas == 2 || moistMinSamplesPerMeas == 5 || moistMinSamplesPerMeas == 10 || moistMinSamplesPerMeas == 20)
					controller->setMoistSamplesPerMeasurement(QString::number(moistMinSamplesPerMeas));
				else
					status = -1;
				sprintf(msg, "Moist Min Samples Per Meas: %i\n", model->moistMinSamplesPerMeas);
				emit outputToConsole(QString(msg), QString("Red"));
				delete[] payload;
				payload = NULL;
				payloadLength = 0;
			}
			else
				status = -1;
			*message = new char[MSG_HEADER_SIZE];
			msgLength = MSG_HEADER_SIZE;
			qDebug("msg length: %i\n", msgLength);
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		default:
			status = -1;
			*message = NULL;
			msgLength = 0;
			break;
	}
	return status;
}
short RUIThread::processCANCommand(char command, char *payload, short &payloadLength, char **message, short &msgLength)
{
	short status;
	char msg[80];
	short numberOfTagsFound;
	short payloadIndex;
	short size;
	float tempValue;
	float sensorValue;
	float onChipRSSIValue;
	switch(command)
	{
		case SEARCH_FOR_TEMP_TAGS:
			qDebug("Received SEARCH FOR TEMP TAGS\n");
			sprintf(msg, "Received SEARCH FOR TEMP TAGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			controller->searchForTempTags(MAX_SEARCH_TIME);
			numberOfTagsFound = model->TempTagList.size();
			size = numberOfTagsFound * (1 * 8 +  	//EPC length packet
					11 * 8 + 	//EPC packets max
					1* 8 + 	//TID length packet
					4 * 8 + 	//TID packets max
					5 * 8) + 	//Data packets
				1 * 8;       //Done packet			
			qDebug("buffer size %i\n", size);
			*message = new char[size];
			if(!((*message)))
			{
				qDebug("failed to allocate memory\n");
				*message = NULL;
				msgLength = 0;
				status = -1;
				break;
			}
			payloadIndex = 0;
			sprintf(msg, "Number of tags found: %i\n", numberOfTagsFound);
			emit outputToConsole(QString(msg), QString("Red"));
			qDebug("Number of tags found: %i\n", numberOfTagsFound);
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				sprintf(msg, "Tag number: %i\n", i + 1);
				emit outputToConsole(QString(msg), QString("Red"));
				string epcStr = model->TempTagList[i].getEpc().toStdString();
				string tidStr = model->TempTagList[i].getTid().toStdString();
				short epcLength =  epcStr.length();
				short tidLength = tidStr.length();
				int tempCalC1 = model->TempTagList[i].getTempCalC1();
				float tempCalT1 = model->TempTagList[i].getTempCalT1();
				int tempCalC2 = model->TempTagList[i].getTempCalC2();
				float tempCalT2 = model->TempTagList[i].getTempCalT2();
				bool crcValid = model->TempTagList[i].getCrcValid(); 
				short tagsDataLength = epcLength + EPCLEN_LENGTH +
					tidLength + TIDLEN_LENGTH +
					TEMPCALC1_LENGTH +
					TEMPCALT1_LENGTH +
					TEMPCALC2_LENGTH +
					TEMPCALT2_LENGTH +
					CRCVALID_LENGTH;
				sprintf(msg, "tag's data length: %i\n", tagsDataLength);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, EPC_LEN_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "EPC length: %i\n", epcLength);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after epc len resp %i\n", payloadIndex);
				short epcCharsLoaded = 0;
				while(epcCharsLoaded < epcLength)
				{
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, EPC_RESP);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);  
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
				}
				qDebug("payload index after epc resp %i\n", payloadIndex);
				sprintf(msg, "EPC: 0x");
				char temp[2];
				temp[1] = '\0';
				for(short j = 0; j < epcLength; j++)
				{
					sprintf(temp, "%c", epcStr.at(j));
					strcat(msg, temp);
				}
				strcat(msg, "\n");
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TID_LEN_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "TID length: %i\n", tidLength);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after tid len resp %i\n", payloadIndex);
				short tidCharsLoaded = 0;
				while(tidCharsLoaded < tidLength)
				{
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TID_RESP);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);  
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
				}
				qDebug("payload index after tid resp %i\n", payloadIndex);
				sprintf(msg, "TID: 0x");
				for(short j = 0; j < tidLength; j++)
				{
					sprintf(temp, "%c", tidStr.at(j));
					strcat(msg, temp);
				}
				strcat(msg, "\n");
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_CAL_C1_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "temp cal C1: %i\n", tempCalC1);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after temp cal c1 resp %i\n", payloadIndex);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_CAL_T1_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "temp cal T1: %i\n", (int)tempCalT1);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after temp cal T1 resp %i\n", payloadIndex);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_CAL_C2_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "temp cal C2: %i\n", tempCalC2);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after temp cal c2 resp %i\n", payloadIndex);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_CAL_T2_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "temp cal T2: %i\n", (int)tempCalT2);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after temp cal T2 resp %i\n", payloadIndex);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, CRCVALID_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, crcValid);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "crc valid: %i\n", crcValid);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after crc valid resp %i\n", payloadIndex);
			}
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, DONE_RESP);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			sprintf(msg, "done\n");
			emit outputToConsole(QString(msg), QString("Red"));
			qDebug("payload index after done resp %i\n", payloadIndex);
			msgLength = payloadIndex;
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));			
			break;
		case SEARCH_FOR_MOISTURE_TAGS:
			qDebug("Received SEARCH FOR MOISTURE TAGS\n");
			sprintf(msg, "Received SEARCH FOR MOISTURE TAGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			controller->searchForMoistureTags(MAX_SEARCH_TIME);
			numberOfTagsFound = model->MoistTagList.size();
			size = numberOfTagsFound * (1 * 8 +  	//EPC length packet
					11 * 8 + 	//EPC packets max
					1* 8 + 	//TID length packet
					4 * 8 + 	//TID packets max
					5 * 8) + 	//Data packets
				1 * 8;       //Done packet			
			qDebug("buffer size %i\n", size);
			*message = new char[size];
			if(!((*message)))
			{
				qDebug("failed to allocate memory\n");
				*message = NULL;
				msgLength = 0;
				status = -1;
				break;
			}
			payloadIndex = 0;
			sprintf(msg, "Number of tags found: %i\n", numberOfTagsFound);
			emit outputToConsole(QString(msg), QString("Red"));
			qDebug("Number of tags found: %i\n", numberOfTagsFound);
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				sprintf(msg, "Tag number: %i\n", i + 1);
				emit outputToConsole(QString(msg), QString("Red"));
				string epcStr = model->MoistTagList[i].getEpc().toStdString();
				string tidStr = model->MoistTagList[i].getTid().toStdString();
				short epcLength =  epcStr.length();
				short tidLength = tidStr.length();
				int tempCalC1 = model->MoistTagList[i].getTempCalC1();
				float tempCalT1 = model->MoistTagList[i].getTempCalT1();
				int tempCalC2 = model->MoistTagList[i].getTempCalC2();
				float tempCalT2 = model->MoistTagList[i].getTempCalT2();
				bool crcValid = model->MoistTagList[i].getCrcValid(); 
				short tagsDataLength = epcLength + EPCLEN_LENGTH +
					tidLength + TIDLEN_LENGTH +
					TEMPCALC1_LENGTH +
					TEMPCALT1_LENGTH +
					TEMPCALC2_LENGTH +
					TEMPCALT2_LENGTH +
					CRCVALID_LENGTH;
				sprintf(msg, "tag's data length: %i\n", tagsDataLength);
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, EPC_LEN_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, epcLength >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "EPC length: %i\n", epcLength);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after epc len resp %i\n", payloadIndex);
				short epcCharsLoaded = 0;
				while(epcCharsLoaded < epcLength)
				{
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, EPC_RESP);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);  
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (epcCharsLoaded < epcLength) ? epcStr.at(epcCharsLoaded++) : 0);
				}
				qDebug("payload index after epc resp %i\n", payloadIndex);
				sprintf(msg, "EPC: 0x");
				char temp[2];
				temp[1] = '\0';
				for(short j = 0; j < epcLength; j++)
				{
					sprintf(temp, "%c", epcStr.at(j));
					strcat(msg, temp);
				}
				strcat(msg, "\n");
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TID_LEN_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tidLength >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "TID length: %i\n", tidLength);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after tid len resp %i\n", payloadIndex);
				short tidCharsLoaded = 0;
				while(tidCharsLoaded < tidLength)
				{
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TID_RESP);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);  
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
					SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (tidCharsLoaded < tidLength) ? tidStr.at(tidCharsLoaded++) : 0);
				}
				qDebug("payload index after tid resp %i\n", payloadIndex);
				sprintf(msg, "TID: 0x");
				for(short j = 0; j < tidLength; j++)
				{
					sprintf(temp, "%c", tidStr.at(j));
					strcat(msg, temp);
				}
				strcat(msg, "\n");
				emit outputToConsole(QString(msg), QString("Red"));
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_CAL_C1_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC1 >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "temp cal C1: %i\n", tempCalC1);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after temp cal c1 resp %i\n", payloadIndex);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_CAL_T1_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT1 >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "temp cal T1: %i\n", (int)tempCalT1);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after temp cal T1 resp %i\n", payloadIndex);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_CAL_C2_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, tempCalC2 >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "temp cal C2: %i\n", tempCalC2);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after temp cal c2 resp %i\n", payloadIndex);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_CAL_T2_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)tempCalT2 >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "temp cal T2: %i\n", (int)tempCalT2);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after temp cal T2 resp %i\n", payloadIndex);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, CRCVALID_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, crcValid);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "crc valid: %i\n", crcValid);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("payload index after crc valid resp %i\n", payloadIndex);
			}
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, DONE_RESP);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			sprintf(msg, "done\n");
			emit outputToConsole(QString(msg), QString("Red"));
			qDebug("payload index after done resp %i\n", payloadIndex);
			msgLength = payloadIndex;
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));			
			break;
		case MEASURE_TEMP_TAGS:
			qDebug("Received MEASURE TEMP TAGS\n");
			sprintf(msg, "Received MEASURE TEMP TAGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			controller->measureTempTags();
			numberOfTagsFound = model->TempTagList.size();
			*message = new char[numberOfTagsFound *(1 * 8 +  //Data packet
					1 * 8) + //Data packet
				1 * 8];    //Done packet	
			if(!((*message)))
			{
				qDebug("failed to allocate memory\n");
				*message = NULL;
				msgLength = 0;
				status = -1;
				break;
			}
			payloadIndex = 0;
			sprintf(msg, "Number of tags found: %i\n", numberOfTagsFound);
			emit outputToConsole(QString(msg), QString("Red"));
			qDebug("Number of tags found: %i\n", numberOfTagsFound);
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				sprintf(msg, "Tag number: %i\n", i + 1);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("Tag number: %i\n", i + 1);
				short tagsTempMeasHistorySize = model->TempTagList[i].TemperatureMeasurementHistory.size();
				qDebug("temp meas hist size %i\n", tagsTempMeasHistorySize);
				short tagsOnChipRssiMeasHistorySize = model->TempTagList[i].OnChipRssiMeasurementHistory.size();
				qDebug("on-chip rssi meas hist size %i\n", tagsOnChipRssiMeasHistorySize);
				if(tagsTempMeasHistorySize > 0)
					tempValue = model->TempTagList[i].TemperatureMeasurementHistory[tagsTempMeasHistorySize - 1].getValue();
				else
					tempValue = 0xFFFF;
				if(tagsOnChipRssiMeasHistorySize > 0)
					onChipRSSIValue = model->TempTagList[i].OnChipRssiMeasurementHistory[tagsOnChipRssiMeasHistorySize - 1].getValue();
				else
					onChipRSSIValue = 0xFFFF;
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, TEMP_VALUE_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(tempValue*10) & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(tempValue*10) >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(tempValue*10) >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(tempValue*10) >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "Temp value: %f\n", tempValue);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("Temp value: %f\n", tempValue);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, ONCHIPRSSI_VALUE_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(onChipRSSIValue*10) & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(onChipRSSIValue*10) >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(onChipRSSIValue*10) >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(onChipRSSIValue*10) >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "on chip RSSI value: %f\n", onChipRSSIValue);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("on chip RSSI value: %f\n", onChipRSSIValue);
			}
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, DONE_RESP);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			sprintf(msg, "done\n");
			emit outputToConsole(QString(msg), QString("Red"));
			msgLength = payloadIndex;
			qDebug("msg length: %i\n", msgLength);
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		case MEASURE_MOISTURE_TAGS:
			qDebug("Received MEASURE MOISTURE TAGS\n");
			sprintf(msg, "Received MEASURE MOISTURE TAGS CMD, processing...\n");
			emit outputToConsole(QString(msg), QString("Blue"));
			status = 0;
			if(payloadLength > 0 && payload != NULL)
			{
				//shouldn't be receive a payload for this command
				delete[] payload;
				payload = NULL;
			}
			controller->measureMoistureTags();
			numberOfTagsFound = model->MoistTagList.size();
			*message = new char[numberOfTagsFound *(1 * 8 +  //Data packet
					1 * 8) + //Data packet
				1 * 8];    //Done packet	
			if(!((*message)))
			{
				qDebug("failed to allocate memory\n");
				*message = NULL;
				msgLength = 0;
				status = -1;
				break;
			}
			payloadIndex = 0;
			sprintf(msg, "Number of tags found: %i\n", numberOfTagsFound);
			emit outputToConsole(QString(msg), QString("Red"));
			qDebug("Number of tags found: %i\n", numberOfTagsFound);
			for(short i = 0; i < numberOfTagsFound; i++)
			{
				sprintf(msg, "Tag number: %i\n", i + 1);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("Tag number: %i\n", i + 1);
				short tagsSensorMeasHistorySize = model->MoistTagList[i].SensorMeasurementHistory.size();
				qDebug("sensor meas hist size %i\n", tagsSensorMeasHistorySize);
				short tagsOnChipRssiMeasHistorySize = model->MoistTagList[i].OnChipRssiMeasurementHistory.size();
				qDebug("on-chip rssi meas hist size %i\n", tagsOnChipRssiMeasHistorySize);
				if(tagsSensorMeasHistorySize > 0)
					sensorValue = model->MoistTagList[i].SensorMeasurementHistory[tagsSensorMeasHistorySize - 1].getValue();
				else
					sensorValue = 0xFFFF;
				if(tagsOnChipRssiMeasHistorySize > 0)
					onChipRSSIValue = model->MoistTagList[i].OnChipRssiMeasurementHistory[tagsOnChipRssiMeasHistorySize - 1].getValue();
				else
					onChipRSSIValue = 0xFFFF;
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, SENSOR_VALUE_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(sensorValue*10) & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(sensorValue*10) >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(sensorValue*10) >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(sensorValue*10) >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "Sensor value: %f\n", sensorValue);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("Sensor value: %f\n", sensorValue);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, ONCHIPRSSI_VALUE_RESP);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, i + 1);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(onChipRSSIValue*10) & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(onChipRSSIValue*10) >> 8 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(onChipRSSIValue*10) >> 16 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, (int)(onChipRSSIValue*10) >> 24 & 0xFF);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
				sprintf(msg, "on chip RSSI value: %f\n", onChipRSSIValue);
				emit outputToConsole(QString(msg), QString("Red"));
				qDebug("on chip RSSI value: %f\n", onChipRSSIValue);
			}
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, DONE_RESP);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			SET_MESSAGE_PAYLOAD((*message), payloadIndex++, 0);
			sprintf(msg, "done\n");
			emit outputToConsole(QString(msg), QString("Red"));
			msgLength = payloadIndex;
			qDebug("msg length: %i\n", msgLength);
			sprintf(msg, "msg length: %i\n", msgLength);
			emit outputToConsole(QString(msg), QString("Red"));
			break;
		default:
			status = -1;
			*message = NULL;
			msgLength = 0;
			break;
	}
	return status;
}
float RUIThread::getTempOfATag()
{
	controller->searchForTempTags(MAX_SEARCH_TIME);
	short numberOfTagsFound = model->TempTagList.size();
	if(numberOfTagsFound > 0)
	{
		qDebug("found tags, measuring temperature of one...\n");
		controller->measureTempTags();
		short tagsTempMeasHistorySize = model->TempTagList[0].TemperatureMeasurementHistory.size();
		float tempValue;
		if(tagsTempMeasHistorySize > 0)
		{
			qDebug("measured temp!\n");
			tempValue = model->TempTagList[0].TemperatureMeasurementHistory[tagsTempMeasHistorySize - 1].getValue();
		}
		else
		{
			qDebug("could not measure temp!\n");
			tempValue = 0xE2;
		}
		return tempValue;
	}
	return 0xE3;
}
