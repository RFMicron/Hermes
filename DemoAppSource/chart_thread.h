/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// chart_thread.h
/// This class implements a thread in Qt lauched from the GUI when the Start button
/// is pressed to offload the collection of temperature or sensor code data from a 
/// tag from the GUI thread.
/// 
/// Author: Frank Miranda and Greg Pitner, RFMicron
///-----------------------------------------------------------------------------

#ifndef _CHART_THREAD_H_
#define _CHART_THREAD_H_

#include "kit_controller.h"
#include "kit_model.h"
#include "sensorTag.h"
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

enum CollectionType { TEMPERATURE = 1, MOISTURE, TEMPCAL };

Q_DECLARE_METATYPE(QList<SensorTag>);
Q_DECLARE_METATYPE(QList<QDateTime>);

class ChartThread : public QThread
{
	Q_OBJECT
	private:
		KitController *controller;
		KitModel *model;
		QMutex mutex;
		QWaitCondition condition;
		CollectionType collectionType;
		int measurementPeriod; // Desired measurement period in seconds
	protected:
		void run() Q_DECL_OVERRIDE;
	signals:
		void tempTagsFoundSignal(QList<SensorTag>);
		void tempTagsMeasuredSignal(QList<SensorTag>, QList<QDateTime>);
		void moistTagsFoundSignal(QList<SensorTag>);
		void moistTagsMeasuredSignal(QList<SensorTag>, QList<QDateTime>);
		void tempCodeMeasuredSignal(float code);
	public:
		ChartThread(QObject *parent = 0);
		~ChartThread();
		bool abort;
		void initialize(KitController *controller, KitModel *model);
		short startCollection(CollectionType type, int period);
		void stopCollection(CollectionType type);
};
#endif 
