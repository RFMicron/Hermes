#include <QtWidgets>
#include "configdialog.h"
#include "kit_model.h"
#include "kit_controller.h"

MeasurementDetailsDialog::MeasurementDetailsDialog(KitModel *model, KitController *controller, QString demoType)
{
	demonstrationType = demoType;
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMaximumWidth(128);
	contentsWidget->setSpacing(12);
	pagesWidget = new QStackedWidget;
	tablePage = new MeasurementDetailsPage(model, controller, demoType);
	pagesWidget->addWidget(tablePage);
	QPushButton *closeButton = new QPushButton(tr("Close"));
	QPushButton *helpButton = new QPushButton(tr("Help"));
	helpDialog = new HelpDialog(model, controller, demoType + ":MeasurementDetailsDialog");
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(helpButton, SIGNAL(clicked()), this, SLOT(helpButtonClicked()));
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(pagesWidget, 1);
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(helpButton);
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
	setFixedSize(508, 298);
	setWindowTitle(tr("Details"));
}
void MeasurementDetailsDialog::helpButtonClicked()
{
	helpDialog->exec();
}
void MeasurementDetailsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;
	pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
CalibrationDialog::CalibrationDialog(KitModel *model, KitController *controller, QString demoType)
{
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMaximumWidth(128);
	contentsWidget->setSpacing(12);
	pagesWidget = new QStackedWidget;
	calibrationPage = new CalibrationPage(model, controller, demoType);
	pagesWidget->addWidget(calibrationPage);
	QPushButton *closeButton = new QPushButton(tr("Close"));
	QPushButton *helpButton = new QPushButton(tr("Help"));
	helpDialog = new HelpDialog(model, controller, demoType + ":CalibrationDialog");
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(helpButton, SIGNAL(clicked()), this, SLOT(helpButtonClicked()));
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(pagesWidget, 1);
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(helpButton);
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
	setFixedSize(565, 375);
	setWindowTitle(tr("Calibrate"));
}
void CalibrationDialog::helpButtonClicked()
{
	helpDialog->exec();
}
void CalibrationDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;
	pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
HelpDialog::HelpDialog(KitModel *model, KitController *controller, QString helpType)
{
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMaximumWidth(128);
	contentsWidget->setSpacing(12);
	pagesWidget = new QStackedWidget;
	pagesWidget->addWidget(new HelpPage(model, controller, helpType));
	QPushButton *closeButton = new QPushButton(tr("Close"));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(pagesWidget, 1);
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
	setWindowTitle(tr("Help"));
}
void HelpDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;
	pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
ConfigTempDemoDialog::ConfigTempDemoDialog(KitModel *model, KitController *controller)
{
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMaximumWidth(128);
	contentsWidget->setSpacing(12);
	pagesWidget = new QStackedWidget;
	pagesWidget->addWidget(new TempDemoConfigPage(model, controller));
	QPushButton *closeButton = new QPushButton(tr("Close"));
	QPushButton *helpButton = new QPushButton(tr("Help"));
	helpDialog = new HelpDialog(model, controller, "ConfigTempDemoDialog");
	connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));
	connect(helpButton, SIGNAL(clicked()), this, SLOT(helpButtonClicked()));
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(pagesWidget, 1);
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(helpButton);
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
	setMinimumHeight(425);
	setWindowTitle(tr("Temperature Demonstration Settings"));
}
void ConfigTempDemoDialog::closeDialog()
{
	qDebug("ConfigTempDemoDialog closing");
	close();
}
void ConfigTempDemoDialog::helpButtonClicked()
{
	helpDialog->exec();
}
void ConfigTempDemoDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;
	pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
ConfigMoistureDemoDialog::ConfigMoistureDemoDialog(KitModel *model, KitController *controller)
{
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMaximumWidth(128);
	contentsWidget->setSpacing(12);
	pagesWidget = new QStackedWidget;
	pagesWidget->addWidget(new MoistureDemoConfigPage(model, controller));
	QPushButton *closeButton = new QPushButton(tr("Close"));
	QPushButton *helpButton = new QPushButton(tr("Help"));
	helpDialog = new HelpDialog(model, controller, "ConfigMoistureDemoDialog");
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(helpButton, SIGNAL(clicked()), this, SLOT(helpButtonClicked()));
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(pagesWidget, 1);
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(helpButton);
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
	setMinimumHeight(435);
	setWindowTitle(tr("Configure Moisture Demonstration"));
}
void ConfigMoistureDemoDialog::helpButtonClicked()
{
	helpDialog->exec();
}
void ConfigMoistureDemoDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;
	pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
