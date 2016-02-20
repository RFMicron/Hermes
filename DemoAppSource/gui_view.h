/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// gui_view.h
/// This class implements the Graphical User Interface of the kit.  It encapsulates
/// the resources and allows you to initialize them.  It has also 'connections' to 
/// the Controller and Model modules which control the AMS Radon reader and maintain
/// its state, respectively.  These connections allows the GUI to control the reader
/// upon user requests and retrieve reader settings and results of tag reads.
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _GUI_VIEW_H_
#define _GUI_VIEW_H_

#include "kit_model.h"
#include "kit_controller.h"
#include "hermes.h"
#include <QSplashScreen>
#include <QString>
#include <QObject>

class KitModel;
class KitController;
class Hermes;

class GUIView : public QObject
{
	Q_OBJECT
	private:
		KitModel *model;
		KitController *controller;
		QSplashScreen *splash;
	public:
		GUIView(KitController *controller, KitModel *model);
		Hermes *wizard;
		int initialize();
		void showSplashScreen();
		void closeSplashScreen();
		void showMsgOnSplashScreen(QString msg);
		void show();
	public slots:
		void antennaTuningSlot(int, int);
};
#endif

