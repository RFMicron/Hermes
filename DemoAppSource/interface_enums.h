#ifndef _INTERFACE_ENUMS_H_
#define _INTERFACE_ENUMS_H_
namespace Interface
{
	enum InterfaceType {NO_INTERFACE_SET, UART, TCP, CAN, I2C, SPI, ZIGBEE};
	enum Mode {NORMAL = 1, TEST };
}	
#endif
