#include <QtWidgets>
#include "pages.h"
#include "kit_model.h"
#include "kit_controller.h"

RemoteOpsSettingsPage::RemoteOpsSettingsPage(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	this->model = model;
	this->controller = controller;
	QGroupBox *configGroup = new QGroupBox(tr("Remote Operations Settings"));
	QVBoxLayout *configLayout = new QVBoxLayout;
	QFrame *settingsFrame = new QFrame;
	settingsFrame->setLayout(configLayout);
	settingsFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	settingsFrame->setLineWidth(3);
	settingsFrame->setMidLineWidth(3);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(configGroup);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
MeasurementDetailsPage::MeasurementDetailsPage(KitModel *model, KitController *controller, QString demoType)
	: QWidget()
{
	this->model = model;
	this->controller = controller;
	demonstrationType = demoType;
	selectTable  = new QTableWidget(1, 8, this);
	if (demonstrationType=="Temperature")
		selectTable->setHorizontalHeaderLabels(QString("EPC;Label;Model;Cal;Valid Reads;Invalid Reads;Temp (degC);On-Chip RSSI").split(";"));
	else
		selectTable->setHorizontalHeaderLabels(QString("EPC;Label;Model;Cal;Valid Reads;Invalid Reads;Sensor Code;On-Chip RSSI").split(";"));
	selectTable->setColumnWidth(0,180);
	selectTable->setColumnWidth(1,90);
	selectTable->setColumnWidth(2,50);
	selectTable->setColumnWidth(3,50);
	selectTable->setColumnWidth(4,100);
	selectTable->setColumnWidth(5,100);
	selectTable->setColumnWidth(6,100);
	selectTable->setColumnWidth(7,100);
	selectTable->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignHCenter);
	selectTable->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignHCenter);
	selectTable->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignHCenter);
	selectTable->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignHCenter);
	selectTable->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignHCenter);
	selectTable->horizontalHeaderItem(5)->setTextAlignment(Qt::AlignHCenter);
	selectTable->horizontalHeaderItem(6)->setTextAlignment(Qt::AlignHCenter);
	selectTable->horizontalHeaderItem(7)->setTextAlignment(Qt::AlignHCenter);
	QLabel *titleLabel = new QLabel(tr("Measurement Details"));
	QHBoxLayout *selectTableLayout = new QHBoxLayout;
	selectTableLayout->addWidget(selectTable);
	QHBoxLayout *setupLayout = new QHBoxLayout;
	setupLayout->addLayout(selectTableLayout);
	QFrame *settingsFrame = new QFrame;
	settingsFrame->setLayout(setupLayout);
	settingsFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	settingsFrame->setLineWidth(3);
	settingsFrame->setMidLineWidth(3);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(settingsFrame);
	setLayout(mainLayout);
}
void MeasurementDetailsPage::loadTempTableSlot(QList<SensorTag> TempTagList)
{
	selectTable->clearContents();
	selectTable->setRowCount(0);
	selectTable->setRowCount(TempTagList.length());
	for (int t=0; t<TempTagList.length(); t++)
	{
		selectTable->setItem(t,0, new QTableWidgetItem(TempTagList[t].getEpc()));
		selectTable->setItem(t,1, new QTableWidgetItem(TempTagList[t].Label));
		selectTable->setItem(t,2, new QTableWidgetItem(TempTagList[t].getModel()));
		bool cal = TempTagList[t].getCrcValid();
		if (cal)
			selectTable->setItem(t,3, new QTableWidgetItem("Y"));
		else
			selectTable->setItem(t,3, new QTableWidgetItem("N"));
		int len=TempTagList[t].TemperatureMeasurementHistory.length();
		if (len==0)
		{
			selectTable->setItem(t,4, new QTableWidgetItem("--"));
			selectTable->setItem(t,5, new QTableWidgetItem("--"));
			selectTable->setItem(t,6, new QTableWidgetItem("--"));
			selectTable->setItem(t,7, new QTableWidgetItem("--"));
			continue;
		}
		selectTable->setItem(t,4, new QTableWidgetItem(QString::number(TempTagList[t].TemperatureMeasurementHistory[len-1].getValidPowerReadCount())));
		selectTable->setItem(t,5, new QTableWidgetItem(QString::number(TempTagList[t].TemperatureMeasurementHistory[len-1].getInvalidPowerReadCount())));
		float temp=TempTagList[t].TemperatureMeasurementHistory[len-1].getValue();
		float ocRssi=TempTagList[t].OnChipRssiMeasurementHistory[len-1].getValue();
		if (temp>-100)
			selectTable->setItem(t,6, new QTableWidgetItem(QString::number(temp,'f',1)));
		else
			selectTable->setItem(t,6, new QTableWidgetItem("----"));
		if (ocRssi>0)
			selectTable->setItem(t,7, new QTableWidgetItem(QString::number(ocRssi,'f',1)));
		else
			selectTable->setItem(t,7, new QTableWidgetItem("----"));			
	}
}
void MeasurementDetailsPage::loadMoistTableSlot(QList<SensorTag> MoistTagList)
{
	selectTable->clearContents();
	selectTable->setRowCount(0);
	selectTable->setRowCount(MoistTagList.length());
	for (int t=0; t<MoistTagList.length(); t++)
	{
		selectTable->setItem(t,0, new QTableWidgetItem(MoistTagList[t].getEpc()));
		selectTable->setItem(t,1, new QTableWidgetItem(MoistTagList[t].Label));
		selectTable->setItem(t,2, new QTableWidgetItem(MoistTagList[t].getModel()));
		bool cal = MoistTagList[t].getCrcValid();
		if (cal)
			selectTable->setItem(t,3, new QTableWidgetItem("Y"));
		else
			selectTable->setItem(t,3, new QTableWidgetItem("N"));
		int len=MoistTagList[t].SensorMeasurementHistory.length();
		if (len==0)
		{
			selectTable->setItem(t,4, new QTableWidgetItem("--"));
			selectTable->setItem(t,5, new QTableWidgetItem("--"));
			selectTable->setItem(t,6, new QTableWidgetItem("--"));
			selectTable->setItem(t,7, new QTableWidgetItem("--"));
			continue;
		}
		selectTable->setItem(t,4, new QTableWidgetItem(QString::number(MoistTagList[t].SensorMeasurementHistory[len-1].getValidPowerReadCount())));
		selectTable->setItem(t,5, new QTableWidgetItem(QString::number(MoistTagList[t].SensorMeasurementHistory[len-1].getInvalidPowerReadCount())));
		float moist=MoistTagList[t].SensorMeasurementHistory[len-1].getValue();
		float ocRssi=MoistTagList[t].OnChipRssiMeasurementHistory[len-1].getValue();
		if (moist>-1)
			selectTable->setItem(t,6, new QTableWidgetItem(QString::number(moist,'f',1)));
		else
			selectTable->setItem(t,6, new QTableWidgetItem("----"));
		if (ocRssi>0)
			selectTable->setItem(t,7, new QTableWidgetItem(QString::number(ocRssi,'f',1)));
		else
			selectTable->setItem(t,7, new QTableWidgetItem("----"));			
	}
}
TempDemoConfigPage::TempDemoConfigPage(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	QLabel *titleLabel = new QLabel(tr("Temperature Demo Settings"));
	QTabWidget *tabWidget = new QTabWidget;
	tabWidget->addTab(new TempDemoReaderPowerTab(model, controller), tr("Power"));
	tabWidget->addTab(new TempDemoOtherSettingsTab(model, controller), tr("Other"));
	QVBoxLayout *tabWidgetLayout = new QVBoxLayout;
	tabWidgetLayout->addWidget(tabWidget);
	QFrame *settingsFrame = new QFrame;
	settingsFrame->setLayout(tabWidgetLayout);
	settingsFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	settingsFrame->setLineWidth(3);
	settingsFrame->setMidLineWidth(3);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(settingsFrame);
	setLayout(mainLayout);
}
TempDemoOtherSettingsTab::TempDemoOtherSettingsTab(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	this->model = model;
	this->controller = controller;
	tuningTransmitter = false;
	QLabel *bandRegionLabel = new QLabel(tr("Band Region"));
	bandRegionCombo = new QComboBox;
	bandRegionCombo->addItem(tr("North America"));
	bandRegionCombo->addItem(tr("E.U."));
	bandRegionCombo->addItem(tr("China"));
	bandRegionCombo->addItem(tr("Japan"));
	connect(model, SIGNAL(antennaTuningSignal(int, int)), this, SLOT(antennaTuningSlot(int, int)));
	connect(model, SIGNAL(bandChangedSignal(FreqBandEnum)), this, SLOT(bandChangedSlot(FreqBandEnum)));
	tuningProgress = new QProgressDialog(this);
	tuningProgress->setLabelText("Tuning transmitter at new band. Please wait...");
	tuningProgress->setWindowTitle("Hermes Setup");
	tuningProgress->setCancelButton(0);
	tuningProgress->close();
	QLabel *samplesPerMeasurementLabel = new QLabel(tr("Samples per Measurement"));
	QComboBox *samplesPerMeasurementComboBox = new QComboBox;
	samplesPerMeasurementComboBox->addItem(tr("5"));
	samplesPerMeasurementComboBox->addItem(tr("10"));
	samplesPerMeasurementComboBox->addItem(tr("20"));
	samplesPerMeasurementComboBox->addItem(tr("30"));
	connect(bandRegionCombo, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(bandRegionChanged(QString)));
	connect(samplesPerMeasurementComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(samplesPerMeasurementChanged(QString)));
	samplesPerMeasurementComboBox->setCurrentIndex(1);
	QHBoxLayout *bandRegionComboLayout = new QHBoxLayout;
	bandRegionComboLayout->addWidget(bandRegionCombo);
	bandRegionComboLayout->addStretch(1);
	QHBoxLayout *bandRegionLayout = new QHBoxLayout;
	bandRegionLayout->addWidget(bandRegionLabel);
	bandRegionLayout->addLayout(bandRegionComboLayout);
	QHBoxLayout *samplesPerMeasurementLayout = new QHBoxLayout;
	samplesPerMeasurementLayout->addWidget(samplesPerMeasurementLabel);
	samplesPerMeasurementLayout->addWidget(samplesPerMeasurementComboBox);
	samplesPerMeasurementLayout->addStretch(1);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(bandRegionLayout);
	mainLayout->addLayout(samplesPerMeasurementLayout);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
void TempDemoOtherSettingsTab::bandRegionChanged(QString region)
{
	this->setCursor(Qt::WaitCursor);
	int status=0;
	if (region=="North America" && model->currentFreqBand != FCC)
	{
		tuningTransmitter=true;
		status=controller->setBandRegion(FCC);
	}
	if (region=="E.U." && model->currentFreqBand != ETSI)
	{
		tuningTransmitter=true;
		status=controller->setBandRegion(ETSI);
		qDebug("pages: response from controller: status of setBandRegion call: %d", status);
	}
	if (region=="China" && model->currentFreqBand != PRC)
	{
		tuningTransmitter=true;
		status=controller->setBandRegion(PRC);
	}
	if (region=="Japan" && model->currentFreqBand != JAPAN)
	{
		tuningTransmitter=true;
		status=controller->setBandRegion(JAPAN);
	}
	this->setCursor(Qt::ArrowCursor);
	if (status!=0)
	{
		qDebug("Error setting frequencies");
		QMessageBox msgBox;
		msgBox.setWindowTitle("Error");
		msgBox.setText("Error setting frequencies. Please restart application.");
		msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
		msgBox.exec();
		QFile runningFile("running");
		if (runningFile.exists())
			runningFile.remove();
		controller->turnReaderOff();
		exit(EXIT_FAILURE);
	}
	tuningTransmitter=false;
}
void TempDemoOtherSettingsTab::bandChangedSlot(FreqBandEnum band)
{
	if (band==FCC && bandRegionCombo->currentText() != "North America")
		bandRegionCombo->setCurrentIndex(0);
	if (band==ETSI && bandRegionCombo->currentText() != "E.U.")
		bandRegionCombo->setCurrentIndex(1);
	if (band==PRC && bandRegionCombo->currentText() != "China")
		bandRegionCombo->setCurrentIndex(2);
	if (band==JAPAN && bandRegionCombo->currentText() != "Japan")
		bandRegionCombo->setCurrentIndex(3);	
}
void TempDemoOtherSettingsTab::antennaTuningSlot(int currentFreq, int totalFreq)
{
	if (tuningTransmitter==false)
		return;
	if (currentFreq==0)
	{
		tuningProgress->setMaximum(totalFreq);
		tuningProgress->setValue(0);
	}
	else
		tuningProgress->setValue(currentFreq);
	QApplication::processEvents();
}
void TempDemoOtherSettingsTab::samplesPerMeasurementChanged(QString samples)
{
	controller->setTempSamplesPerMeasurement(samples);
}
TempDemoReaderPowerTab::TempDemoReaderPowerTab(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	qDebug("TempDemoReaderPowerTab constructor");
	this->model = model;
	this->controller = controller;
	QCheckBox *autoPowerAdjustCheckbox = new QCheckBox("Auto Power Adjust", this);
	connect(autoPowerAdjustCheckbox, SIGNAL(stateChanged(int)),
		this, SLOT(autoPowerStateChanged(int))); 	
	minMeasPowerTargetRangeSpinBox = new QSpinBox;
	maxMeasPowerTargetRangeSpinBox = new QSpinBox;
	QLabel *minMeasPowerTargetRangeLabel = new QLabel(tr("Mininum On-Chip Rssi Target"));    
	QLabel *maxMeasPowerTargetRangeLabel = new QLabel(tr("Maximum On-Chip Rssi Target"));    
	minMeasPowerTargetRangeSpinBox->setRange(5, 21);
	maxMeasPowerTargetRangeSpinBox->setRange(15, 31);
	connect(minMeasPowerTargetRangeSpinBox, SIGNAL(valueChanged(int)),
			this, SLOT(minPowerTargetChanged(int)));
	connect(maxMeasPowerTargetRangeSpinBox, SIGNAL(valueChanged(int)),
			this, SLOT(maxPowerTargetChanged(int)));
	minMeasPowerTargetRangeSpinBox->setValue(15);  
	maxMeasPowerTargetRangeSpinBox->setValue(21);
	QLabel *readerPowerLevelLabel = new QLabel(tr("Max Power Level"));
	autoPowerAdjustCheckbox->setChecked(true);
	d_wheel = new QwtWheel();
	d_wheel->setWheelWidth( 20 );
	d_wheel->setMass( 1.0 );
	d_thermo = new QwtThermo();
	d_thermo->setOrientation( Qt::Horizontal );
	d_thermo->setScalePosition( QwtThermo::LeadingScale );
	d_wheel->setOrientation( Qt::Vertical );
	d_wheel->setRange( 18, 30 );
	QwtLinearColorMap *colorMap = new QwtLinearColorMap();
	colorMap->setColorInterval( Qt::darkCyan, Qt::yellow );
	d_thermo->setColorMap( colorMap );
	double min = d_wheel->minimum();
	double max = d_wheel->maximum();
	if ( d_wheel->isInverted() )
		qSwap( min, max );
	d_thermo->setScale( min, max );
	d_thermo->setValue( d_wheel->value() );
	connect( d_wheel, SIGNAL( valueChanged( double ) ),
			d_thermo, SLOT( setValue( double ) ) );
	d_wheel->setValue( 27 );
	readerPowerLevelThermoLabel = new QLabel;
	readerPowerLevelThermoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	setNum(d_wheel->value());
	connect(d_wheel, SIGNAL(valueChanged(double)), this, SLOT(setNum(double)));
	QBoxLayout *readerPowerLevelIndicatorLayout;
	readerPowerLevelIndicatorLayout = new QHBoxLayout;
	readerPowerLevelIndicatorLayout->addWidget(readerPowerLevelLabel);
	readerPowerLevelIndicatorLayout->addWidget( d_thermo, Qt::AlignCenter );
	readerPowerLevelIndicatorLayout->addWidget( d_wheel );
	QVBoxLayout *readerPowerLevelLayout = new QVBoxLayout;
	readerPowerLevelLayout->addLayout(readerPowerLevelIndicatorLayout);
	readerPowerLevelLayout->addWidget(readerPowerLevelThermoLabel);
	QFrame *readerPowerLevelFrame = new QFrame;
	readerPowerLevelFrame->setLayout(readerPowerLevelLayout);
	readerPowerLevelFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	readerPowerLevelFrame->setLineWidth(3);
	readerPowerLevelFrame->setMidLineWidth(3);
	QHBoxLayout *autoPowerAdjustLayout = new QHBoxLayout;
	autoPowerAdjustLayout->addWidget(autoPowerAdjustCheckbox);
	autoPowerAdjustLayout->addStretch(1);
	QHBoxLayout *minMeasPowerTargetRangeLayout  = new QHBoxLayout;
	minMeasPowerTargetRangeLayout->addWidget(minMeasPowerTargetRangeLabel);
	minMeasPowerTargetRangeLayout->addWidget(minMeasPowerTargetRangeSpinBox);
	minMeasPowerTargetRangeLayout->addStretch(1);
	QHBoxLayout *maxMeasPowerTargetRangeLayout  = new QHBoxLayout;
	maxMeasPowerTargetRangeLayout->addWidget(maxMeasPowerTargetRangeLabel);
	maxMeasPowerTargetRangeLayout->addWidget(maxMeasPowerTargetRangeSpinBox);
	maxMeasPowerTargetRangeLayout->addStretch(1);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(autoPowerAdjustLayout);
	mainLayout->addWidget(readerPowerLevelFrame);
	mainLayout->addLayout(minMeasPowerTargetRangeLayout);
	mainLayout->addLayout(maxMeasPowerTargetRangeLayout);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
void TempDemoReaderPowerTab::setNum(double v)
{
	QString text;
	text.setNum(v, 'f', 0);
	int dBm = (int)v;
	readerPowerLevelThermoLabel->setText("Power: " + text +" dBm");
	controller->setTempMaxPowerLevel(dBm);
}
void TempDemoReaderPowerTab::autoPowerStateChanged(int s)
{
	qDebug("Changing temp autopower state");
	if (s==0) // Auto power off
	{
		minMeasPowerTargetRangeSpinBox->setEnabled(false);
		maxMeasPowerTargetRangeSpinBox->setEnabled(false);
		controller->setTempAutoPower(false);
		qDebug("autopower set to false");
	}
	else if (s==2) // Auto power on
	{
		minMeasPowerTargetRangeSpinBox->setEnabled(true);
		maxMeasPowerTargetRangeSpinBox->setEnabled(true);
		controller->setTempAutoPower(true);
		qDebug("autopower set to true");
	}
}
void TempDemoReaderPowerTab::minPowerTargetChanged(int min)
{
	controller->setTempOnChipRssiTargetMin(min);
	maxMeasPowerTargetRangeSpinBox->setRange(min, 31);
}
void TempDemoReaderPowerTab::maxPowerTargetChanged(int max)
{
	controller->setTempOnChipRssiTargetMax(max);
	minMeasPowerTargetRangeSpinBox->setRange(5, max);
}
//================================================
//================================================
//================================================
//===============    MOISTURE     =======================
//================================================
//================================================
//================================================
MoistureDemoConfigPage::MoistureDemoConfigPage(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	QLabel *titleLabel = new QLabel(tr("Moisture Demo Settings"));
	QTabWidget *tabWidget = new QTabWidget;
	tabWidget->addTab(new MoistureDemoReaderPowerTab(model, controller), tr("Power"));
	tabWidget->addTab(new MoistureDemoOtherSettingsTab(model, controller), tr("Other"));
	QVBoxLayout *tabWidgetLayout = new QVBoxLayout;
	tabWidgetLayout->addWidget(tabWidget);
	QFrame *settingsFrame = new QFrame;
	settingsFrame->setLayout(tabWidgetLayout);
	settingsFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	settingsFrame->setLineWidth(3);
	settingsFrame->setMidLineWidth(3);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(settingsFrame);
	setLayout(mainLayout);
}
MoistureDemoOtherSettingsTab::MoistureDemoOtherSettingsTab(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	this->model = model;
	this->controller = controller;
	tuningTransmitter=false;
	QLabel *dataReductionLabel = new QLabel(tr("Data Reduction Method"));
	QComboBox *dataReductionCombo = new QComboBox;
	dataReductionCombo->addItem(tr("Average Over Freq"));
	dataReductionCombo->addItem(tr("Linear Fit"));
	QLabel *bandRegionLabel = new QLabel(tr("Band Region"));
	bandRegionCombo = new QComboBox;
	bandRegionCombo->addItem(tr("North America"));
	bandRegionCombo->addItem(tr("E.U."));
	bandRegionCombo->addItem(tr("China"));
	bandRegionCombo->addItem(tr("Japan"));
	connect(model, SIGNAL(antennaTuningSignal(int, int)), this, SLOT(antennaTuningSlot(int, int)));
	connect(model, SIGNAL(bandChangedSignal(FreqBandEnum)), this, SLOT(bandChangedSlot(FreqBandEnum)));
	tuningProgress = new QProgressDialog(this);
	tuningProgress->setLabelText("Tuning transmitter at new band. Please wait...");
	tuningProgress->setWindowTitle("Hermes Setup");
	tuningProgress->setCancelButton(0);
	tuningProgress->close();
	QLabel *samplesPerMeasurementLabel = new QLabel(tr("Samples per Measurement"));
	QComboBox *samplesPerMeasurementComboBox = new QComboBox;
	samplesPerMeasurementComboBox->addItem(tr("2"));
	samplesPerMeasurementComboBox->addItem(tr("5"));
	samplesPerMeasurementComboBox->addItem(tr("10"));
	samplesPerMeasurementComboBox->addItem(tr("20"));
	QLabel *wetThresholdLabel = new QLabel(tr("Wet Threshold"));
	QSpinBox *wetThresholdSpinBox = new QSpinBox;
	wetThresholdSpinBox->setRange(1,511);
	QLabel *wetThresholdDirectionLabel = new QLabel(tr("Wet Is Above/Below Threshold"));
	QComboBox *wetThresholdDirectionCombo = new QComboBox;
	wetThresholdDirectionCombo->addItem(tr("Above"));
	wetThresholdDirectionCombo->addItem(tr("Below"));
	connect(dataReductionCombo, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(dataReductionChanged(QString)));
	connect(bandRegionCombo, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(bandRegionChanged(QString)));
	connect(samplesPerMeasurementComboBox, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(samplesPerMeasurementChanged(QString)));
	connect(wetThresholdSpinBox, SIGNAL(valueChanged(int)),
			this, SLOT(wetThresholdChanged(int)));
	connect(wetThresholdDirectionCombo, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(wetThresholdDirectionChanged(QString)));
	//====== set defaults =======
	dataReductionCombo->setCurrentIndex(1);
	samplesPerMeasurementComboBox->setCurrentIndex(1);
	wetThresholdDirectionCombo->setCurrentIndex(1);
	wetThresholdSpinBox->setValue(15);
	//=====================
	QHBoxLayout *dataReductionComboLayout = new QHBoxLayout;
	dataReductionComboLayout->addWidget(dataReductionCombo);
	dataReductionComboLayout->addStretch(1);
	QVBoxLayout *dataReductionLayout = new QVBoxLayout;
	dataReductionLayout->addWidget(dataReductionLabel);
	dataReductionLayout->addLayout(dataReductionComboLayout);
	QHBoxLayout *bandRegionComboLayout = new QHBoxLayout;
	bandRegionComboLayout->addWidget(bandRegionCombo);
	bandRegionComboLayout->addStretch(1);
	QVBoxLayout *bandRegionLayout = new QVBoxLayout;
	bandRegionLayout->addWidget(bandRegionLabel);
	bandRegionLayout->addLayout(bandRegionComboLayout);
	QHBoxLayout *samplesPerMeasurementLayout = new QHBoxLayout;
	samplesPerMeasurementLayout->addWidget(samplesPerMeasurementLabel);
	samplesPerMeasurementLayout->addWidget(samplesPerMeasurementComboBox);
	samplesPerMeasurementLayout->addStretch(1);
	QHBoxLayout *wetThresholdLayout = new QHBoxLayout;
	wetThresholdLayout->addWidget(wetThresholdLabel);
	wetThresholdLayout->addWidget(wetThresholdSpinBox);
	wetThresholdLayout->addStretch(1);
	QHBoxLayout *wetThresholdDirectionLayout = new QHBoxLayout;
	wetThresholdDirectionLayout->addWidget(wetThresholdDirectionLabel);
	wetThresholdDirectionLayout->addWidget(wetThresholdDirectionCombo);
	wetThresholdDirectionLayout->addStretch(1);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(dataReductionLayout);
	mainLayout->addLayout(bandRegionLayout);
	mainLayout->addLayout(samplesPerMeasurementLayout);
	mainLayout->addLayout(wetThresholdLayout);
	mainLayout->addLayout(wetThresholdDirectionLayout);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
void MoistureDemoOtherSettingsTab::dataReductionChanged(QString method)
{
	if (method=="Linear Fit")
		controller->setMoistLinearFit(true);
	else
		controller->setMoistLinearFit(false);
}
void MoistureDemoOtherSettingsTab::bandRegionChanged(QString region)
{
	int status=0;
	this->setCursor(Qt::WaitCursor);
	if (region=="North America" && model->currentFreqBand != FCC)
	{
		tuningTransmitter=true;
		status=controller->setBandRegion(FCC);
	}
	if (region=="E.U." && model->currentFreqBand != ETSI)
	{
		tuningTransmitter=true;
		status=controller->setBandRegion(ETSI);
	}
	if (region=="China" && model->currentFreqBand != PRC)
	{
		tuningTransmitter=true;
		status=controller->setBandRegion(PRC);
	}
	if (region=="Japan" && model->currentFreqBand != JAPAN)
	{
		tuningTransmitter=true;
		status=controller->setBandRegion(JAPAN);
	}
	this->setCursor(Qt::ArrowCursor);
	if (status!=0)
	{
		qDebug("Error setting frequencies");
		QMessageBox msgBox;
		msgBox.setWindowTitle("Error");
		msgBox.setText("Error setting frequencies. Please restart application.");
		msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
		msgBox.exec();
		QFile runningFile("running");
		if (runningFile.exists())
			runningFile.remove();
		controller->turnReaderOff();
		exit(EXIT_FAILURE);
	}
	tuningTransmitter=false;
}
void MoistureDemoOtherSettingsTab::bandChangedSlot(FreqBandEnum band)
{
	if (band==FCC && bandRegionCombo->currentText() != "North America")
		bandRegionCombo->setCurrentIndex(0);
	if (band==ETSI && bandRegionCombo->currentText() != "E.U.")
		bandRegionCombo->setCurrentIndex(1);
	if (band==PRC && bandRegionCombo->currentText() != "China")
		bandRegionCombo->setCurrentIndex(2);
	if (band==JAPAN && bandRegionCombo->currentText() != "Japan")
		bandRegionCombo->setCurrentIndex(3);	
}
void MoistureDemoOtherSettingsTab::antennaTuningSlot(int currentFreq, int totalFreq)
{
	if (tuningTransmitter==false)
		return;
	if (currentFreq==0)
	{
		tuningProgress->setMaximum(totalFreq);
		tuningProgress->setValue(0);
	}
	else
		tuningProgress->setValue(currentFreq);
	QApplication::processEvents();
}
void MoistureDemoOtherSettingsTab::samplesPerMeasurementChanged(QString samples)
{
	controller->setMoistSamplesPerMeasurement(samples);
}
void MoistureDemoOtherSettingsTab::wetThresholdChanged(int threshold)
{
	controller->setMoistThreshold(threshold);
}
void MoistureDemoOtherSettingsTab::wetThresholdDirectionChanged(QString direction)
{
	if (direction=="Above")
		controller->setMoistAboveThreshold(true);
	else
		controller->setMoistAboveThreshold(false);
}
MoistureDemoReaderPowerTab::MoistureDemoReaderPowerTab(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	this->model = model;
	this->controller = controller;
	QCheckBox *autoPowerAdjustCheckbox = new QCheckBox("Auto Power Adjust", this);
	connect(autoPowerAdjustCheckbox, SIGNAL(stateChanged(int)),
			this, SLOT(autoPowerStateChanged(int)));
	minMeasPowerTargetRangeSpinBox = new QSpinBox;
	maxMeasPowerTargetRangeSpinBox = new QSpinBox;
	QLabel *minMeasPowerTargetRangeLabel = new QLabel(tr("Mininum On-Chip Rssi Target"));    
	QLabel *maxMeasPowerTargetRangeLabel = new QLabel(tr("Maximum On-Chip Rssi Target"));    
	minMeasPowerTargetRangeSpinBox->setRange(1, 24);
	maxMeasPowerTargetRangeSpinBox->setRange(15, 31);
	connect(minMeasPowerTargetRangeSpinBox, SIGNAL(valueChanged(int)),
			this, SLOT(minPowerTargetChanged(int)));
	connect(maxMeasPowerTargetRangeSpinBox, SIGNAL(valueChanged(int)),
			this, SLOT(maxPowerTargetChanged(int)));
	minMeasPowerTargetRangeSpinBox->setValue(15);  
	maxMeasPowerTargetRangeSpinBox->setValue(24);
	autoPowerAdjustCheckbox->setChecked(true);
	QLabel *readerPowerLevelLabel = new QLabel(tr("Max Power Level"));
	d_wheel = new QwtWheel(); 
	d_wheel->setWheelWidth( 20 );
	d_wheel->setMass( 1.0 );
	d_thermo = new QwtThermo();
	d_thermo->setOrientation( Qt::Horizontal );
	d_thermo->setScalePosition( QwtThermo::LeadingScale );
	d_wheel->setOrientation( Qt::Vertical );
	d_wheel->setRange( 18, 30 );
	QwtLinearColorMap *colorMap = new QwtLinearColorMap();
	colorMap->setColorInterval( Qt::darkCyan, Qt::yellow );
	d_thermo->setColorMap( colorMap );
	double min = d_wheel->minimum();
	double max = d_wheel->maximum();
	if ( d_wheel->isInverted() )
		qSwap( min, max );
	d_thermo->setScale( min, max );
	d_thermo->setValue( d_wheel->value() );
	connect( d_wheel, SIGNAL( valueChanged( double ) ),
			d_thermo, SLOT( setValue( double ) ) );
	d_wheel->setValue( 27 );
	readerPowerLevelThermoLabel = new QLabel;
	readerPowerLevelThermoLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	setNum(d_wheel->value());
	connect(d_wheel, SIGNAL(valueChanged(double)), this, SLOT(setNum(double)));
	QBoxLayout *readerPowerLevelIndicatorLayout;
	readerPowerLevelIndicatorLayout = new QHBoxLayout;
	readerPowerLevelIndicatorLayout->addWidget(readerPowerLevelLabel);
	readerPowerLevelIndicatorLayout->addWidget( d_thermo, Qt::AlignCenter );
	readerPowerLevelIndicatorLayout->addWidget( d_wheel );
	QVBoxLayout *readerPowerLevelLayout = new QVBoxLayout;
	readerPowerLevelLayout->addLayout(readerPowerLevelIndicatorLayout);
	readerPowerLevelLayout->addWidget(readerPowerLevelThermoLabel);
	QFrame *readerPowerLevelFrame = new QFrame;
	readerPowerLevelFrame->setLayout(readerPowerLevelLayout);
	readerPowerLevelFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	readerPowerLevelFrame->setLineWidth(3);
	readerPowerLevelFrame->setMidLineWidth(3);
	QHBoxLayout *autoPowerAdjustLayout = new QHBoxLayout;
	autoPowerAdjustLayout->addWidget(autoPowerAdjustCheckbox);
	autoPowerAdjustLayout->addStretch(1);
	QHBoxLayout *minMeasPowerTargetRangeLayout  = new QHBoxLayout;
	minMeasPowerTargetRangeLayout->addWidget(minMeasPowerTargetRangeLabel);
	minMeasPowerTargetRangeLayout->addWidget(minMeasPowerTargetRangeSpinBox);
	minMeasPowerTargetRangeLayout->addStretch(1);
	QHBoxLayout *maxMeasPowerTargetRangeLayout  = new QHBoxLayout;
	maxMeasPowerTargetRangeLayout->addWidget(maxMeasPowerTargetRangeLabel);
	maxMeasPowerTargetRangeLayout->addWidget(maxMeasPowerTargetRangeSpinBox);
	maxMeasPowerTargetRangeLayout->addStretch(1);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(autoPowerAdjustLayout);
	mainLayout->addWidget(readerPowerLevelFrame);
	mainLayout->addLayout(minMeasPowerTargetRangeLayout);
	mainLayout->addLayout(maxMeasPowerTargetRangeLayout);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}
void MoistureDemoReaderPowerTab::setNum(double v)
{
	QString text;
	text.setNum(v, 'f', 0);
	int dBm = (int)v;
	readerPowerLevelThermoLabel->setText("Power: " + text +" dBm");
	controller->setMoistMaxPowerLevel(dBm);
}
void MoistureDemoReaderPowerTab::autoPowerStateChanged(int s)
{
	if (s==0) // Auto power off
	{
		minMeasPowerTargetRangeSpinBox->setEnabled(false);
		maxMeasPowerTargetRangeSpinBox->setEnabled(false);
		controller->setMoistAutoPower(false);
	}
	else if (s==2) // Auto power on
	{
		minMeasPowerTargetRangeSpinBox->setEnabled(true);
		maxMeasPowerTargetRangeSpinBox->setEnabled(true);
		controller->setMoistAutoPower(true);
	}
}
void MoistureDemoReaderPowerTab::minPowerTargetChanged(int min)
{
	controller->setMoistOnChipRssiTargetMin(min);
	maxMeasPowerTargetRangeSpinBox->setRange(min, 31);
}
void MoistureDemoReaderPowerTab::maxPowerTargetChanged(int max)
{
	controller->setMoistOnChipRssiTargetMax(max);
	minMeasPowerTargetRangeSpinBox->setRange(1, max);
}
HelpPage::HelpPage(KitModel *model, KitController *controller, QString helpType)
	:QWidget()
{    
	QLabel *titleLabel = new QLabel(tr("Window's Usage"));
	QTextEdit *instructionsView = new QTextEdit;
	instructionsView->setReadOnly(true);
	if (helpType=="Temperature:CalibrationDialog")
	{
		instructionsView->setText("Temperature-enabled tags generate a Temperature Code which can be translated to a temperature measurement in degrees. To make the translation, the average temperature codes generated by the tag at 2 known temperatures are stored in tag memory. This dialog is for overwriting any existing calibration information with new data.");
		instructionsView->append("");
		instructionsView->append("Calibration will be applied to the tag selected on the temperature plot.");
		instructionsView->append("");
		instructionsView->append("For 1-point calibration, measured values are specified at one temperature and values for the second temperature are automatically estimated and added.");
		instructionsView->append("");
		instructionsView->append("For 2-point calibration, measured values are specified at two temperatures, which can offer improved accuracy.");
		instructionsView->append("");
		instructionsView->append("Before calibrating, make sure that Autopower is turned on, or that the tag is receiving the proper power level. Temperature codes can be read by clicking the Read buttons or manually entering a value. Temperatures must be entered manually.");
	}
	else if (helpType=="TempDemoPage")
	{
		instructionsView->setText("Press <Start to begin reading tags.");
		instructionsView->append("");
		instructionsView->append("Temperature-enabled tags will be read repeatedly with a preferred cycle time given by the Period pulldown until <Stop> is pressed. Tags can be selected or de-selected for observation by clicking on them in the plot legend.");
		instructionsView->append("");
		instructionsView->append("<Clear> erases the plot and clears the measurement history");
		instructionsView->append("");
		instructionsView->append("<Export> writes a log file containing the measurement results");
		instructionsView->append("");
		instructionsView->append("<Details> displays a table giving additional information about the most recent measurement");
		instructionsView->append("");
		instructionsView->append("<Settings> contains controls for reader power and other measurement parameters");
		instructionsView->append("");
		instructionsView->append("<Calibrate> allows writing new temperature calibration data to the tag. Exactly one temperature tag must be selected in the result plot.");
		instructionsView->append("");
		instructionsView->append("<Main> returns to the main screen");
	}
	else if (helpType=="MoistureDemoPage")
	{
		instructionsView->setText("Press <Start> to begin reading tags.");
		instructionsView->append("");
		instructionsView->append("Tags will be read repeatedly with a preferred cycle time given by the Period pulldown until <Stop> is pressed. Tags can be selected or de-selected for observation by clicking on them in the plot legend.");
		instructionsView->append("");
		instructionsView->append("<Clear> erases the plot and clears the measurement history");
		instructionsView->append("");
		instructionsView->append("<Export> writes a log file containing the measurement results");
		instructionsView->append("");
		instructionsView->append("<Details> displays a table giving additional information about the most recent measurement");
		instructionsView->append("");
		instructionsView->append("<Settings> contains controls for reader power and other measurement parameters");
		instructionsView->append("");
		instructionsView->append("<Main> returns to the main screen");
	}
	else if (helpType=="ConfigTempDemoDialog")
	{
		instructionsView->setText("Power Tab:");
		instructionsView->append("");
		instructionsView->append("Auto Power Adjust: When this box is checked, the application will automatically adjust the transmit power to try to achieve On-Chip RSSI values on the sensor tags within the target range. This is the recommended setting.");
		instructionsView->append("");
		instructionsView->append("Max Power Level: When Auto Power Adjust is checked, the reader will not transmit at powers above this limit. When Auto Power is not checked, the reader will always transmit at this level.");
		instructionsView->append("");
		instructionsView->append("On-Chip RSSI Targets: When Auto Power Adjust is checked, these targets define the On-Chip RSSI range the reader will attempt to achieve for each sensor tag. Use caution if setting a narrow range: the reader may not find a satisfactory power.");
		instructionsView->append("");
		instructionsView->append("Other Tab:");
		instructionsView->append("");
		instructionsView->append("Band Region: This sets the frequency band to be used, by regulatory region.");
		instructionsView->append("");
		instructionsView->append("Samples per Measurement: The number of valid sensor readings to be taken and averaged to produce a single measurment point on the result plot. Higher values result in more precise measurements but longer measurement time.");
	}
	else if (helpType=="ConfigMoistureDemoDialog")
	{
		instructionsView->setText("Power Tab:");
		instructionsView->append("");
		instructionsView->append("Auto Power Adjust: When this box is checked, the application will automatically adjust the transmit power to try to achieve On-Chip RSSI values on the sensor tags within the target range. This is the recommended setting.");
		instructionsView->append("");
		instructionsView->append("Max Power Level: When Auto Power Adjust is checked, the reader will not transmit at powers above this limit. When Auto Power is not checked, the reader will always transmit at this level.");
		instructionsView->append("");
		instructionsView->append("On-Chip RSSI Targets: When Auto Power Adjust is checked, these targets define the On-Chip RSSI range the reader will attempt to achieve for each sensor tag. Use caution if setting a narrow range: the reader may not find a satisfactory power.");
		instructionsView->append("");
		instructionsView->append("Other Tab:");
		instructionsView->append("");
		instructionsView->append("Data Reduction Method:");
		instructionsView->append("Average Over Freq: Sensor measurement is an average of all valid readings at the channel frequencies used");
		instructionsView->append("Linear Fit: Constructs a linear fit of individual Sensor Codes as a function of channel frequencies. The sensor measuremement is the value of the fitted line at the center of the frequency band.");
		instructionsView->append("");
		instructionsView->append("Band Region: This sets the frequency band to be used, by regulatory region.");
		instructionsView->append("");
		instructionsView->append("Samples per Measurement: The number of valid sensor readings to be taken and averaged to produce a single measurment point on the result plot. Higher values result in more precise measurements but longer measurement time.");
		instructionsView->append("");
		instructionsView->append("Wet Threshold: The sensor code value above or below which a wet reading will be indicated");
		instructionsView->append("");
		instructionsView->append("Above/Below: Sets whether codes above or below the threshold will be considered wet.");
	}
	else if (helpType=="Temperature:MeasurementDetailsDialog")
	{
		instructionsView->setText("The table lists the results of the most recent completed measurement.");
		instructionsView->append("");
		instructionsView->append("Table fields:");
		instructionsView->append("");
		instructionsView->append("EPC: Complete electronic product code stored on the tag");
		instructionsView->append("");
		instructionsView->append("Label: Identifier used in the plot legend");
		instructionsView->append("");
		instructionsView->append("Model: Hexadecimal model code of the tag");
		instructionsView->append("");
		instructionsView->append("Cal: Y/N indicates that the tag contains or does not contain valid temperature calibration data");
		instructionsView->append("");
		instructionsView->append("Valid Reads: Number of times in the most recent measurement the tag was successfully read with the correct On-Chip RSSI");
		instructionsView->append("");
		instructionsView->append("Invalid Reads: Number of times in the most recent measurement the tag returned data, but with an error code or incorrect On-Chip RSSI");  
		instructionsView->append("");
		instructionsView->append("Temp (degC): The temperature in degrees C, averaged over all the valid reads comprising the measurement");    	
		instructionsView->append("");
		instructionsView->append("On-Chip RSSI: The average value of the On-Chip RSSI over all the valid reads comprising the measurement");    	          	    
	}
	else if (helpType=="Moisture:MeasurementDetailsDialog")
	{
		instructionsView->setText("The table lists the results of the most recent completed measurement.");
		instructionsView->append("");
		instructionsView->append("Table fields:");
		instructionsView->append("");
		instructionsView->append("EPC: Complete electronic product code stored on the tag");
		instructionsView->append("");
		instructionsView->append("Label: Identifier used in the plot legend");
		instructionsView->append("");
		instructionsView->append("Model: Hexadecimal model code of the tag");
		instructionsView->append("");
		instructionsView->append("Cal: Y/N indicates that the tag contains or does not contain valid temperature calibration data");
		instructionsView->append("");
		instructionsView->append("Valid Reads: Number of times in the most recent measurement the tag was successfully read with the correct On-Chip RSSI");
		instructionsView->append("");
		instructionsView->append("Invalid Reads: Number of times in the most recent measurement the tag returned data, but with an error code or incorrect On-Chip RSSI");   
		instructionsView->append("");
		instructionsView->append("Sensor Code: The measured sensor code, calculated by averaging or taking the value at band center of a linear fit");    	          	    
		instructionsView->append("");
		instructionsView->append("On-Chip RSSI: The average value of the On-Chip RSSI over all the valid reads comprising the measurement");    	          	    
	}
	else if (helpType=="RemoteOperationPage")
	{
		instructionsView->setText("Select the interface to connect through.");
		instructionsView->append("");
		instructionsView->append("Press <Start> to begin listening over that interface for commands.  Once a command is received, its type and the response being sent will be displayed.");
		instructionsView->append("");
		instructionsView->append("Press <Stop> to stop Hermes from listening over the interface selected");
		instructionsView->append("");
		instructionsView->append("<Main> returns to the main screen");
	}
	
	QVBoxLayout *instructionsLayout = new QVBoxLayout;
	instructionsLayout->addWidget(instructionsView);
	QFrame *instructionsFrame = new QFrame;
	instructionsFrame->setLayout(instructionsLayout);
	instructionsFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	instructionsFrame->setLineWidth(3);
	instructionsFrame->setMidLineWidth(3);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(instructionsFrame);
	setLayout(mainLayout);
}
CalibrationPage::CalibrationPage(KitModel *model, KitController *controller, QString demoType, QWidget *parent)
	: QWidget(parent)
{
	this->model = model;
	this->controller = controller;
	QLabel *titleLabel = new QLabel(tr("Calibration Definition"));
	QTabWidget *tabWidget = new QTabWidget;
	onePointTab = new OnePointTempCalTab(model, controller);
	tabWidget->addTab(onePointTab, tr("One-Point"));
	tabWidget->addTab(new TwoPointTempCalTab(model, controller), tr("Two-Point"));
	QVBoxLayout *calibrationLayout = new QVBoxLayout;
	calibrationLayout->addWidget(tabWidget);
	QFrame *calibrationFrame = new QFrame;
	calibrationFrame->setLayout(calibrationLayout);
	calibrationFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	calibrationFrame->setLineWidth(3);
	calibrationFrame->setMidLineWidth(3);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(titleLabel);
	mainLayout->addWidget(calibrationFrame);
	setLayout(mainLayout);
}
OnePointTempCalTab::OnePointTempCalTab(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	this->model = model;
	this->controller = controller;
	thread = new ChartThread;
	thread->initialize(controller, model);
	connect(thread, SIGNAL(tempCodeMeasuredSignal(float)), this, SLOT(tempCodeMeasuredSlot(float)));
	QLabel *condition1Label = new QLabel(tr("Condition 1"));
	QLabel *loadCondition1Label = new QLabel(tr("Temperature (degC)"));
	QLabel *sensorCodeCondition1Label = new QLabel(tr("Temperature Code"));
	temp1TextEdit = new QTextEdit;
	code1TextEdit = new QTextEdit;
	temp1TextEdit->setFixedHeight(30);
	code1TextEdit->setFixedHeight(30);
	QHBoxLayout *loadCondition1Layout = new QHBoxLayout;
	loadCondition1Layout->addWidget(loadCondition1Label);
	loadCondition1Layout->addWidget(temp1TextEdit);
	loadCondition1Layout->addSpacing(200);
	QWidget *loadCondition1Widget = new QWidget;
	loadCondition1Widget->setLayout(loadCondition1Layout);
	loadCondition1Widget->setMaximumSize(400, 200);
	QHBoxLayout *sensorCodeCondition1Layout = new QHBoxLayout;
	sensorCodeCondition1Layout->addWidget(sensorCodeCondition1Label);
	sensorCodeCondition1Layout->addSpacing(10);
	sensorCodeCondition1Layout->addWidget(code1TextEdit);
	sensorCodeCondition1Layout->addSpacing(200);
	QWidget *sensorCodeCondition1Widget = new QWidget;
	sensorCodeCondition1Widget->setLayout(sensorCodeCondition1Layout);
	sensorCodeCondition1Widget->setMaximumSize(400, 200);
	QVBoxLayout *condition1Layout = new QVBoxLayout;
	condition1Layout->addWidget(condition1Label);
	condition1Layout->addWidget(loadCondition1Widget);
	condition1Layout->addWidget(sensorCodeCondition1Widget);
	QHBoxLayout *conditionsLayout = new QHBoxLayout;
	conditionsLayout->addLayout(condition1Layout);
	QPushButton *readTempCodeButton = new QPushButton("Read Temp Code");
	QPushButton *writeCalButton = new QPushButton("Write Calibration");
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(readTempCodeButton);
	buttonLayout->addWidget(writeCalButton);
	buttonLayout->addStretch(1);
	QVBoxLayout *highestLayout = new QVBoxLayout;
	highestLayout->addLayout(conditionsLayout);
	highestLayout->addLayout(buttonLayout);
	connect(readTempCodeButton, SIGNAL(clicked()),
			this, SLOT(readTempCodeButtonClicked()));
	connect(writeCalButton, SIGNAL(clicked()),
			this, SLOT(writeCalButtonClicked()));
	setLayout(highestLayout);
}
bool OnePointTempCalTab::abortRequested()
{
	return thread->abortRequested();
}
void OnePointTempCalTab::readTempCodeButtonClicked()
{
	qDebug("Read Temp Code button clicked");
	this->setCursor(Qt::WaitCursor);
	thread->startCollection(TEMPCAL, 0);
}
void OnePointTempCalTab::tempCodeMeasuredSlot(float tempCode)
{
	qDebug("TempCode measured: %f", tempCode);
	this->setCursor(Qt::ArrowCursor);
	if (tempCode > 0)
		code1TextEdit->setText(QString::number(tempCode,'f',1));
	else
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("Calibration");
		msgBox.setText("Did not successfully read temperature tag");
		msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
		msgBox.exec();
	}
}
void OnePointTempCalTab::writeCalButtonClicked()
{
	qDebug("Write Cal button clicked");
	bool ok1;
	bool ok2;
	QString errorMessage="";
	QMessageBox msgBox;
	float code1=code1TextEdit->toPlainText().toFloat(&ok1);
	float temp1=temp1TextEdit->toPlainText().toFloat(&ok2);
	if (ok1==false || ok2==false)
		errorMessage="Temperature code and temperature must be numbers";
	else if (code1 < 0 || code1 > 4095)
		errorMessage="Temperature code must be between 0 and 4095";
	else if (temp1 < -80 || temp1 > 120)
		errorMessage="Temperature must be between -80 and 120";
	if (errorMessage!="")
	{
		msgBox.setWindowTitle("Error");
		msgBox.setText(errorMessage);
		msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
		msgBox.exec();
	}
	else
	{
		int tagIndex = model->selectedTempTagIndex();
		if (tagIndex==-1)
			return;
		QString epc = model->TempTagList[tagIndex].getEpc();
		QString calData = model->TempTagList[tagIndex].calculateTempCal1Point(code1, temp1);
		int result;
		for (int i=1;i<=3;i++)
		{
			result = model->writeDataToTag(epc, 3, 8, calData);
			if (result==0)
				break;
		}
		if (result != 0)
		{
			msgBox.setWindowTitle("Write Calibration");
			QString message = "Calibration write to tag " + epc + " failed";
			msgBox.setText(message);
			msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
			msgBox.exec();
		}
		else
		{
			msgBox.setWindowTitle("Write Calibration");
			QString message = "Calibration write to tag " + epc + " succeeded";
			msgBox.setText(message);
			msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
			msgBox.exec();
		}
	}
}
TwoPointTempCalTab::TwoPointTempCalTab(KitModel *model, KitController *controller, QWidget *parent)
	: QWidget(parent)
{
	this->model = model;
	this->controller = controller;
	thread = new ChartThread;
	thread->initialize(controller, model);
	connect(thread, SIGNAL(tempCodeMeasuredSignal(float)), this, SLOT(tempCodeMeasuredSlot(float)));
	QLabel *condition1Label = new QLabel(tr("Condition 1"));
	QLabel *loadCondition1Label = new QLabel(tr("Temperature (degC)"));
	QLabel *sensorCodeCondition1Label = new QLabel(tr("Temperature Code"));
	temp1TextEdit= new QTextEdit;
	code1TextEdit = new QTextEdit;
	temp1TextEdit->setFixedHeight(30);
	code1TextEdit->setFixedHeight(30);
	QHBoxLayout *loadCondition1Layout = new QHBoxLayout;
	loadCondition1Layout->addWidget(loadCondition1Label);
	loadCondition1Layout->addWidget(temp1TextEdit);
	QWidget *loadCondition1Widget = new QWidget;
	loadCondition1Widget->setLayout(loadCondition1Layout);
	QHBoxLayout *sensorCodeCondition1Layout = new QHBoxLayout;
	sensorCodeCondition1Layout->addWidget(sensorCodeCondition1Label);
	sensorCodeCondition1Layout->addSpacing(10);
	sensorCodeCondition1Layout->addWidget(code1TextEdit);
	QWidget *sensorCodeCondition1Widget = new QWidget;
	sensorCodeCondition1Widget->setLayout(sensorCodeCondition1Layout);
	QVBoxLayout *condition1Layout = new QVBoxLayout;
	condition1Layout->addWidget(condition1Label);
	condition1Layout->addWidget(loadCondition1Widget);
	condition1Layout->addWidget(sensorCodeCondition1Widget);
	QLabel *condition2Label = new QLabel(tr("Condition 2"));
	QLabel *loadCondition2Label = new QLabel(tr("Temperature (degC)"));
	QLabel *sensorCodeCondition2Label = new QLabel(tr("Temperature Code"));
	temp2TextEdit = new QTextEdit;
	code2TextEdit= new QTextEdit;
	temp2TextEdit->setFixedHeight(30);
	code2TextEdit->setFixedHeight(30);
	QHBoxLayout *loadCondition2Layout = new QHBoxLayout;
	loadCondition2Layout->addWidget(loadCondition2Label);
	loadCondition2Layout->addWidget(temp2TextEdit);
	QWidget *loadCondition2Widget = new QWidget;
	loadCondition2Widget->setLayout(loadCondition2Layout);
	loadCondition2Widget->setMaximumSize(400, 200);
	QHBoxLayout *sensorCodeCondition2Layout = new QHBoxLayout;
	sensorCodeCondition2Layout->addWidget(sensorCodeCondition2Label);
	sensorCodeCondition2Layout->addSpacing(10);
	sensorCodeCondition2Layout->addWidget(code2TextEdit);
	QWidget *sensorCodeCondition2Widget = new QWidget;
	sensorCodeCondition2Widget->setLayout(sensorCodeCondition2Layout);
	QVBoxLayout *condition2Layout = new QVBoxLayout;
	condition2Layout->addWidget(condition2Label);
	condition2Layout->addWidget(loadCondition2Widget);
	condition2Layout->addWidget(sensorCodeCondition2Widget);
	QHBoxLayout *conditionsLayout = new QHBoxLayout;
	conditionsLayout->addLayout(condition1Layout);
	conditionsLayout->addLayout(condition2Layout);
	QPushButton *readTempCode1Button = new QPushButton("Read Temp Code 1");
	QPushButton *readTempCode2Button = new QPushButton("Read Temp Code 2");
	QPushButton *writeCalButton = new QPushButton("Write Calibration");
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(readTempCode1Button);
	buttonLayout->addWidget(readTempCode2Button);
	buttonLayout->addWidget(writeCalButton);
	buttonLayout->addStretch(1);
	QVBoxLayout *highestLayout = new QVBoxLayout;
	highestLayout->addLayout(conditionsLayout);
	highestLayout->addLayout(buttonLayout);
	connect(readTempCode1Button, SIGNAL(clicked()),
			this, SLOT(readTempCode1ButtonClicked()));
	connect(readTempCode2Button, SIGNAL(clicked()),
			this, SLOT(readTempCode2ButtonClicked()));
	connect(writeCalButton, SIGNAL(clicked()),
			this, SLOT(writeCalButtonClicked()));
	setLayout(highestLayout);
}
void TwoPointTempCalTab::readTempCode1ButtonClicked()
{
	readingCode1=true;
	this->setCursor(Qt::WaitCursor);
	thread->startCollection(TEMPCAL, 0);
}
void TwoPointTempCalTab::readTempCode2ButtonClicked()
{
	readingCode1=false;
	this->setCursor(Qt::WaitCursor);
	thread->startCollection(TEMPCAL, 0);
}
void TwoPointTempCalTab::writeCalButtonClicked()
{
	bool ok1;
	bool ok2;
	bool ok3;
	bool ok4;
	QString errorMessage="";
	QMessageBox msgBox;
	float code1=code1TextEdit->toPlainText().toFloat(&ok1);
	float temp1=temp1TextEdit->toPlainText().toFloat(&ok2);
	float code2=code2TextEdit->toPlainText().toFloat(&ok3);
	float temp2=temp2TextEdit->toPlainText().toFloat(&ok4);
	if (ok1==false || ok2==false || ok3==false || ok4==false)
		errorMessage="Temperature codes and temperatures must be numbers";
	else if (code1 < 0 || code1 > 4095 || code2 < 0 || code2 > 4095)
		errorMessage="Temperature codes must be between 0 and 4095";
	else if (temp1 < -80 || temp1 > 120 || temp2 < -80 || temp2 > 120)
		errorMessage="Temperatures must be between -80 and 120";
	if (errorMessage!="")
	{
		msgBox.setWindowTitle("Error");
		msgBox.setText(errorMessage);
		msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
		msgBox.exec();
	}
	else
	{
		int tagIndex = model->selectedTempTagIndex();
		if (tagIndex==-1)
			return;
		QString epc = model->TempTagList[tagIndex].getEpc();
		QString calData = model->TempTagList[tagIndex].calculateTempCal2Point(code1, temp1, code2, temp2);
		int result;
		for (int i=1;i<=3;i++)
		{
			result = model->writeDataToTag(epc, 3, 8, calData);
			if (result==0)
				break;
		}
		if (result != 0)
		{
			msgBox.setWindowTitle("Write Calibration");
			QString message = "Calibration write to tag " + epc + " failed";
			msgBox.setText(message);
			msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
			msgBox.exec();
		}
		else
		{
			msgBox.setWindowTitle("Write Calibration");
			QString message = "Calibration write to tag " + epc + " succeeded";
			msgBox.setText(message);
			msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
			msgBox.exec();
		}
	}
}
bool TwoPointTempCalTab::abortRequested()
{
	return thread->abortRequested();
}
void TwoPointTempCalTab::tempCodeMeasuredSlot(float tempCode)
{
	qDebug("TempCode measured: %f", tempCode);
	this->setCursor(Qt::ArrowCursor);
	if (tempCode < 0)
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("Calibration");
		msgBox.setText("Did not successfully read temperature tag");
		msgBox.setWindowIcon(QIcon(APPLICATION_ICON));
		msgBox.exec();
		return;
	}
	if (readingCode1)
		code1TextEdit->setText(QString::number(tempCode,'f',1));
	else
		code2TextEdit->setText(QString::number(tempCode,'f',1));
}
