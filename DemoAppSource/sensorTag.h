/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.    
///
/// sensorTag.h
/// This file implements classes for storing and processing data retrieved
/// from sensor tags.
///
/// SensorRead is a data structure representing a single read from a
/// single tag.
///
/// The SensorMeasurement class represents a higher-level concept than a
/// SensorRead. A SensorMeasurement value will typically be an average of
/// multiple reads. In the case of temperature measurements, the value will
/// be a calibrated measurement in degrees.
///
/// The SensorTag class stores and processes data for a single tag.
///
/// Author: Greg Pitner, RFMicron
///-----------------------------------------------------------------------------

#ifndef SENSORTAG_H
#define SENSORTAG_H

#include <QApplication>
#include <QDateTime>
 
class SensorRead
{
	private:
		int FrequencyKHz;
		char ReadPower;
		int SensorCode;
		int OnChipRssiCode;
		int TemperatureCode;
	public:
		SensorRead();
		SensorRead(int freq, int power, int sensor, int onChipRssi, int temp);
		void setFrequencyKHz(int freq);
		void setReadPower(int power);
		void setSensorCode(int sensor);
		void setOnChipRssiCode(int rssi);
		void setTemperatureCode(int temp);
		int getFrequencyKHz();
		int getReadPower();
		int getSensorCode();
		int getOnChipRssiCode();
		int getTemperatureCode();
};
class SensorMeasurement
{
	private:
		float Value;
		int ValidPowerReadCount;
		int InvalidPowerReadCount;
		int ReadPowerCode;
		int Number;  // Is the number of the measurement since the last Clear
		QDateTime TimeStamp;
	public:
		SensorMeasurement();
		void setValue(float v);
		void setValidPowerReadCount(int c);
		void setInvalidPowerReadCount(int c);
		void setReadPowerCode(int p);
		void setTime();
		void setNumber(int n);
		float getValue();
		int getValidPowerReadCount();
		int getInvalidPowerReadCount();
		int getReadPowerCode();
		int getNumber();
		QDateTime getFullTimeStamp();
		QString getTimeString();
};
class SensorTag
{
	private:
		QString Epc;
		QString Tid;
		int UserMemory[12];
		int TempCalC1;
		float TempCalT1;
		int TempCalC2;
		float TempCalT2;
		bool CrcValid; 
		int ReadHistoryMaxLength;
		int MeasurementHistoryMaxLength;
	public:
		QList<SensorRead> SensorReadHistory;
		QList<SensorMeasurement> SensorMeasurementHistory;
		QList<SensorMeasurement> TemperatureMeasurementHistory;
		QList<SensorMeasurement> OnChipRssiMeasurementHistory;
		QString Label;
		bool SelectedForMeasurement;
		SensorTag();
		void setEpc(QString epc);
		void setTid(QString tid);
		void setUserMemory(int addr, int data);	
		void setTempCalC1(int c1);
		void setTempCalT1(float t1);
		void setTempCalC2(int c2);
		void setTempCalT2(float t2);
		void setReadHistoryMaxLength(int l);
		void addSensorRead(SensorRead r);
		void clearSensorReads();
		void addSensorMeasurement(SensorMeasurement m);
		void addTemperatureMeasurement(SensorMeasurement m);
		void addOnChipRssiMeasurement(SensorMeasurement m);
		void clearMeasurementHistory();
		QString getEpc();
		QString getTid();
		int getUserMemory(int addr);  	
		int getTempCalC1();
		float getTempCalT1();
		int getTempCalC2();
		float getTempCalT2();
		bool getCrcValid();
		QString getModel();
		int getLastOnChipRssiReading();
		float tempCodeToDegC(float tempCode);
		float linearFitSensorCode(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int freqKHz, int &validCount);
		float avgSensorCode(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int &validCount);
		float avgOnChipRssiCode(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int &validCount);
		float calculateTemperature(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int &validCount);
		float calculateTempCode(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int &validCount);
		void decodeTemperatureCalWords();
		QString calculateTempCal2Point(float c1, float t1, float c2, float t2);
		QString calculateTempCal1Point(float c1, float t1);
};
#endif
