/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// kit_controller.h
/// This class is reponsible for handling user request from the GUI or RUI.
/// It is responsible for controlling the AMS Radon reader through the commands
/// available through the Model module.
///
/// 
/// Author: Frank Miranda and Greg Pitner, RFMicron
///-----------------------------------------------------------------------------

#ifndef _KIT_CONTROLLER_H_
#define _KIT_CONTROLLER_H_

enum FreqBandEnum {FCC, ETSI, PRC, JAPAN, FCC_center, ETSI_center};

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include "kit_model.h"
#include "rui_thread.h"
#include "rui_view.h"
#include "sensorTag.h"

#define APPLICATION_ICON ":/images/RFMAppLogo.png"
#define SPLASH_SCREEN_ICON ":/images/RFMSplashScreenLogo.bmp"
#define ANIMATED_ICON ":/images/RFMAnimatedLogo.gif"
#define MAIN_PAGE_ICON ":/images/RFMMainPageLogo.bmp"

class KitModel;
class GUIView;
class RUIView;
class RUIThread;

class KitController
{
	private:
		KitModel *model;
		GUIView *GUI;
		RUIView *RUI;
		QApplication *app;
	public:
		KitController(KitModel *model, int &argc, char *argv[]);
		int initialize();
		int run();
		short launchRUI(QString interfaceType);
		void stopRUI();
		RUIThread* getRUIThreadPointer();
		void turnReaderOn();
		void turnReaderOff();
		double measureTempCodeForCalibration();
		void searchForTempTags();
		void searchForMoistureTags();
		void searchForTempTags(int maxSearchTime);
		void searchForMoistureTags(int maxSearchTime);
		void measureTempTags();
		void measureMoistureTags();
		int clearTempTags();
		int clearMoistTags();
		int setMoistLinearFit(bool setting);
		int setTempAutoPower(bool setting);
		int setMoistAutoPower(bool setting);
		int setTempMaxPowerLevel(int dBm);
		int setMoistMaxPowerLevel(int dBm);
		int setTempSamplesPerMeasurement(QString s);
		int setMoistSamplesPerMeasurement(QString s);
		int setTempOnChipRssiTargetMin(int t);
		int setTempOnChipRssiTargetMax(int t);
		int setMoistOnChipRssiTargetMin(int t);
		int setMoistOnChipRssiTargetMax(int t);
		int setMoistThreshold(int threshold);
		int setMoistAboveThreshold(bool above);
		int setBandRegion(FreqBandEnum band);
};
#endif
