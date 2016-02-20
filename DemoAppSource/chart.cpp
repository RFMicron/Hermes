#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_canvas.h>
#include "chart.h"

class Background: public QwtPlotItem
{
	public:
		Background()
		{
			setZ( 0.0 );
		}
		virtual int rtti() const
		{
			return QwtPlotItem::Rtti_PlotUserItem;
		}
		virtual void draw( QPainter *painter,
				const QwtScaleMap &, const QwtScaleMap &yMap,
				const QRectF &canvasRect ) const
		{
			QColor c( Qt::white );
			QRectF r = canvasRect;
			for ( int i = 100; i > 0; i -= 10 )
			{
				r.setBottom( yMap.transform( i - 10 ) );
				r.setTop( yMap.transform( i ) );
				painter->fillRect( r, c );
				c = c.dark( 110 );
			}
		}
};
class CpuCurve: public QwtPlotCurve
{
	public:
		CpuCurve( const QString &title ):
			QwtPlotCurve( title )
	{
		setRenderHint( QwtPlotItem::RenderAntialiased );
	}
		void setColor( const QColor &color )
		{
			QColor c = color;
			c.setAlpha( 150 );
			setPen( QPen( Qt::NoPen ) );
			setBrush( c );
		}
};
XAxisScaleDraw::XAxisScaleDraw(QList<QString> x)
{
	xLabels=x;
}
void XAxisScaleDraw::setXLabels(QList<QString> x)
{
	xLabels=x;
}
QwtText XAxisScaleDraw::label(double v) const
{
	int vint = (int)v;
	if (vint != v)
	{
		return QString("");
	}
	if (vint < 0 || vint >= xLabels.count())
	{
		return QString("");
	}
	return xLabels[vint];
}
Chart::Chart(QString typeOfPlot, KitModel *model, KitController *controller):
	QwtPlot()
{
	this->model = model;
	this->controller = controller;
	plotType=typeOfPlot;
	MaxNumberOfPlotPoints=25;
	setAutoReplot( false );
	if(plotType == "Moisture")
	{
		QwtText ytitle("Sensor Code");
		ytitle.setFont(QFont("Helvetica", 10));
		setAxisTitle( QwtPlot::yLeft, ytitle );
		QwtText xtitle("Time (seconds)");
		xtitle.setFont(QFont("Helvetica", 10));
		setAxisTitle( QwtPlot::xBottom, xtitle );
		setAxisScale( QwtPlot::yLeft, 0, 31 );
	} 
	else if(plotType == "Temperature")
	{
		QwtText ytitle("Temperature (deg C)");
		ytitle.setFont(QFont("Helvetica", 10));
		setAxisTitle( QwtPlot::yLeft, ytitle );
		QwtText xtitle("Time (seconds)");
		xtitle.setFont(QFont("Helvetica", 10));
		setAxisTitle( QwtPlot::xBottom, xtitle );
		setAxisScale( QwtPlot::yLeft, 0, 50 );
	}
	setAxisScale(QwtPlot::xBottom, 0, MaxNumberOfPlotPoints-1);
	legend = new QwtLegend;
	legend->setDefaultItemMode( QwtLegendData::Checkable );
	insertLegend( legend, QwtPlot::RightLegend );
	QList<QString> xVal;
	xVal.append("");
	scaleDrawPtr = new XAxisScaleDraw(xVal);
	connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),
			SLOT( legendChecked( const QVariant &, bool ) ) );
}
int Chart::curveInfoIndex(QString label)
{
	int index;
	for (index=0; index<curveInfo.length(); index++)
	{
		if (curveInfo[index].curveLabel == label)
			return index;
	}
	return -1;
}
void Chart::setTempCurvesSlot(QList<SensorTag> TempTagList)
{
	qDebug("setTempCurvesSlot");
	if (TempTagList.length()==0)
	{
		qDebug("clearing plot");
		clearPlot();
		return;
	}
	QColor curveColors[] = {QColor(0,0,255), QColor(255,0,0), QColor(0,200,0), QColor(50,200,200), QColor(250,150,50)};
	int numColors=5;
	for (int i=0; i < TempTagList.length(); i++)
	{
		if (curveInfoIndex(TempTagList[i].Label)>=0)
			continue; // Tag is already in the curveInfo list
		curveStruct c;
		c.curveLabel = TempTagList[i].Label;
		c.curvePointer = new QwtPlotCurve(TempTagList[i].Label);
		c.curvePointer->setPen(QPen(curveColors[i % numColors]));
		c.curvePointer->setRenderHint( QwtPlotItem::RenderAntialiased );
		c.symbolPointer=new QwtSymbol(QwtSymbol::Ellipse, QBrush(curveColors[i % numColors]), QPen(curveColors[i % numColors]), QSize(5,5));
		c.curvePointer->setSymbol(c.symbolPointer);
		c.curvePointer->attach(this);
		curveInfo.append(c);
		QwtPlotItemList plotItems = itemList();
		for (int i=0;i<plotItems.size();i++)
		{
			const QVariant itemInfo = itemToInfo(plotItems[i]);
			QwtLegendLabel *legendLabel = qobject_cast<QwtLegendLabel *>(legend->legendWidget(itemInfo));
			if (legendLabel)
			{
				if (legendLabel->text().text()==TempTagList[i].Label)
					legendLabel->setChecked(true);
			}
		}	
	}
	replot();
	qDebug("setTempCurvesSlot ending");
}
void Chart::setMoistCurvesSlot(QList<SensorTag> MoistTagList)
{
	qDebug("setMoistCurvesSlot");
	if (MoistTagList.length()==0)
	{
		qDebug("clearing plot");
		clearPlot();
		return;
	}
	QColor curveColors[] = {QColor(0,0,255), QColor(255,0,0), QColor(0,200,0), QColor(50,200,200), QColor(250,150,50)};
	int numColors=5;
	for (int i=0; i < MoistTagList.length(); i++)
	{
		if (curveInfoIndex(MoistTagList[i].Label)>=0)
			continue; // Tag is already in the curveInfo list
		curveStruct c;
		c.curveLabel = MoistTagList[i].Label;
		c.curvePointer = new QwtPlotCurve(MoistTagList[i].Label);
		c.curvePointer->setPen(QPen(curveColors[i % numColors]));
		c.curvePointer->setRenderHint( QwtPlotItem::RenderAntialiased );
		c.symbolPointer=new QwtSymbol(QwtSymbol::Ellipse, QBrush(curveColors[i % numColors]), QPen(curveColors[i % numColors]), QSize(5,5));
		c.curvePointer->setSymbol(c.symbolPointer);
		c.curvePointer->attach(this);
		curveInfo.append(c);
		QwtPlotItemList plotItems = itemList();
		for (int i=0;i<plotItems.size();i++)
		{
			const QVariant itemInfo = itemToInfo(plotItems[i]);
			QwtLegendLabel *legendLabel = qobject_cast<QwtLegendLabel *>(legend->legendWidget(itemInfo));
			if (legendLabel)
			{
				if (legendLabel->text().text()==MoistTagList[i].Label)
					legendLabel->setChecked(true);
			}
		}	
	}
	replot();
	qDebug("setMoistCurvesSlot ending");
}
void Chart::updateTempCurvesSlot(QList<SensorTag> TempTagList, QList<QDateTime> TempMeasTimeList)
{
	qDebug("updateTempCurvesSlot");
	int MeasTimeListLength = TempMeasTimeList.length();
	if (MeasTimeListLength==0)
		return;
	setAxisAutoScale(QwtPlot::yLeft);
	QDateTime firstMeasTime = TempMeasTimeList[0];
	int earliestMeasNumber = MeasTimeListLength - MaxNumberOfPlotPoints;
	if (earliestMeasNumber < 0)
		earliestMeasNumber=0;
	QDateTime earliestPlottedTime = TempMeasTimeList[earliestMeasNumber];
	QDateTime latestPlottedTime = TempMeasTimeList[MeasTimeListLength-1];
	float earliestSeconds = 0.001*firstMeasTime.msecsTo(earliestPlottedTime);
	float latestSeconds = 0.001*firstMeasTime.msecsTo(latestPlottedTime);
	if (earliestSeconds==latestSeconds)
		latestSeconds=earliestSeconds+10;
	for (int t=0; t < TempTagList.length(); t++)
	{
		int c;
		for (c=0; c < curveInfo.length(); c++)
		{
			if (TempTagList[t].Label==curveInfo[c].curveLabel)
				break;
		}
		if (c==curveInfo.length())
			setTempCurvesSlot(TempTagList); // There is a tag in the list for which there is no curve
		QPolygonF points;
		int historyLength = TempTagList[t].TemperatureMeasurementHistory.length();
		if (historyLength > 0)
		{
			for (int m=historyLength-1;m>=0;m--)
			{
				int measNumber = TempTagList[t].TemperatureMeasurementHistory[m].getNumber();
				if (measNumber < earliestMeasNumber)
					break;
				float val = TempTagList[t].TemperatureMeasurementHistory[m].getValue();
				if (val > -1000)
				{
					QDateTime measTime=TempMeasTimeList[measNumber];
					float seconds = 0.001*firstMeasTime.msecsTo(measTime);
					points << QPointF(seconds, val);
				}
			}
			curveInfo[c].curvePointer->setSamples(points);
		}
	}
	updateAxes();
	QwtScaleDiv scaleDiv = axisScaleDiv(QwtPlot::yLeft);
	double lowerBound = scaleDiv.lowerBound();
	double upperBound = scaleDiv.upperBound();
	if (upperBound-lowerBound<10)
	{
		double center = 0.5*(lowerBound+upperBound);
		setAxisScale( QwtPlot::yLeft, center-5, center+5);
	}
	setAxisScale( QwtPlot::xBottom, earliestSeconds, latestSeconds);
	replot();
}
void Chart::updateMoistCurvesSlot(QList<SensorTag> MoistTagList, QList<QDateTime> MoistMeasTimeList)
{
	qDebug("updateMoistCurvesSlot");
	int MeasTimeListLength = MoistMeasTimeList.length();
	if (MeasTimeListLength==0)
		return;
	QDateTime firstMeasTime = MoistMeasTimeList[0];
	int earliestMeasNumber = MeasTimeListLength - MaxNumberOfPlotPoints;
	if (earliestMeasNumber < 0)
		earliestMeasNumber=0;
	QDateTime earliestPlottedTime = MoistMeasTimeList[earliestMeasNumber];
	QDateTime latestPlottedTime = MoistMeasTimeList[MeasTimeListLength-1];
	float earliestSeconds = 0.001*firstMeasTime.msecsTo(earliestPlottedTime);
	float latestSeconds = 0.001*firstMeasTime.msecsTo(latestPlottedTime);
	if (earliestSeconds==latestSeconds)
		latestSeconds=earliestSeconds+10;
	bool SelectedM3TagsInList=false;
	for (int t=0; t < MoistTagList.length(); t++)
	{
		if (MoistTagList[t].getModel().left(2)=="03" && MoistTagList[t].SelectedForMeasurement)
		{
			SelectedM3TagsInList=true;
		}
		int c;
		for (c=0; c < curveInfo.length(); c++)
		{
			if (MoistTagList[t].Label==curveInfo[c].curveLabel)
				break;
		}
		if (c==curveInfo.length())
			setMoistCurvesSlot(MoistTagList); // There is a tag in the list for which there is no curve
		QPolygonF points;
		int historyLength = MoistTagList[t].SensorMeasurementHistory.length();
		if (historyLength > 0)
		{
			for (int m=historyLength-1;m>=0;m--)
			{
				int measNumber = MoistTagList[t].SensorMeasurementHistory[m].getNumber();
				if (measNumber < earliestMeasNumber)
					break;
				float val = MoistTagList[t].SensorMeasurementHistory[m].getValue();
				if (val > -1000)
				{
					QDateTime measTime=MoistMeasTimeList[measNumber];
					float seconds = 0.001*firstMeasTime.msecsTo(measTime);
					points << QPointF(seconds, val);
				}
			}
			curveInfo[c].curvePointer->setSamples(points);
		}
	}
	setAxisScale( QwtPlot::xBottom, earliestSeconds, latestSeconds);
	if (SelectedM3TagsInList)
	{
		setAxisScale( QwtPlot::yLeft, 0, 510);
	}
	else
	{
		setAxisScale( QwtPlot::yLeft, 0, 35);
	}
	replot();
}
void Chart::clearPlot()
{
	for (int i=0;i<curveInfo.length();i++)
	{	
		curveInfo[i].curvePointer->detach();	
		delete curveInfo[i].curvePointer;
	}
	curveInfo.clear();
	replot();
}
void Chart::legendChecked( const QVariant &itemInfo, bool on )
{
	QwtPlotItem *plotItem = infoToItem( itemInfo );
	if ( plotItem )
	{
		plotItem->setVisible(on);
		QString label = plotItem->title().text();
		qDebug("%s clicked", qPrintable(label));
		model->selectForMeasurement(plotType, label, on);
	}
	replot();
}
