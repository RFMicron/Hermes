#include "chart_thread.h"

ChartThread::ChartThread(QObject *parent) : QThread(parent)
{
	abort = false;
}
ChartThread::~ChartThread()
{
	if(!isRunning())
	{
		if(collectionType == TEMPERATURE)
			stopCollection(TEMPERATURE);
		if(collectionType == MOISTURE)
			stopCollection(MOISTURE);
		if (collectionType == TEMPCAL)
			stopCollection(TEMPCAL);
	}
}
void ChartThread::initialize(KitController *controller, KitModel *model)
{
	this->controller = controller;
	this->model = model;
}
short ChartThread::startCollection(CollectionType type, int period)
{
	if(!isRunning())
	{
		abort = false;
		model->setAbort(false);
		collectionType = type;
		measurementPeriod = period;
		start(LowPriority);
	}
	else
		qDebug("thread already running\n");
	return 0;
}
void ChartThread::stopCollection(CollectionType type)
{
	if(collectionType == type)
	{
		mutex.lock();
		abort = true;
		model->setAbort(true);
		mutex.unlock();
		wait();
	}
	else
		qDebug("collection type not running\n");
}
void ChartThread::run()
{
	if (collectionType == TEMPERATURE)
	{
		qDebug("calling searchForTempTags()");
		controller->searchForTempTags();
		emit tempTagsFoundSignal(model->TempTagList);
		while(abort != true)
		{
			QDateTime measureStartTime = QDateTime::currentDateTime();
			qDebug("Measurement starting\n");
			controller->measureTempTags();	
			if (abort==true)
				break;
			emit tempTagsMeasuredSignal(model->TempTagList, model->TempMeasTimeList);
			QThread::msleep(100);
			QDateTime nextStartTime = measureStartTime.addSecs(measurementPeriod);
			int msecondsToNextMeas = QDateTime::currentDateTime().msecsTo(nextStartTime);
			while (msecondsToNextMeas>300)
			{
				QThread::msleep(250);
				if (abort == true)
					break;
				msecondsToNextMeas = QDateTime::currentDateTime().msecsTo(nextStartTime);
			}	
		}
	}
	if (collectionType == MOISTURE)
	{
		qDebug("calling searchForMoistTags()");
		controller->searchForMoistureTags();
		emit moistTagsFoundSignal(model->MoistTagList);
		while(abort != true)
		{
			QDateTime measureStartTime = QDateTime::currentDateTime();
			qDebug("Measurement starting\n");
			controller->measureMoistureTags();	
			if (abort==true)
				break;
			emit moistTagsMeasuredSignal(model->MoistTagList, model->MoistMeasTimeList);
			QThread::msleep(100);
			QDateTime nextStartTime = measureStartTime.addSecs(measurementPeriod);
			int msecondsToNextMeas = QDateTime::currentDateTime().msecsTo(nextStartTime);
			while (msecondsToNextMeas>300)
			{
				QThread::msleep(250);
				if (abort == true)
					break;
				msecondsToNextMeas = QDateTime::currentDateTime().msecsTo(nextStartTime);
			}
		}
	}
	if (collectionType == TEMPCAL)
	{
		qDebug("Starting temperature calibration process");
		float tempCode = controller->measureTempCodeForCalibration();
		emit tempCodeMeasuredSignal(tempCode);
	}
	qDebug("Thread stopped\n");
	return;
}
