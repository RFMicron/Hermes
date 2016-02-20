/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// pages.h
/// This class implements the pages of the Details, Settings, Calibration, and 
/// Help dialogs launched from the Temperature and Moisture demo screens.
/// It lays out the pages and populates them with buttons, frames, tabs, etc.
/// It also is responsible for hanlding the button clicks and interfacing with the
/// Controller and Model modules to carry out user requests.
///
/// 
/// Author: Frank Miranda and Greg Pitner, RFMicron
///-----------------------------------------------------------------------------

#ifndef PAGES_H
#define PAGES_H

#include <QWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <qlabel.h>
#include <qlayout.h>
#include <qwt_wheel.h>
#include <qwt_thermo.h>
#include <qwt_scale_engine.h>
#include <qwt_transform.h>
#include <qwt_color_map.h>
#include <QDialog>
#include <QSpinBox>
#include "kit_model.h"
#include "kit_controller.h"

class KitModel;
class KitController;
class SensorTag;
class ChartThread;
class QProgressDialog;

class RemoteOpsSettingsPage : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
	public:
		RemoteOpsSettingsPage(KitModel *model, KitController *controller, QWidget *parent = 0);
};
class MeasurementDetailsPage : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
		QString demonstrationType;
		QTableWidget *selectTable;
	public:
		MeasurementDetailsPage(KitModel *model, KitController *controller, QString demoType);
	public slots:
		void loadTempTableSlot(QList<SensorTag>);
		void loadMoistTableSlot(QList<SensorTag>);
};
class OnePointTempCalTab : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
		ChartThread *thread;
		QTextEdit *temp1TextEdit;
		QTextEdit *code1TextEdit;
	public:
		explicit OnePointTempCalTab(KitModel *model, KitController *controller, QWidget *parent = 0);
		bool abortRequested();
	private slots:
		void readTempCodeButtonClicked();
		void writeCalButtonClicked();
	public slots:
		void tempCodeMeasuredSlot(float tempCode);
};
class TwoPointTempCalTab : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
		ChartThread *thread;
		QTextEdit *temp1TextEdit;
		QTextEdit *code1TextEdit;
		QTextEdit *temp2TextEdit;
		QTextEdit *code2TextEdit;
		bool readingCode1;
	public:
		explicit TwoPointTempCalTab(KitModel *model, KitController *controller, QWidget *parent = 0);
		bool abortRequested();
	private slots:
		void readTempCode1ButtonClicked();
		void readTempCode2ButtonClicked();
		void writeCalButtonClicked();
	public slots:
		void tempCodeMeasuredSlot(float tempCode);
};
class CalibrationPage : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
	public:
		CalibrationPage(KitModel *model, KitController *controller, QString demoType, QWidget *parent = 0);
		OnePointTempCalTab *onePointTab;
};
class HelpPage : public QWidget
{
	Q_OBJECT
	public:
		HelpPage(KitModel *model, KitController *controller, QString helpType);
};
class TempDemoConfigPage : public QWidget
{
	Q_OBJECT
	public:
		TempDemoConfigPage(KitModel *model, KitController *controller, QWidget *parent = 0);
};
class MoistureDemoConfigPage : public QWidget
{
	Q_OBJECT
	public:
		MoistureDemoConfigPage(KitModel *model, KitController *controller, QWidget *parent = 0);
};
class TempDemoOtherSettingsTab : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
		QComboBox *bandRegionCombo;
		QProgressDialog *tuningProgress;
		bool tuningTransmitter;
	public:
		explicit TempDemoOtherSettingsTab(KitModel *model, KitController *controller, QWidget *parent = 0);
	private slots:
		void bandRegionChanged(QString region);
		void samplesPerMeasurementChanged(QString samples);
	public slots:
		void bandChangedSlot(FreqBandEnum band);
		void antennaTuningSlot(int, int);
};
class TempDemoReaderPowerTab : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
	public:
		explicit TempDemoReaderPowerTab(KitModel *model, KitController *controller, QWidget *parent = 0);
	private Q_SLOTS:
		void setNum(double v);
		void autoPowerStateChanged(int);
		void minPowerTargetChanged(int);
		void maxPowerTargetChanged(int);
	private:
		QLabel *readerPowerLevelThermoLabel;
		QwtThermo *d_thermo;
		QwtWheel *d_wheel;
		QSpinBox *minMeasPowerTargetRangeSpinBox;
		QSpinBox *maxMeasPowerTargetRangeSpinBox;
};
class MoistureDemoOtherSettingsTab : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
		QComboBox *bandRegionCombo;
		QProgressDialog *tuningProgress;
		bool tuningTransmitter;
	public:
		explicit MoistureDemoOtherSettingsTab(KitModel *model, KitController *controller, QWidget *parent = 0);
	private slots:
		void dataReductionChanged(QString method);
		void bandRegionChanged(QString region);
		void samplesPerMeasurementChanged(QString samples);
		void wetThresholdChanged(int threshold);
		void wetThresholdDirectionChanged(QString direction);
	public slots:
		void bandChangedSlot(FreqBandEnum band);
		void antennaTuningSlot(int, int);
};
class MoistureDemoReaderPowerTab : public QWidget
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller; 
	public:
		explicit MoistureDemoReaderPowerTab(KitModel *model, KitController *controller, QWidget *parent = 0);
	private Q_SLOTS:
		void setNum(double v);
		void autoPowerStateChanged(int);
		void minPowerTargetChanged(int);
		void maxPowerTargetChanged(int);
	private:
		QLabel *readerPowerLevelThermoLabel;
		QwtThermo *d_thermo;
		QwtWheel *d_wheel;
		QSpinBox *minMeasPowerTargetRangeSpinBox;
		QSpinBox *maxMeasPowerTargetRangeSpinBox;
};
#endif
