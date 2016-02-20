#include "gui_view.h"
#include "kit_controller.h"
#include <QTimer>

GUIView::GUIView(KitController *controller, KitModel *model)
{
	this->controller = controller;
	this->model = model;
	splash = new QSplashScreen;
	wizard = new Hermes(model, controller);
	wizard->setStyleSheet("background-color:#E8E8E8");
	wizard->setWindowIcon(QIcon(APPLICATION_ICON));
}
int GUIView::initialize()
{
	Q_INIT_RESOURCE(hermes);
	splash->setPixmap(QPixmap(SPLASH_SCREEN_ICON));
	connect(model, SIGNAL(antennaTuningSignal(int, int)), this, SLOT(antennaTuningSlot(int, int)));
	model->registerTemperatureObserver(this);
	model->registerMoistureObserver(this);
	return 0;
}
void GUIView::showSplashScreen()
{
	splash->show();
}
void GUIView::closeSplashScreen()
{
	splash->close();	
}
void GUIView::antennaTuningSlot(int currentFreq, int totalFreq)
{
	int progress = ((float)currentFreq/(float)totalFreq)*100;
	splash->showMessage("Tuning reader: " + QString::number(progress) + "% done, please wait...");
	if (currentFreq==totalFreq)
		disconnect(model, SIGNAL(antennaTuningSignal(int, int)), this, SLOT(antennaTuningSlot(int, int)));
}
void GUIView::showMsgOnSplashScreen(QString msg)
{
	splash->showMessage(msg);	
}
void GUIView::show()
{
	wizard->show();
}
