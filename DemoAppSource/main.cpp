#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QTimer>
#include <QLibraryInfo>
#include <QSplashScreen>
#include <QProcess>
#include "hermes.h"
#include "kit_model.h"
#include "kit_controller.h"
#include <QTextStream>

int main(int argc, char *argv[])
{	
	QFile runningFile("running");
	if (runningFile.exists()==false)
	{
		runningFile.open(QIODevice::ReadWrite);
		runningFile.close();
	}
	else
	{
		QProcess process;
		process.start("ps -e");
		process.waitForFinished(-1);
		QString output = process.readAllStandardOutput();
		QStringList processList = output.split('\n');
		int numberOfHermesProcesses=0;
		for (int i=0; i<processList.length(); i++)
		{
			QStringList processFields=processList[i].split(" ", QString::SkipEmptyParts);
			if (processFields.length()==4)
			{
				if (processFields[3]=="hermes")
					numberOfHermesProcesses++;
			}
		}
		if (numberOfHermesProcesses>1)
		{
			qDebug("INSTANCE ALREADY RUNNING");
			return 0;
    	    	}
    	}	
	KitModel *model = new KitModel;
	KitController *controller = new KitController(model, argc, argv);
	controller->initialize();
	return controller->run();
}
