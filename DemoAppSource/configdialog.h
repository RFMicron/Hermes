/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// configdialog.h
/// These classes implement the dialog windows for the Details, Configuration,
/// Calibration, and Help buttons on the Temperature and Moisture demos.
/// They layout the dialogs and populate them with pages, buttons, frames, etc.
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "kit_model.h"
#include "kit_controller.h"
#include "pages.h"

class KitController;
class KitModel;
class MeasurementDetailsPage;
class CalibrationPage;
QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
QT_END_NAMESPACE

class HelpDialog : public QDialog
{
	Q_OBJECT
	public:
		HelpDialog(KitModel *model, KitController *controller, QString helpType);	
	public slots:
		void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	private:
		QListWidget *contentsWidget;
		QStackedWidget *pagesWidget;
};
class MeasurementDetailsDialog : public QDialog
{
	Q_OBJECT
	public:
		MeasurementDetailsDialog(KitModel *model, KitController *controller, QString demoType);
		MeasurementDetailsPage *tablePage;
	public slots:
		void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	private:
		QString demonstrationType;
		HelpDialog *helpDialog;
		QListWidget *contentsWidget;
		QStackedWidget *pagesWidget;
	private slots:
		void helpButtonClicked();
};
class CalibrationDialog : public QDialog
{
	Q_OBJECT
	public:
		CalibrationDialog(KitModel *model, KitController *controller, QString demoType);
		CalibrationPage *calibrationPage;
	public slots:
		void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	private:
		HelpDialog *helpDialog;
		QListWidget *contentsWidget;
		QStackedWidget *pagesWidget;
	private slots:
		void helpButtonClicked();
};
class ConfigTempDemoDialog : public QDialog
{
	Q_OBJECT
	public:
		ConfigTempDemoDialog(KitModel *model, KitController *controller);
	public slots:
		void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	private:
		HelpDialog *helpDialog;
		QListWidget *contentsWidget;
		QStackedWidget *pagesWidget;
	private slots:
		void closeDialog();
		void helpButtonClicked();
};
class ConfigMoistureDemoDialog : public QDialog
{
	Q_OBJECT
	public:
		ConfigMoistureDemoDialog(KitModel *model, KitController *controller);
	public slots:
		void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	private:
		HelpDialog *helpDialog;
		QListWidget *contentsWidget;
		QStackedWidget *pagesWidget;
	private slots:
		void helpButtonClicked();
};
#endif
