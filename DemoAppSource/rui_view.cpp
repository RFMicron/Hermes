#include "rui_view.h"
#include "rui_thread.h"

#define ERROR_NO_INTERFACE_SET -1

RUIView::RUIView(KitController *controller, KitModel *model)
{
	this->controller = controller;
	this->model = model;
	thread = new RUIThread;
	gpio22 = new GPIO(22);
	gpio26 = new GPIO(26);
	gpio66 = new GPIO(66);
	gpio27 = new GPIO(27);
}
short RUIView::initialize()
{
	thread->initialize(controller, model);
	gpio22->setDirection(GPIO::OUTPUT);
	gpio26->setDirection(GPIO::OUTPUT);
	gpio66->setDirection(GPIO::OUTPUT);
	gpio27->setDirection(GPIO::OUTPUT);
	return 0;
}
short RUIView::setType(Interface::InterfaceType interface)
{
	if(interface == Interface::I2C ||
			interface == Interface::SPI ||
			interface == Interface::UART)
		configureBoardsMUXFor(interface);		
	return thread->setType(interface);
}
RUIThread* RUIView::getThreadPointer()
{
	return thread;
}
void RUIView::configureBoardsMUXFor(Interface::InterfaceType interface)
{
	if(interface == Interface::I2C || interface == Interface::SPI)
	{
		gpio22->setValue(GPIO::HIGH);
		gpio27->setValue(GPIO::LOW);   //MUX_2
		gpio26->setValue(GPIO::HIGH);  //MUX_1
		gpio66->setValue(GPIO::HIGH);  //MUX_0
	}
	else if(interface == Interface::UART)
	{
		gpio22->setValue(GPIO::HIGH);
		gpio27->setValue(GPIO::LOW);   //MUX_2
		gpio26->setValue(GPIO::HIGH);  //MUX_1
		gpio66->setValue(GPIO::LOW);  //MUX_0
	}
	else if(interface == Interface::NO_INTERFACE_SET)
	{
		gpio22->setValue(GPIO::HIGH);
		gpio27->setValue(GPIO::LOW);   //MUX_2
		gpio26->setValue(GPIO::LOW);  //MUX_1
		gpio66->setValue(GPIO::LOW);  //MUX_0
	}
}
void RUIView::setMode(Interface::Mode mode)
{
	thread->setMode(mode);
}
short RUIView::start()
{
	return thread->startInterface();
}
void RUIView::stop()
{
	thread->stopInterface();
}
bool RUIView::isRunning()
{
	return thread->isRunning();	
}
