/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// rui_thread.h
/// This class implements a thread that continuously processes commands received 
/// from an external device connected to Hermes to offload that processing from the
/// GUI thread.  It makes use of the generic interface implemented by the Interfaces 
/// class to receive commands through UART, TCP, CAN, I2C or SPI. The processing
/// involves getting the command, decoding it, executing it through the Model module,
/// building a response and sending that response through the generic interface.
///
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _RUI_THREAD_H_
#define _RUI_THREAD_H_

#include "kit_controller.h"
#include "kit_model.h"
#include "interfaces.h"
#include "interface_enums.h"
#include <QString>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class KitController;
class KitModel;

class RUIThread : public QThread
{
	Q_OBJECT
	private:
		Interfaces interface;
		KitController *controller;
		KitModel *model;
		QMutex mutex;
		QWaitCondition condition;
		bool abort;
		Interface::Mode mode;
	protected:
		void run() Q_DECL_OVERRIDE;
	signals:
		void outputToConsole(QString text, QString color);
	public:
		RUIThread(QObject *parent = 0);
		void initialize(KitController *controller, KitModel *model);
		short startInterface();
		void stopInterface();
		short processCommand(char command, char *payload, short &payloadLength, char **message, short &msgLength);
		short processCANCommand(char command, char *payload, short &payloadLength, char **message, short &msgLength);
		short setType(Interface::InterfaceType interface);
		short setMode(Interface::Mode mode);
}; 
#endif 
