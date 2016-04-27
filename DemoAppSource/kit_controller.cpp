#include "kit_controller.h"
#include "kit_model.h"
#include "rui_view.h"
#include "interface_enums.h"
#include <QThread>
#include <stdio.h>
#include <stdlib.h>

KitController::KitController(KitModel *model, int &argc, char *argv[])
{
	this->model = model;
	model->initialize();
	app = new QApplication(argc, argv);
	QThread::currentThread()->setPriority(QThread::NormalPriority);
	app->setStyleSheet("QPushButton { min-width: 50px; min-height: 30px }"
			"QSpinbox::up-button { subcontrol-position: left; width: 50px; height: 30px }"
			"QSpinbox::down-button { subcontrol-position: right; width: 50px; height: 30px }"
			"QComboBox { min-width: 50px; min-height: 30px }");
	RUI = new RUIView((KitController *)this, model);
	GUI = new GUIView((KitController *)this, model);
}
int KitController::initialize()
{
	qDebug("KitController::initialize\n");
	GUI->initialize();
	RUI->initialize();
	return 0;
}
int KitController::run()
{
	int status;
	turnReaderOn();
	GUI->showSplashScreen();
	app->processEvents();
	GUI->showMsgOnSplashScreen("Initializing reader...");
	status=model->initializeReader();
	if (status!=0)
	{
		GUI->closeSplashScreen();
		app->processEvents();
		qDebug("Initialization error");
		QMessageBox msgBox;
		msgBox.setWindowTitle("Error");
		msgBox.setText("Reader initialization error. Please restart application.");
		msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
		msgBox.exec();
		QFile runningFile("running");
		if (runningFile.exists())
			runningFile.remove();
		turnReaderOff();
		exit(EXIT_FAILURE);
	}
	status=model->setFrequencyBand(FCC);
	if (status!=0)
	{
		GUI->closeSplashScreen();
		app->processEvents();
		qDebug("Error setting frequencies");
		QMessageBox msgBox;
		msgBox.setWindowTitle("Error");
		msgBox.setText("Error setting frequencies. Please restart application.");
		msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
		msgBox.exec();
		QFile runningFile("running");
		if (runningFile.exists())
			runningFile.remove();
		turnReaderOff();
		exit(EXIT_FAILURE);
	}
	GUI->closeSplashScreen();
	GUI->show();
	return app->exec();
}
short KitController::launchRUI(QString interfaceType)
{
	if(RUI->isRunning() == false)
	{
		qDebug("RUI thread not running\n");
		Interface::InterfaceType interface;
		if(interfaceType == "UART")
			interface = Interface::UART;
		else if(interfaceType == "TCP")
			interface = Interface::TCP;
		else if(interfaceType == "CAN")
			interface = Interface::CAN;
		else if(interfaceType == "I2C")
			interface = Interface::I2C;
		else if(interfaceType == "SPI")
			interface = Interface::SPI;
		else if(interfaceType == "ZIGBEE")
			interface = Interface::ZIGBEE;
		else
		{
			qDebug("invalid interface provided\n");
			return -1;
		}
		if(RUI->setType(interface) == 0)
		{
			qDebug("set RUI type ok\n");
			RUI->setMode(Interface::NORMAL);
			qDebug("set RUI mode ok\n");
			RUI->start();
		}
		else
		{
			qDebug("could not set interface\n");
			return -1;
		}
	}
	else
	{
		qDebug("interface already running\n");
		return -1;
	}
	return 0;
}
void KitController::stopRUI()
{
	RUI->stop();
	qDebug("RUI thread stopping ok\n");
}
RUIThread* KitController::getRUIThreadPointer()
{
	return RUI->getThreadPointer();	
}
void KitController::turnReaderOn()
{
	model->turnReaderOn();
}
void KitController::turnReaderOff()
{
	model->turnReaderOff();
}
int KitController::clearTempTags()
{
	model->clearTags("Temperature");
	return 0;
}
int KitController::clearMoistTags()
{
	model->clearTags("Moisture");
	return 0;
}
double KitController::measureTempCodeForCalibration()
{
	return model->measureTempCodeForCalibration();
}
void KitController::searchForTempTags()
{
	model->searchForTempTags();
}
void KitController::searchForMoistureTags()
{
	model->searchForMoistTags();
}
void KitController::searchForTempTags(int maxSearchTime)
{
	model->searchForTempTags(maxSearchTime);
}
void KitController::searchForMoistureTags(int maxSearchTime)
{
	model->searchForMoistTags(maxSearchTime);
}
void KitController::measureTempTags()
{
	model->measureTempTags();
}
void KitController::measureMoistureTags()
{
	model->measureMoistTags();
}
int KitController::setMoistLinearFit(bool setting)
{
	if (setting==true)
		model->moistLinearRegression=true;
	else
		model->moistLinearRegression=false;
	return 0;
}
int KitController::setTempAutoPower(bool setting)
{
	model->tempAutoPower = setting;
	return 0;
}
int KitController::setMoistAutoPower(bool setting)
{
	model->moistAutoPower = setting;
	return 0;
}
int KitController::setTempMaxPowerLevel(int dBm)
{
	int powerCode;
	if (dBm <= 28)
		powerCode=37-dBm;
	else
		powerCode=36-dBm;
	model->tempMaxPower = powerCode;
	return 0;
}
int KitController::setMoistMaxPowerLevel(int dBm)
{
	int powerCode;
	if (dBm <= 28)
		powerCode=37-dBm;
	else
		powerCode=36-dBm;
	model->moistMaxPower = powerCode;
	return 0;
}
int KitController::setTempOnChipRssiTargetMin(int t)
{
	model->TempTargetOnChipRssiMin=t;
	return 0;
}
int KitController::setTempOnChipRssiTargetMax(int t)
{
	model->TempTargetOnChipRssiMax=t;
	return 0;
}
int KitController::setMoistOnChipRssiTargetMin(int t)
{
	model->MoistTargetOnChipRssiMin=t;
	return 0;
}
int KitController::setMoistOnChipRssiTargetMax(int t)
{
	model->MoistTargetOnChipRssiMax=t;
	return 0;
}
int KitController::setBandRegion(FreqBandEnum band)
{
	int status = model->setFrequencyBand(band);
	return status;
}
int KitController::setTempSamplesPerMeasurement(QString s)
{
	int samples = s.toInt();
	model->tempMinSamplesPerMeas=samples;
	return 0;
}
int KitController::setMoistSamplesPerMeasurement(QString s)
{
	int samples = s.toInt();
	model->moistMinSamplesPerMeas=samples;
	return 0;
}
int KitController::setMoistThreshold(int threshold)
{
	model->moistThreshold=threshold;
	qDebug("Moist threshold set to: %d", model->moistThreshold);
	return 0;
}
int KitController::setMoistAboveThreshold(bool above)
{
	if (above)
	{
		model->wetAbove=true;
		qDebug("wetAbove: true");
	}
	else
	{
		model->wetAbove=false;
		qDebug("wetAbove: false");
	}
	return 0;
}
