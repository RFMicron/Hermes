/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// chart.h
/// This class allows you to plot temperature and sensor code data for tag(s)
/// read through the AMS Radon reader.  It also allows you to maintain a show 
/// a history of the data collected in the plot for a configurable length of time.
/// 
/// Author: Greg Pitner, RFMicron
///-----------------------------------------------------------------------------

#ifndef CHART_H
#define CHART_H

#include <qwt_plot.h>
#include <qwt_plot_textlabel.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_symbol.h>
#include "sensorTag.h"
#include "kit_controller.h"
#include "kit_model.h"

#define HISTORY 60 // seconds

class QwtPlotCurve;

class XAxisScaleDraw: public QwtScaleDraw
{
	public:
		QList<QString> xLabels;
		XAxisScaleDraw(QList<QString> x);
		XAxisScaleDraw();
		void setXLabels(QList<QString> x);
		virtual QwtText label(double v) const;
};
class Chart : public QwtPlot
{
	Q_OBJECT
	public:
		enum CpuData
		{
			User,
			System,
			NCpuData
		};
		Chart(QString plotType, KitModel *model, KitController *controller);
		int MaxNumberOfPlotPoints;
	public Q_SLOTS:
		void setTempCurvesSlot(QList<SensorTag> TempTagList);
		void setMoistCurvesSlot(QList<SensorTag> MoistTagList);
		void updateTempCurvesSlot(QList<SensorTag> TempTagList, QList<QDateTime> TempMeasTimeList);
		void updateMoistCurvesSlot(QList<SensorTag> MoistTagList, QList<QDateTime> MoistMeasTimeList);
		private Q_SLOTS:
		void legendChecked( const QVariant &, bool on );
	private:
		struct curveStruct
		{
			QwtPlotCurve* curvePointer;
			QwtSymbol* symbolPointer;
			QString curveLabel;
		};
		QList<curveStruct> curveInfo;
		int dataCount;
		QString plotType;
		QwtLegend *legend;
		XAxisScaleDraw* scaleDrawPtr;
		KitModel *model;
		KitController *controller;
		int curveInfoIndex(QString label);
		void clearPlot();
};
#endif
