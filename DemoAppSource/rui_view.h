/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.    
///
/// rui_view.h
/// This class implements the Remote User Interface of Hermes, which allows Hermes
/// to connect to an external device, through one generic interface, communicating
/// through UART, TCP, CAN, I2C, or SPI.  It's reponsible for configuring a 
/// Multiplexer (MUX) on the Hermes board to select to connect to the external 
/// device through UART, I2C or SPI.  It encapsulates the GPIOs, command-processing
/// thread, and driver resources needed to interface Hermes from an external device.
///
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _RUI_VIEW_H_
#define _RUI_VIEW_H_

#include "kit_model.h"
#include "kit_controller.h"
#include "rui_thread.h"
#include "interfaces.h"
#include "interface_enums.h"
#include "GPIO.h"

class RUIThread;

using namespace exploringBB;

class RUIView 
{
	private:
		KitModel *model;
		KitController *controller;
		RUIThread *thread;
		GPIO *gpio22;
		GPIO *gpio26;
		GPIO *gpio66;
		GPIO *gpio27;
	public:
		RUIView(KitController *controller, KitModel *model);
		short initialize();
		short setType(Interface::InterfaceType interface);
		void setMode(Interface::Mode mode);
		bool isRunning();
		short start();
		void stop();
		RUIThread* getThreadPointer();
		void configureBoardsMUXFor(Interface::InterfaceType interface);
};
#endif
