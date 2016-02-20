/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// hermes.h
/// This class implements a wizard based on Qt's QWizard class, which is the 
/// framework for this (Hermes) demonstration software.  It creates a Hermes
/// wizard object and lays out its Main, Temperature demo, Moisture demo, and Remote
/// Operation demo pages and populates those pages with frames, buttons, plots, etc.
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef HERMES_H
#define HERMES_H

#include <QWizard>
#include <QLCDNumber>
#include <qlabel.h>
#include <qlayout.h>
#include <QComboBox>
#include <qwt_wheel.h>
#include <qwt_thermo.h>
#include <qwt_scale_engine.h>
#include <qwt_transform.h>
#include <qwt_color_map.h>
#include <QTextEdit>
#include "configdialog.h"
#include "chart.h"
#include "kit_controller.h"
#include "kit_model.h"
#include "chart_thread.h"

class Chart; 
class ChartThread;
class ConfigKitDialog;
class ConfigTempDemoDialog;
class ConfigMoistureDemoDialog;
class MeasurementDetailsDialog;
class HelpDialog;
class CalibrationDialog;
class TempDemoPage;
class MoistureDemoPage;
class RemoteOperationPage;
class QProgressDialog;
QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

class Hermes : public QWizard
{
	Q_OBJECT
	private:
		KitController *controller;
		KitModel *model;
	public:
		enum { Page_Main, Page_TempDemo, Page_MoistureDemo, Page_RemoteOperation };
		TempDemoPage *tempDemoPage;
		MoistureDemoPage *moistDemoPage;
		RemoteOperationPage *remoteOperationPage;
		Hermes(KitModel *model, KitController *controller, QWidget *parent = 0);
	private slots:
		void quitButtonClicked();
};
class MainPage : public QWizardPage
{
	Q_OBJECT
	public:
		MainPage(KitModel *model, KitController *controller, QWidget *parent = 0);
		int nextId() const;
	private:
		QString buttonClicked;
		KitModel *model;
		QProgressDialog *tuningProgress;
		QPushButton *tempDemoButton;
		QPushButton *moistureDemoButton;
		QPushButton *remoteOperationButton;
	private slots:
		void tempDemoButtonClicked();
		void moistureDemoButtonClicked();
		void remoteOperationButtonClicked();
};
class TempDemoPage : public QWizardPage
{
	Q_OBJECT
	public:
		TempDemoPage(KitModel *model, KitController *controller, QWidget *parent = 0);
		bool abortRequested();
		CalibrationDialog *calibrationDialog;
	private:
		KitModel *model;
		KitController *controller;
		ChartThread *thread;
		ConfigTempDemoDialog *configTempDemoDialog;
		MeasurementDetailsDialog *measurementDetailsDialog;
		HelpDialog *helpDialog;
		QPushButton *measurementDetailsButton;
		QPushButton *mainScreenButton;
		QPushButton *helpButton;
		QPushButton *configButton;
		QPushButton *calibrationButton;
		QPushButton *startButton;
		QPushButton *stopButton;
		QPushButton *clearButton;
		QPushButton *exportButton;
		QLabel *periodLabel;
		QComboBox *periodCombo;
		Chart *plot;
		QList<QLabel*> outputLabels;
		QLabel *logoLabel;
		QMovie *movie;
		QVBoxLayout *plotLayout;
	public slots:
		void updateTagSelectionsSlot();
		void updateTempOutputLabelsSlot(QList<SensorTag>);
	private slots:
		void measurementDetailsButtonClicked();
		void mainScreenButtonClicked();
		void helpButtonClicked();
		void configButtonClicked();
		void calibrationButtonClicked();
		void periodChanged();
		void startButtonClicked();
		void stopButtonClicked();
		void clearButtonClicked();
		void exportButtonClicked();
};
class MoistureDemoPage : public QWizardPage
{
	Q_OBJECT
	public:
		MoistureDemoPage(KitModel *model, KitController *controller, QWidget *parent = 0);
		bool abortRequested();
	private:
		KitModel *model;
		KitController *controller;
		ChartThread *thread;
		ConfigMoistureDemoDialog *configMoistureDemoDialog;
		HelpDialog *helpDialog;
		MeasurementDetailsDialog *measurementDetailsDialog;
		CalibrationDialog *calibrationDialog;
		QPushButton *measurementDetailsButton;
		QPushButton *mainScreenButton;
		QPushButton *helpButton;
		QPushButton *configButton;
		QPushButton *calibrationButton;
		QPushButton *startButton;
		QPushButton *stopButton;
		QPushButton *clearButton;
		QPushButton *exportButton;
		QLabel *periodLabel;
		QComboBox *periodCombo;
		Chart *plot;
		QList<QLabel*> outputLabels;
		QLabel *logoLabel;
		QMovie *movie;
		QVBoxLayout *plotLayout;
	public slots:
		void updateTagSelectionsSlot();
		void updateMoistOutputLabelsSlot(QList<SensorTag>);
	private slots:
		void measurementDetailsButtonClicked();
		void mainScreenButtonClicked();
		void helpButtonClicked();
		void configButtonClicked();
		void periodChanged();
		void startButtonClicked();
		void stopButtonClicked();
		void clearButtonClicked();
		void exportButtonClicked();
};
class RemoteOperationPage : public QWizardPage
{
	Q_OBJECT
	public:
		RemoteOperationPage(KitModel *model, KitController *controller, QWidget *parent = 0);
	private:
		KitModel *model;
		KitController *controller;
		QLabel *interfaceTypeLabel;
		QComboBox *interfaceTypeCombo;
		QPushButton *startButton;
		QPushButton *stopButton;
		QPushButton *helpButton;
		QPushButton *mainButton;
		QTextEdit *console;
		HelpDialog *helpDialog;
	public slots:
		void outputToConsole(QString text, QString color);
	private slots:
		void startButtonClicked();
		void stopButtonClicked();
		void helpButtonClicked();
		void mainButtonClicked();
};
#endif
