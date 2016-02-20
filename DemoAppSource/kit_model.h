/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// kit_model.h
/// This class is responsible for the actual control of the AMS Radon reader and 
/// for maintaining its state (settings and results of reads).  It receives
/// requests from the Controller module to set settings of the reader and instruct
/// the reader to perform different operations.  It also receives requests from
/// the GUI and RUI modules to provide data (settings and results of reads) for the
/// user.
///
/// 
/// Author: Frank Miranda and Greg Pitner, RFMicron
///-----------------------------------------------------------------------------

#ifndef _KIT_MODEL_H_
#define _KIT_MODEL_H_

#include <QtAlgorithms>
#include "gui_view.h"
#include "kit_controller.h"
#include "sensorTag.h"
#include "ams_radon_reader.h"
#include "GPIO.h"
#include <QFile>

#define NUMBER_OF_TEMP_INVENTORIES 50

class GUIView;
class QFile;

using namespace exploringBB;

class KitModel : public QObject
{
	Q_OBJECT
	private:
		GUIView *tempObserver;
		GUIView *moistureObserver;
		AMSRadonReader *reader;
		int FCCBandFreqs[50];
		int ETSIBandFreqs[4];
		int PRCBandFreqs[16];
		int JPNBandFreqs[6];
		GPIO *gpio7;
	public:
		KitModel();
		FreqBandEnum currentFreqBand;
		bool tempAutoPower;
		bool moistAutoPower;
		char tempMaxPower;
		char moistMaxPower;
		char txPower;
		int TempTargetOnChipRssiMin;
		int TempTargetOnChipRssiMax;
		int MoistTargetOnChipRssiMin;
		int MoistTargetOnChipRssiMax;
		int tempMinSamplesPerMeas;
		int moistMinSamplesPerMeas;
		bool moistLinearRegression;
		int centerFrequency;
		int moistThreshold;
		bool wetAbove;
		QList<QDateTime> TempMeasTimeList; // List of timestamps for each measurement completion since the last clear
		QList<QDateTime> MoistMeasTimeList;
		QList<SensorTag> TempTagList;
		QList<SensorTag> MoistTagList;
		void initialize();
		void turnReaderOn();
		void turnReaderOff();
		void debugPrintTagLists();
		void performInventoriesCheck();
		void selectForMeasurement(QString measurementType, QString tagLabel, bool select);
		int initializeReader();
		int setPower(char value);
		int setFrequencyBand(FreqBandEnum band);
		void registerTemperatureObserver(GUIView *gui);
		void registerMoistureObserver(GUIView *gui);
		void addTagToList(TagData tag, QString measurementType);
		float calculateTemperature(QString tagEpc, int minOnChipRssi, int maxOnChipRssi, int minNumberSuccessfulReads);
		void addSensorReading(TagData tag, QString measurementType);
		int findTags(int numInventories, QString measurementType);
		int searchForTempTags();
		int searchForMoistTags();
		int setSelectsForReading();
		int writeDataToTag(QString epc, char bankCode, int address, QString dataHexString);
		int readTags(int numInventories, QString measurementType);
		bool validTempTagsInList();
		bool validMoistTagsInList();
		bool findOptimumPower(int tagIndex, QString measurementType);
		int autotune(int c);
		int measureTempTags();
		int measureMoistTags();
		double measureTempCodeForCalibration();
		void clearTags(QString measurementType);
		int selectedTempTagIndex();
		int numberOfSelectedTempTags();
		bool tagInTempTagList(QString epc);
		bool tagInMoistTagList(QString epc);
		int exportMoistLog(QFile *file);
		int exportTempLog(QFile *file);
		void continuousWave(char timeInSeconds);
	signals:
		void updateTempTagsSignal(QList<SensorTag>);
		void updateTempTagSelectionsSignal();
		void updateMoistTagsSignal(QList<SensorTag>);
		void updateMoistTagSelectionsSignal();
		void antennaTuningSignal(int, int);
		void bandChangedSignal(FreqBandEnum);
};
#endif
