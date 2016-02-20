#include <vector>
#include <iostream>
#include "kit_model.h"
#include "gui_view.h"
#include "utilityFunctions.h"
#include "ams_radon_reader.h"

using namespace std;

KitModel::KitModel()
{
	reader = new AMSRadonReader("/dev/ttyO4");
	gpio7 = new GPIO(7);
}
void KitModel::turnReaderOn()
{
	gpio7->setValue(GPIO::HIGH);	
}
void KitModel::turnReaderOff()
{
	gpio7->setValue(GPIO::LOW);	
}
void KitModel::initialize()
{
	FCCBandFreqs[0] = 902750; FCCBandFreqs[10] = 920750; FCCBandFreqs[20] = 908250; FCCBandFreqs[30] = 917250; FCCBandFreqs[40] = 907250;	
	FCCBandFreqs[1] = 924250; FCCBandFreqs[11] = 910250; FCCBandFreqs[21] = 904750; FCCBandFreqs[31] = 922250; FCCBandFreqs[41] = 914250;	
	FCCBandFreqs[2] = 926750; FCCBandFreqs[12] = 909750; FCCBandFreqs[22] = 919750; FCCBandFreqs[32] = 916750; FCCBandFreqs[42] = 904250;
	FCCBandFreqs[3] = 913750; FCCBandFreqs[13] = 918750; FCCBandFreqs[23] = 924750; FCCBandFreqs[33] = 909250; FCCBandFreqs[43] = 921250;
	FCCBandFreqs[4] = 916250; FCCBandFreqs[14] = 907750; FCCBandFreqs[24] = 903250; FCCBandFreqs[34] = 913250; FCCBandFreqs[44] = 923250;
	FCCBandFreqs[5] = 915250; FCCBandFreqs[15] = 926250; FCCBandFreqs[25] = 911250; FCCBandFreqs[35] = 921750; FCCBandFreqs[45] = 912750;
	FCCBandFreqs[6] = 906750; FCCBandFreqs[16] = 917750; FCCBandFreqs[26] = 905250; FCCBandFreqs[36] = 920250; FCCBandFreqs[46] = 925250;
	FCCBandFreqs[7] = 912250; FCCBandFreqs[17] = 906250; FCCBandFreqs[27] = 918250; FCCBandFreqs[37] = 923750; FCCBandFreqs[47] = 910750;
	FCCBandFreqs[8] = 922750; FCCBandFreqs[18] = 925750; FCCBandFreqs[28] = 905750; FCCBandFreqs[38] = 908750; FCCBandFreqs[48] = 915750;
	FCCBandFreqs[9] = 919250; FCCBandFreqs[19] = 903750; FCCBandFreqs[29] = 927250; FCCBandFreqs[39] = 911750; FCCBandFreqs[49] = 914750;
	ETSIBandFreqs[0] = 865700; ETSIBandFreqs[1] = 866900; ETSIBandFreqs[2] =  866300; ETSIBandFreqs[3] = 867500;
	PRCBandFreqs[0] = 920625; PRCBandFreqs[4] = 922875; PRCBandFreqs[8] = 921625; PRCBandFreqs[12] = 923625;
	PRCBandFreqs[1] = 922625; PRCBandFreqs[5] = 920875; PRCBandFreqs[9] = 923875; PRCBandFreqs[13] = 922125;
	PRCBandFreqs[2] = 923375; PRCBandFreqs[6] = 921875; PRCBandFreqs[10] = 921125; PRCBandFreqs[14] = 924125;
	PRCBandFreqs[3] = 922375; PRCBandFreqs[7] = 923125; PRCBandFreqs[11] = 921375; PRCBandFreqs[15] = 924375;
	JPNBandFreqs[0] = 916800; JPNBandFreqs[2] = 920600; JPNBandFreqs[4] = 919200;
	JPNBandFreqs[1] = 920800; JPNBandFreqs[3] = 918000; JPNBandFreqs[5] = 920400;
	TempTargetOnChipRssiMin=0;
	TempTargetOnChipRssiMax=31;
	MoistTargetOnChipRssiMin=0;
	MoistTargetOnChipRssiMax=31;
	tempAutoPower = false;
	moistAutoPower = false;
	tempMaxPower = 6;
	moistMaxPower = 6;
	tempMinSamplesPerMeas = 5;
	moistMinSamplesPerMeas = 5;
	gpio7->setDirection(GPIO::OUTPUT);	
	qDebug("kit_model initialized");
}
void KitModel::registerTemperatureObserver(GUIView *gui)
{
	tempObserver = gui;
}
void KitModel::registerMoistureObserver(GUIView *gui)
{
	moistureObserver = gui;
}
void KitModel::selectForMeasurement(QString measurementType, QString tagLabel, bool select)
{
	if (measurementType=="Temperature")
	{
		for (int t=0;t<TempTagList.length();t++)
		{
			if (tagLabel==TempTagList[t].Label)
			{
				TempTagList[t].SelectedForMeasurement=select;
				emit updateTempTagSelectionsSignal();
				return;
			}
		}
	}
	if (measurementType=="Moisture")
	{
		for (int t=0;t<MoistTagList.length();t++)
		{
			if (tagLabel==MoistTagList[t].Label)
			{
				MoistTagList[t].SelectedForMeasurement=select;
				emit updateMoistTagSelectionsSignal();
				return;
			}
		}
	}
}
void KitModel::clearTags(QString measurementType)
{
	if (measurementType=="Temperature")
	{
		TempTagList.clear();
		TempMeasTimeList.clear();
		emit updateTempTagsSignal(TempTagList);
	}
	if (measurementType=="Moisture")
	{
		MoistTagList.clear();
		MoistMeasTimeList.clear();
		emit updateMoistTagsSignal(MoistTagList);
	}	
}
int KitModel::initializeReader()
{
	char status;
	int initStatus = reader->initialize();
	if (initStatus != 0)
		return initStatus;
	status = reader->clearListOfSelectCommands();
	if(status != 0)
		return status;
	char storedAntennaID;
	status = reader->setAntennaID(2, storedAntennaID);
	if(status != 0)
		return status;
	signed char storedAntennaSensitivity;
	status = reader->setAntennaSensitivity((signed char)(-80), storedAntennaSensitivity);
	if(status != 0)
		return status;
	char reg0x00Value;
	status = reader->readAS3993Reg(0x00, &reg0x00Value);
	if(status != 0)
		return status;
	reg0x00Value &= ~(0x04);
	reg0x00Value |= 0x04;   // Enable AGC (?)
	char operationStatus;
	status = reader->writeToAS3993Reg(0x00, reg0x00Value, operationStatus);
	if(status != 0)
		return status;
	reg0x00Value = 0;
	status  = reader->readAS3993Reg(0x00, &reg0x00Value);
	if(status != 0)
		return status;
	char reg0x15Value;
	status = reader->readAS3993Reg(0x15, &reg0x15Value);
	if(status != 0)
		return status;
	reg0x15Value &= 0xE0;
	reg0x15Value |= 0x17;
	status = reader->writeToAS3993Reg(0x15, reg0x15Value, operationStatus);
	if(status != 0)
		return status;
	reg0x15Value = 0;
	status = reader->readAS3993Reg(0x15, &reg0x15Value);
	if(status != 0)
		return status;
	char storedLinkFreq;
	status = reader->setLinkFrequency(6, storedLinkFreq);
	if(status != 0)
		return status;
	char storedCoding;
	status = reader->setCoding(2, storedCoding);
	if(status != 0)
		return status;
	char storedSession;
	status = reader->setSession(0, storedSession);
	if(status != 0)
		return status;
	char storedTrext;
	status = reader->setTrext(0, storedTrext);
	if(status != 0)
		return status;
	char storedTari;
	status = reader->setTari(2, storedTari);
	if(status != 0)
		return status;
	char storedQBegin;
	status = reader->setQBegin(6, storedQBegin);
	if(status != 0)
		return status;
	char storedSel;
	status = reader->setSel(0, storedSel);
	if(status != 0)
		return status;
	char storedTarget;
	status = reader->setTarget(0, storedTarget);
	if(status != 0)
		return status;
	char reg0x17Value;
	status = reader->readAS3993Reg(0x17, &reg0x17Value);
	if(status != 0)
		return status;
	char regRefDiv = 0x07 - 3;
	reg0x17Value &= 0x8F;
	reg0x17Value |= (regRefDiv << 4);
	status = reader->writeToAS3993Reg(0x17, reg0x17Value, operationStatus);
	if(status != 0)
		return status;
	reg0x17Value = 0;
	status = reader->readAS3993Reg(0x17, &reg0x17Value);
	if(status != 0)
		return status;
	status  = reader->performAutoTuning(0x01);
	if(status != 0)
		return status;
	return 0;
}
int KitModel::setPower(char value)
{
	// value: attenuation factor for ams chip, NOT actual power in dBm
	char reg0x15Value;
	char status;
	char operationStatus;
	qDebug("Setting power to attenuation factor %d", value);
	if (value > 19)
		return -1;
	char registerEntry = value;
	if (value > 11)
		registerEntry = value + 4;
	status = reader->readAS3993Reg(0x15, &reg0x15Value);
	if(status != 0)
		return status;
	reg0x15Value &= 0xE0;
	reg0x15Value |= registerEntry;
	status = reader->writeToAS3993Reg(0x15, reg0x15Value, operationStatus);
	if (status!=0)
		return status;
	txPower = value;
	return 0;
}
int KitModel::setFrequencyBand(FreqBandEnum band)
{
	char status;
	char operationStatus;
	char maxTuningTableSizeSupported;
	if (band==FCC)
	{
		qDebug("Changing band to FCC");
		emit antennaTuningSignal(0,50);
		status = reader->deleteCurrentTuningTable(maxTuningTableSizeSupported);
		if (status != 0)
			return status;
		status = reader->addHoppingFreq(FCCBandFreqs[0], 0x01, 1, operationStatus);
		if(status != 0)
			return status;
		status = autotune(FCCBandFreqs[0]);
		if (status != 0)
			return status;
		for (int i=1;i<=49;i++)
		{
			status = reader->addHoppingFreq(FCCBandFreqs[i], 0x00, 1, operationStatus);
			if(status != 0)
			{
				qDebug("Failed to add channel to hop table");
				return status;
			}
			status=autotune(FCCBandFreqs[i]);
			if (status != 0)
				return status;
			emit antennaTuningSignal(i,50);
		}
		centerFrequency=915000;
		emit antennaTuningSignal(50,50);	
	}
	else if (band==ETSI)
	{
		qDebug("Changing band to ETSI");
		status = reader->deleteCurrentTuningTable(maxTuningTableSizeSupported);
		if (status != 0)
			return status;
		status = reader->addHoppingFreq(ETSIBandFreqs[0], 0x01, 1, operationStatus);
		if(status != 0)
			return status;
		status = autotune(ETSIBandFreqs[0]);
		if (status != 0)
			return status;
		for (int i=1;i<=3;i++)
		{
			status = reader->addHoppingFreq(ETSIBandFreqs[i], 0x00, 1, operationStatus);
			if(status != 0)
				return status;
			status = autotune(ETSIBandFreqs[i]);
			if (status != 0)
			{
				qDebug("autotune failed");
				return status;
			}
		}
		qDebug("ETSI Tuning complete");
		centerFrequency=866600;
	}
	else if (band==PRC)
	{
		emit antennaTuningSignal(0,16);
		status = reader->deleteCurrentTuningTable(maxTuningTableSizeSupported);
		if (status != 0)
			return status;
		status = reader->addHoppingFreq(PRCBandFreqs[0], 0x01, 1, operationStatus);
		if(status != 0)
			return status;
		status = autotune(PRCBandFreqs[0]);
		if (status != 0)
			return status;
		for (int i=1;i<=15;i++)
		{
			status = reader->addHoppingFreq(PRCBandFreqs[i], 0x00, 1, operationStatus);
			if(status != 0)
			{
				qDebug("Failed to add channel to hop table");
				return status;
			}
			status=autotune(PRCBandFreqs[i]);
			if (status != 0)
				return status;
			emit antennaTuningSignal(i,16);
		}
		centerFrequency=922375;
		emit antennaTuningSignal(16,16);	
	}
	else if (band==JAPAN)
	{
		emit antennaTuningSignal(0,6);
		status = reader->deleteCurrentTuningTable(maxTuningTableSizeSupported);
		if (status != 0)
			return status;
		status = reader->addHoppingFreq(JPNBandFreqs[0], 0x01, 1, operationStatus);
		if(status != 0)
			return status;
		status = autotune(JPNBandFreqs[0]);
		if (status != 0)
			return status;
		for (int i=1;i<=5;i++)
		{
			status = reader->addHoppingFreq(JPNBandFreqs[i], 0x00, 1, operationStatus);
			if(status != 0)
			{
				qDebug("Failed to add channel to hop table");
				return status;
			}
			status=autotune(JPNBandFreqs[i]);
			if (status != 0)
				return status;
			emit antennaTuningSignal(i,6);
		}
		centerFrequency=919200;
		emit antennaTuningSignal(6,6);	
	}
	else if (band==FCC_center)
	{
		qDebug("Changing band to 915 MHz only");
		status = reader->deleteCurrentTuningTable(maxTuningTableSizeSupported);
		if (status != 0)
			return status;
		status = reader->addHoppingFreq(915250, 0x01, 1, operationStatus);
		if(status != 0)
			return status;
		status = autotune(915250);
		if (status != 0)
			return status;
		centerFrequency=915250;
	}
	else if (band==ETSI_center)
	{
		status = reader->deleteCurrentTuningTable(maxTuningTableSizeSupported);
		if (status != 0)
			return status;
		status = reader->addHoppingFreq(866900, 0x01, 1, operationStatus);
		if(status != 0)
			return status;
		status = autotune(866900);
		if (status != 0)
			return status;
		centerFrequency=866900;
	}
	status = reader->setFreqHoppingParams(1, 400, 0, -40, operationStatus);
	if(status != 0)
	{
		qDebug("Error setting hopping parameters");
		return status;
	}
	currentFreqBand=band;
	emit bandChangedSignal(band);
	qDebug("Band changed");
	return 0;
}
void KitModel::debugPrintTagLists()
{
	int validCount;
	qDebug("TEMPERATURE TAGS");
	for (int i=0;i<TempTagList.length();i++)
	{
		qDebug("TID: %s", qPrintable(TempTagList[i].getTid()));
		qDebug("EPC: %s", qPrintable(TempTagList[i].getEpc()));
		qDebug("Label: %s", qPrintable(TempTagList[i].Label));
		qDebug("C1: %d", TempTagList[i].getTempCalC1());
		qDebug("C1: %d", TempTagList[i].getTempCalC2());
		qDebug("C1: %f", TempTagList[i].getTempCalT1());
		qDebug("C1: %f", TempTagList[i].getTempCalT2());
		for (int j=0; j<TempTagList[i].SensorReadHistory.length(); j++)
		{
			qDebug("    Freq: %d", TempTagList[i].SensorReadHistory[j].getFrequencyKHz());
			qDebug("    Pwr: %d", TempTagList[i].SensorReadHistory[j].getReadPower());
			qDebug("    Sensor: %d", TempTagList[i].SensorReadHistory[j].getSensorCode());
			qDebug("    OnChipRssi: %d", TempTagList[i].SensorReadHistory[j].getOnChipRssiCode());
			qDebug("    Temp: %d", TempTagList[i].SensorReadHistory[j].getTemperatureCode());
			qDebug(" ");
		}
		qDebug("Temp: %f", TempTagList[i].calculateTemperature(0,31,1, validCount));
		qDebug(" ");
	}
	qDebug("MOISTURE TAGS");
	for (int i=0;i<MoistTagList.length();i++)
	{
		qDebug("TID: %s", qPrintable(MoistTagList[i].getTid()));
		qDebug("EPC: %s", qPrintable(MoistTagList[i].getEpc()));
		qDebug("Label: %s", qPrintable(MoistTagList[i].Label));
		qDebug(" ");
	}
}
void KitModel::addTagToList(TagData tag, QString measurementType)
{
	int maxNumberOfTags=5;
	QString tid = QString::fromStdString(tag.getTID());
	if (tid.length() == 26)
		tid = tid.right(24);
	else
		return;
	if (tid.left(5) != "e2824")
		return;
	if (measurementType=="Temperature" && tid.mid(5,2)!="03")
		return;
	QString epc = QString::fromStdString(tag.getEPC());
	if (epc.length() > 2)
		epc = epc.right(epc.length()-2);
	else
		epc = "";
	QString tempCal = QString::fromStdString(tag.getTempCalibrationParams());
	if (tempCal.length() == 18)
		tempCal = tempCal.right(16);
	else
		tempCal = "";
	if (measurementType=="Temperature")
	{
		if (TempTagList.length()>=maxNumberOfTags)
			return;
		bool tagAlreadyInList=false;
		for (int t=0; t<TempTagList.length(); t++)
		{		
			if (TempTagList[t].getEpc()==epc)
			{
				tagAlreadyInList=true;
				if (tempCal.length() == 16)
				{
					for (int i=0;i<=3;i++)
						TempTagList[t].setUserMemory(8+i, UtilityFunctions::HexWordToInt(tempCal.mid(4*i,4)));
					TempTagList[t].decodeTemperatureCalWords();
				}	
				break;
			}			
		}
		if (tagAlreadyInList == false)
		{
			SensorTag newTag;
			newTag.setEpc(epc);
			newTag.setTid(tid);
			QString abbreviatedEpc=UtilityFunctions::AbbreviatedEpc(epc);
			newTag.Label=abbreviatedEpc;
			bool labelIsUnique;
			do
			{
				labelIsUnique=true;
				for (int t=0; t<TempTagList.length(); t++)
				{
					if (newTag.Label == TempTagList[t].Label)
					{
						labelIsUnique=false;
						if (newTag.Label==abbreviatedEpc)
							newTag.Label=abbreviatedEpc+"_0";
						else
						{
							int underscoreIndex = newTag.Label.indexOf("_");
							int len = newTag.Label.length();
							int num = newTag.Label.right(len-underscoreIndex-1).toInt();
							num++;
							newTag.Label = abbreviatedEpc + "_" + QString::number(num);
						}					
					}
				}		
			} while (labelIsUnique==false);		
			if (tempCal.length()==16)
			{
				for (int i=0;i<=3;i++)
					newTag.setUserMemory(8+i, UtilityFunctions::HexWordToInt(tempCal.mid(4*i,4)));
				newTag.decodeTemperatureCalWords();
				TempTagList.append(newTag);
			}		
		}		
	}
	if (measurementType=="Moisture")
	{
		if (MoistTagList.length()>=maxNumberOfTags)
			return;
		bool tagAlreadyInList=false;
		for (int t=0; t<MoistTagList.length(); t++)
		{		
			if (MoistTagList[t].getEpc()==epc)
			{
				tagAlreadyInList=true;
				if (tempCal.length() == 16)
				{
					for (int i=0;i<=3;i++)
						MoistTagList[t].setUserMemory(8+i, UtilityFunctions::HexWordToInt(tempCal.mid(4*i,4)));
					MoistTagList[t].decodeTemperatureCalWords();
				}	
				break;
			}			
		}
		if (tagAlreadyInList == false)
		{
			SensorTag newTag;
			newTag.setEpc(epc);
			newTag.setTid(tid);
			QString abbreviatedEpc=UtilityFunctions::AbbreviatedEpc(epc);
			newTag.Label=abbreviatedEpc;
			bool labelIsUnique;
			do
			{
				labelIsUnique=true;
				for (int t=0; t<MoistTagList.length(); t++)
				{
					if (newTag.Label == MoistTagList[t].Label)
					{
						labelIsUnique=false;
						if (newTag.Label==abbreviatedEpc)
							newTag.Label=abbreviatedEpc+"_0";
						else
						{
							int underscoreIndex = newTag.Label.indexOf("_");
							int len = newTag.Label.length();
							int num = newTag.Label.right(len-underscoreIndex-1).toInt();
							num++;
							newTag.Label = abbreviatedEpc + "_" + QString::number(num);
						}					
					}
				}		
			} while (labelIsUnique==false);		
			if (tempCal.length()==16)
			{
				for (int i=0;i<=3;i++)
					newTag.setUserMemory(8+i, UtilityFunctions::HexWordToInt(tempCal.mid(4*i,4)));
				newTag.decodeTemperatureCalWords();
				MoistTagList.append(newTag);
			}		
		}		
	}
}
int KitModel::findTags(int numInventories, QString measurementType)
{
	char status;
	vector<TagData> tags;
	char numberOfTagsFound;	
	char inventoryResult;
	char inventoryType;
	status = reader->clearListOfSelectCommands();
	if(status != 0)
		return status;
	for (int i=1; i<=numInventories; i++)
	{
		status = reader->performGen2Inventory(0, 0x03, 0x06);
		if(status != 0)
			return status;
		status = reader->getTagData(tags, inventoryType, inventoryResult, numberOfTagsFound);
		if(status != 0)
			break;	
		if(inventoryResult != 0)
			break;
		for (unsigned j=0; j<tags.size(); j++)
			addTagToList(tags.at(j), measurementType);		
	}
	return 0;
}
int KitModel::setSelectsForReading()
{
	// Select commands must be set before calling
	char status;
	status = reader->clearListOfSelectCommands();
	if(status != 0)
		return status;	
	char mask = 0x1F;
	status = reader->singulateATag(1, 0x03, 0x06, 3, 0xA0, 0x08, 0, &mask, 1); // Magnus 2 On-Chip RSSI Select
	if(status != 0)
		return status;
	status = reader->singulateATag(1, 0x03, 0x06, 3, 0xD0, 0x08, 0, &mask, 1); // Magnus 3 On-Chip RSSI Select
	if(status != 0)
		return status;
	mask = 0;
	status = reader->singulateATag(1, 0x03, 0x06, 3, 0xE0, 0, 0, &mask, 0); // Magnus 3 Temperature Select
	if(status != 0)
		return status;
	return 0;
}
void KitModel::addSensorReading(TagData tag, QString measurementType)
{
	QString epc = QString::fromStdString(tag.getEPC());
	if (epc.length() > 2)
		epc = epc.right(epc.length()-2);
	else
		epc = "";
	if (measurementType=="Temperature")
	{
		for (int i=0; i<TempTagList.length(); i++)
		{
			if (TempTagList[i].getEpc() == epc && TempTagList[i].SelectedForMeasurement)
			{
				SensorRead sr;
				sr.setFrequencyKHz(tag.getCommFrequency());
				sr.setReadPower(txPower);
				sr.setSensorCode(tag.getMMS());
				sr.setOnChipRssiCode(tag.getVFC());
				sr.setTemperatureCode(tag.getTEMP());
				TempTagList[i].addSensorRead(sr);
				break;
			}
		}
	}
	else if (measurementType=="Moisture")
	{
		for (int i=0; i<MoistTagList.length(); i++)
		{
			if (MoistTagList[i].getEpc() == epc && MoistTagList[i].SelectedForMeasurement)
			{
				SensorRead sr;
				sr.setFrequencyKHz(tag.getCommFrequency());
				sr.setReadPower(txPower);
				sr.setSensorCode(tag.getMMS());
				sr.setOnChipRssiCode(tag.getVFC());
				sr.setTemperatureCode(tag.getTEMP());
				MoistTagList[i].addSensorRead(sr);
				break;
			}
		}
	}
}
int KitModel::readTags(int numInventories, QString measurementType)
{
	char status;
	TagData tag;
	vector<TagData> tags;
	char numberOfTagsFound;	
	char inventoryResult;
	char inventoryType;
	for(int i = 1; i <= numInventories; i++) 
	{
		status = reader->performGen2Inventory(0, 0x05, 0x06);
		if(status != 0)
			return status;
		status = reader->getTagData(tags, inventoryType, inventoryResult, numberOfTagsFound);
		if(status != 0)
			break;	
		if(inventoryResult != 0)
			break;
		for (unsigned j=0; j<tags.size(); j++)
		{	
			addSensorReading(tags.at(j), measurementType);	
		}	
	}
	return 0;
}
bool KitModel::validTempTagsInList()
{
	for (int i=0; i<TempTagList.length(); i++)
	{
		if (TempTagList[i].getCrcValid())
			return true;
	}
	return false;
}
bool KitModel::findOptimumPower(int tagIndex, QString measurementType)
{
	int validCount;
	int t=tagIndex;
	qDebug("Finding optimum power for tag %d", t);
	//=============== Temperature ====================
	if (measurementType == "Temperature")
	{
		// First, make sure this is necessary. Do we have enough reads for a good measurement already?
		float temp = TempTagList[t].calculateTemperature(TempTargetOnChipRssiMin, TempTargetOnChipRssiMax, tempMinSamplesPerMeas, validCount);
		qDebug("Temp from existing data: %f", temp);
		if (temp> -100)
		{
			return true;
		}		
		int nextPowerCode;
		int count = TempTagList[t].OnChipRssiMeasurementHistory.length();
		if (count>0)
		{
			nextPowerCode = TempTagList[t].OnChipRssiMeasurementHistory[count-1].getReadPowerCode();
			if (nextPowerCode<=tempMaxPower)
				nextPowerCode = tempMaxPower+7;
			if (nextPowerCode > 19)
				nextPowerCode = 19;
		}
		else
			nextPowerCode = (19+tempMaxPower)/2;		
		int stepSize=7;
		for (int i=1; i<=6; i++)  // Maximum of 6 iterations to look for power
		{
			qDebug("Iteration %d", i);
			bool abort = tempObserver->wizard->tempDemoPage->abortRequested();
			if (abort)
				return false;
			setPower(nextPowerCode);
			qDebug("Power set to %d", txPower);
			int ocRssi;
			for (int j=1; j<=8; j++)
			{
				int readHistoryLength1 = TempTagList[t].SensorReadHistory.length();
				readTags(1, "Temperature");
				int readHistoryLength2 = TempTagList[t].SensorReadHistory.length();
				if (readHistoryLength1==readHistoryLength2)
					ocRssi=-1;
				else
					ocRssi = TempTagList[t].getLastOnChipRssiReading();
				if (ocRssi > 31 || ocRssi <= 0)
					ocRssi = -1;
				else
					break;
			}
			qDebug("  read results: power: %d rssi: %d", txPower, ocRssi);
			if (ocRssi<TempTargetOnChipRssiMin)
			{
				qDebug("power too low");
				if (txPower == tempMaxPower)
				{
					qDebug("Already at max power, nothing to be done...");
					return false;
				}
				else if (TempTargetOnChipRssiMin>0 && ocRssi==TempTargetOnChipRssiMin-1)
				{
					nextPowerCode = txPower - 1;	
					stepSize = 1;
				}
				else
				{
					nextPowerCode = txPower - stepSize;
					if (stepSize > 6)
						stepSize = stepSize-3;
					else
						stepSize = stepSize-2;
					if (nextPowerCode <= tempMaxPower)
					{
						nextPowerCode=tempMaxPower;
						stepSize = (txPower - tempMaxPower)/2;
					}
					if (stepSize < 1)
						stepSize=1;
				}
			}	
			if (ocRssi>TempTargetOnChipRssiMax)
			{
				qDebug("power too high");
				if (txPower==19)
					return false;
				if (ocRssi==TempTargetOnChipRssiMax+1)
				{
					nextPowerCode = txPower+1;
					stepSize=1;	
				}
				else
				{
					nextPowerCode = txPower + stepSize;
					if (stepSize > 6)
						stepSize = stepSize-3;
					else
						stepSize = stepSize-2;
					if (nextPowerCode >= 19)
					{
						nextPowerCode=19;
						stepSize = (19-txPower)/2;
					}
					if (stepSize < 1)
						stepSize=1;	
				}
			}
			if (ocRssi>=TempTargetOnChipRssiMin && ocRssi <=TempTargetOnChipRssiMax)
			{
				qDebug("power good");
				return true;		
			}
		} // end power adjustment iterations
	}
	//=============== Moisture ====================	
	if (measurementType == "Moisture")
	{
		// First, make sure this is necessary. Do we have enough reads for a good measurement already?
		float moist;
		if (moistLinearRegression)
			moist = MoistTagList[t].linearFitSensorCode(MoistTargetOnChipRssiMin, MoistTargetOnChipRssiMax, moistMinSamplesPerMeas, centerFrequency, validCount);
		else
			moist = MoistTagList[t].avgSensorCode(MoistTargetOnChipRssiMin, MoistTargetOnChipRssiMax, moistMinSamplesPerMeas, validCount);
		qDebug("Moist from existing data: %f", moist);
		if (moist> -100)
		{
			return true;
		}		
		int nextPowerCode;
		int count = MoistTagList[t].OnChipRssiMeasurementHistory.length();
		if (count>0)
		{
			nextPowerCode = MoistTagList[t].OnChipRssiMeasurementHistory[count-1].getReadPowerCode();
			if (nextPowerCode<=moistMaxPower)
				nextPowerCode = moistMaxPower+7;
			if (nextPowerCode > 19)
				nextPowerCode = 19;
		}
		else
			nextPowerCode = (19+moistMaxPower)/2;		
		int stepSize=7;
		for (int i=1; i<=6; i++)  // Maximum of 6 iterations to look for power
		{
			bool abort = moistureObserver->wizard->moistDemoPage->abortRequested();
			if (abort)
				return false;
			setPower(nextPowerCode);
			qDebug("Power set to %d", txPower);
			int ocRssi;
			for (int j=1; j<=8; j++)
			{
				int readHistoryLength1 = MoistTagList[t].SensorReadHistory.length();
				readTags(1, "Moisture");
				int readHistoryLength2 = MoistTagList[t].SensorReadHistory.length();
				if (readHistoryLength1==readHistoryLength2)
					ocRssi=-1;
				else
					ocRssi = MoistTagList[t].getLastOnChipRssiReading();
				if (ocRssi > 31 || ocRssi <= 0)
					ocRssi = -1;
				else
					break;
			}
			qDebug("  read results: power: %d rssi: %d", txPower, ocRssi);
			if (ocRssi<MoistTargetOnChipRssiMin)
			{
				qDebug("power too low");
				if (txPower == moistMaxPower)
				{
					qDebug("Already at max power, nothing to be done...");
					return false;
				}
				else if (MoistTargetOnChipRssiMin>0 && ocRssi==MoistTargetOnChipRssiMin-1)
				{
					nextPowerCode = txPower - 1;	
					stepSize = 1;
				}
				else
				{
					nextPowerCode = txPower - stepSize;
					if (stepSize > 6)
						stepSize = stepSize-3;
					else
						stepSize = stepSize-2;
					if (nextPowerCode <= moistMaxPower)
					{
						nextPowerCode=moistMaxPower;
						stepSize = (txPower - moistMaxPower)/2;
					}
					if (stepSize < 1)
						stepSize=1;
				}
			}	
			if (ocRssi>MoistTargetOnChipRssiMax)
			{
				qDebug("power too high");
				if (txPower==19)
					return false;
				if (ocRssi==MoistTargetOnChipRssiMax+1)
				{
					nextPowerCode = txPower+1;
					stepSize=1;	
				}
				else
				{
					nextPowerCode = txPower + stepSize;
					if (stepSize > 6)
						stepSize = stepSize-3;
					else
						stepSize = stepSize-2;
					if (nextPowerCode >= 19)
					{
						nextPowerCode=19;
						stepSize = (19-txPower)/2;
					}
					if (stepSize < 1)
						stepSize=1;	
				}
			}
			if (ocRssi>=MoistTargetOnChipRssiMin && ocRssi <=MoistTargetOnChipRssiMax)
			{
				qDebug("power good");
				return true;		
			}
		} // end power adjustment iterations
	}
	return false;
}
int KitModel::searchForTempTags()
{
	qDebug("searchForTempTags()");
	bool abort=false;
	while (abort==false)
	{
		setPower(tempMaxPower);
		findTags(2, "Temperature");
		setPower((19 + tempMaxPower)/2);
		findTags(2, "Temperature");
		qDebug("No tags found");
		if (TempTagList.length() > 0)
		{
			setSelectsForReading();
			qDebug("Tags found");
			break;
		}
		abort = tempObserver->wizard->tempDemoPage->abortRequested();
	}
	return 0;
}
int KitModel::searchForMoistTags()
{
	qDebug("searchForMoistTags()");
	bool abort=false;
	while (abort==false)
	{
		setPower(moistMaxPower);
		findTags(2, "Moisture");
		setPower((19 + moistMaxPower)/2);
		findTags(2, "Moisture");
		qDebug("No tags found");
		if (MoistTagList.length() > 0)
		{
			setSelectsForReading();
			qDebug("Tags found");
			break;
		}
		abort = moistureObserver->wizard->moistDemoPage->abortRequested();
	}
	return 0;
}
int KitModel::writeDataToTag(QString epc, char bankCode, int address, QString dataHexString)
{
	// bankCode: 0=Reserved, 1=EPC, 2=TID, 3=User
	char mask[30];
	char data[30];
	short numWordsWritten;
	char errorCode;
	int status;
	setPower(tempMaxPower);
	if ((epc.length() % 4 != 0) || (dataHexString.length() % 4 != 0))
		return -2;
	char maskBitLength=epc.length()*4;
	char maskByteLength = maskBitLength/8;
	bool r=UtilityFunctions::HexStringToCharArray(epc, mask);
	if (r==false)
		return -2;
	char dataLen = dataHexString.length()/2;
	r=UtilityFunctions::HexStringToCharArray(dataHexString, data);
	if (r==false)
		return -2;
	status = reader->clearListOfSelectCommands();
	if(status != 0)
		return status;
	char storedTarget;
	status=reader->setTarget(1, storedTarget);  // Target tags in B state
	if (status!=0)
		return status;
	status=reader->singulateATag(0x1, 0x0, 0x4, 0x1, 0x20, maskBitLength, 0x0, mask, maskByteLength); 
	if (status!=0)
		return status;
	status = reader->writeToTag(bankCode, address, 0, data, dataLen, numWordsWritten, errorCode);
	qDebug("status %d   numWordsWritten: %d    ErrorCode: %d", status, numWordsWritten, errorCode);
	reader->setTarget(0, storedTarget); // Reset target to A state
	reader->clearListOfSelectCommands();
	if (numWordsWritten!=dataHexString.length()/4)
		return -3;
	return status;
}
int KitModel::measureTempTags()
{
	bool abort;
	int validTempCount;
	int validOcRssiCount;
	for (int t=0; t<TempTagList.length(); t++)
	{
		TempTagList[t].clearSensorReads();
	}
	for (int t=0; t<TempTagList.length(); t++)
	{
		if (txPower!=tempMaxPower)
			setPower(tempMaxPower);
		findTags(2, "Temperature");
		setSelectsForReading();		
		if (TempTagList[t].getCrcValid()==true && TempTagList[t].SelectedForMeasurement)
		{
			abort = tempObserver->wizard->tempDemoPage->abortRequested();
			if (abort)
				return 0;
			float temp;	
			bool goodPower=true;
			if (tempAutoPower==true)
			{
				goodPower = findOptimumPower(t, "Temperature");
			}
			for (int r=0; r<=4*tempMinSamplesPerMeas; r++)
			{
				abort = tempObserver->wizard->tempDemoPage->abortRequested();
				if (abort)
					return 0;
				if (tempAutoPower == true)
				{
					temp = TempTagList[t].calculateTemperature(TempTargetOnChipRssiMin, TempTargetOnChipRssiMax, tempMinSamplesPerMeas, validTempCount);
					qDebug("Tag %d read iteration %d temp: %f", t, r, temp);
					if (goodPower==false)
						break;
				}
				else
				{
					temp = TempTagList[t].calculateTemperature(0, 31, tempMinSamplesPerMeas, validTempCount);
					qDebug("Tag %d read iteration %d temp: %f", t, r, temp);
				}
				if (r==2*tempMinSamplesPerMeas && validTempCount <= tempMinSamplesPerMeas/3)
					break; // Give up on this round
				if (temp > -100)
				{
					break;
				}
				if (tempAutoPower == true)
				{
					int ocRssi = TempTagList[t].getLastOnChipRssiReading();
					// Check if On-Chip RSSI is outside limits and tweak power if necessary
					if (ocRssi<TempTargetOnChipRssiMin && ocRssi>0 && txPower > tempMaxPower)
						setPower(txPower-1);
					if (ocRssi>TempTargetOnChipRssiMax && ocRssi>0 && txPower < 18)
						setPower(txPower+1);
					// If on-Chip RSSI target range is wide enough, try to get away from low rssi values for better read reliability
					if (TempTargetOnChipRssiMin<12 && TempTargetOnChipRssiMax > (TempTargetOnChipRssiMin+6))
					{
						int targetMiddle=(TempTargetOnChipRssiMin+TempTargetOnChipRssiMax)/2;
						if (ocRssi>0 && ocRssi<=targetMiddle && txPower > tempMaxPower)
							setPower(txPower-1);
					}
				}
				readTags(1, "Temperature");
			}
			int totalCount=TempTagList[t].SensorReadHistory.length();
			float avgOcRssi;
			if (tempAutoPower)
			{
				temp = TempTagList[t].calculateTemperature(TempTargetOnChipRssiMin, TempTargetOnChipRssiMax, tempMinSamplesPerMeas, validTempCount);
				avgOcRssi = TempTagList[t].avgOnChipRssiCode(TempTargetOnChipRssiMin, TempTargetOnChipRssiMax, tempMinSamplesPerMeas, validOcRssiCount);
			}
			else
			{
				temp = TempTagList[t].calculateTemperature(0, 31, tempMinSamplesPerMeas, validTempCount);
				avgOcRssi = TempTagList[t].avgOnChipRssiCode(0, 31, tempMinSamplesPerMeas, validOcRssiCount);
			}
			qDebug("Tag %d final temp: %f", t, temp);
			SensorMeasurement tempMeas;
			tempMeas.setValue(temp);
			tempMeas.setTime();
			tempMeas.setNumber(TempMeasTimeList.length());
			tempMeas.setReadPowerCode(txPower);
			tempMeas.setValidPowerReadCount(validTempCount);
			tempMeas.setInvalidPowerReadCount(totalCount-validTempCount);
			SensorMeasurement ocRssiMeas;
			ocRssiMeas.setValue(avgOcRssi);
			ocRssiMeas.setTime();
			ocRssiMeas.setNumber(TempMeasTimeList.length());
			ocRssiMeas.setReadPowerCode(txPower);
			ocRssiMeas.setValidPowerReadCount(validOcRssiCount);
			ocRssiMeas.setInvalidPowerReadCount(totalCount-validOcRssiCount);
			TempTagList[t].addTemperatureMeasurement(tempMeas);
			TempTagList[t].addOnChipRssiMeasurement(ocRssiMeas);
		}		
	}
	TempMeasTimeList.append(QDateTime::currentDateTime());
	return 0;	
}
int KitModel::measureMoistTags()
{
	bool abort;
	int validMoistCount;
	int validOcRssiCount;
	for (int t=0; t<MoistTagList.length(); t++)
	{
		MoistTagList[t].clearSensorReads();
	}
	for (int t=0; t<MoistTagList.length(); t++)
	{
		if (txPower!=moistMaxPower)
			setPower(moistMaxPower);
		findTags(2, "Moisture");
		setSelectsForReading();		
		if (MoistTagList[t].SelectedForMeasurement)
		{
			abort = moistureObserver->wizard->moistDemoPage->abortRequested();
			if (abort)
				return 0;
			float moist;	
			bool goodPower=true;
			if (moistAutoPower==true)
			{
				goodPower = findOptimumPower(t, "Moisture");
			}
			for (int r=0; r<=4*moistMinSamplesPerMeas; r++)
			{
				abort = moistureObserver->wizard->moistDemoPage->abortRequested();
				if (abort)
					return 0;
				if (moistAutoPower == true)
				{						
					if (moistLinearRegression)
						moist = MoistTagList[t].linearFitSensorCode(MoistTargetOnChipRssiMin, MoistTargetOnChipRssiMax, moistMinSamplesPerMeas, centerFrequency, validMoistCount);
					else
						moist = MoistTagList[t].avgSensorCode(MoistTargetOnChipRssiMin, MoistTargetOnChipRssiMax, moistMinSamplesPerMeas, validMoistCount);			
					qDebug("Tag %d read iteration %d moist: %f", t, r, moist);
					if (goodPower==false)
						break;
				}
				else
				{
					if (moistLinearRegression)
						moist = MoistTagList[t].linearFitSensorCode(0, 31, moistMinSamplesPerMeas, centerFrequency, validMoistCount);
					else
						moist = MoistTagList[t].avgSensorCode(0, 31, moistMinSamplesPerMeas, validMoistCount);					
					qDebug("Tag %d read iteration %d moist: %f", t, r, moist);
				}
				if (r==2*moistMinSamplesPerMeas && validMoistCount <= moistMinSamplesPerMeas/3)
					break; // It's hopeless
				if (moist > -1)
				{
					break;
				}
				if (moistAutoPower == true)
				{
					int ocRssi = MoistTagList[t].getLastOnChipRssiReading();
					if (ocRssi<MoistTargetOnChipRssiMin && ocRssi>0 && txPower > moistMaxPower)
						setPower(txPower-1);
					if (ocRssi>MoistTargetOnChipRssiMax && ocRssi>0 && txPower < 18)
						setPower(txPower+1);
					// If on-Chip RSSI target range is wide enough, try to get away from low rssi values for better read reliability
					if (MoistTargetOnChipRssiMin<12 && MoistTargetOnChipRssiMax > (MoistTargetOnChipRssiMin+6))
					{
						int targetMiddle=(MoistTargetOnChipRssiMin+MoistTargetOnChipRssiMax)/2;
						if (ocRssi>0 && ocRssi<=targetMiddle && txPower > moistMaxPower)
							setPower(txPower-1);
					}
					
				}
				readTags(1, "Moisture");
			}
			int totalCount=MoistTagList[t].SensorReadHistory.length();
			float avgOcRssi;
			if (moistAutoPower)
			{
				if (moistLinearRegression)
					moist = MoistTagList[t].linearFitSensorCode(MoistTargetOnChipRssiMin, MoistTargetOnChipRssiMax, moistMinSamplesPerMeas, centerFrequency, validMoistCount);
				else
					moist = MoistTagList[t].avgSensorCode(MoistTargetOnChipRssiMin, MoistTargetOnChipRssiMax, moistMinSamplesPerMeas, validMoistCount);
				avgOcRssi = MoistTagList[t].avgOnChipRssiCode(MoistTargetOnChipRssiMin, MoistTargetOnChipRssiMax, moistMinSamplesPerMeas, validOcRssiCount);
			}
			else
			{
				if (moistLinearRegression)
					moist = MoistTagList[t].linearFitSensorCode(0, 31, moistMinSamplesPerMeas, centerFrequency, validMoistCount);
				else
					moist = MoistTagList[t].avgSensorCode(0, 31, moistMinSamplesPerMeas, validMoistCount);
				avgOcRssi = MoistTagList[t].avgOnChipRssiCode(0, 31, moistMinSamplesPerMeas, validOcRssiCount);
			}
			qDebug("Tag %d final moist: %f", t, moist);
			SensorMeasurement moistMeas;
			moistMeas.setValue(moist);
			moistMeas.setTime();
			moistMeas.setNumber(MoistMeasTimeList.length());
			moistMeas.setReadPowerCode(txPower);
			moistMeas.setValidPowerReadCount(validMoistCount);
			moistMeas.setInvalidPowerReadCount(totalCount-validMoistCount);
			SensorMeasurement ocRssiMeas;
			ocRssiMeas.setValue(avgOcRssi);
			ocRssiMeas.setTime();
			ocRssiMeas.setNumber(MoistMeasTimeList.length());
			ocRssiMeas.setReadPowerCode(txPower);
			ocRssiMeas.setValidPowerReadCount(validOcRssiCount);
			ocRssiMeas.setInvalidPowerReadCount(totalCount-validOcRssiCount);
			MoistTagList[t].addSensorMeasurement(moistMeas);
			MoistTagList[t].addOnChipRssiMeasurement(ocRssiMeas);
		}		
	}
	MoistMeasTimeList.append(QDateTime::currentDateTime());
	return 0;	
}
double KitModel::measureTempCodeForCalibration()
{
	bool abort;
	float tempCode;
	int validTempCount;
	int desiredSampleCount=20;
	qDebug("KitModel::measureTempCodeForCalibration");
	for (int t=0; t<TempTagList.length(); t++)
	{
		TempTagList[t].clearSensorReads();
	}
	setSelectsForReading();		
	for (int t=0; t<TempTagList.length(); t++)
	{	
		qDebug("Tag %d", t);
		if (TempTagList[t].SelectedForMeasurement)
		{
			qDebug("Tag %d selected for measurement", t);
			bool goodPower=true;
			if (tempAutoPower==true)
			{
				goodPower = findOptimumPower(t, "Temperature");
			}
			for (int r=0; r<=6*desiredSampleCount; r++)
			{
				qDebug("Attempt %d", r);
				abort = tempObserver->wizard->tempDemoPage->calibrationDialog->calibrationPage->onePointTab->abortRequested();
				if (abort)
				{
					qDebug("Abort requested");
					return -1000;
				}
				readTags(1, "Temperature");
				if (tempAutoPower == true)
				{
					tempCode = TempTagList[t].calculateTempCode(TempTargetOnChipRssiMin, TempTargetOnChipRssiMax, desiredSampleCount, validTempCount);
					qDebug("Tag %d read iteration %d tempCode: %f", t, r, tempCode);
					if (goodPower==false)
						break;
				}
				else
				{
					tempCode = TempTagList[t].calculateTempCode(0, 31, desiredSampleCount, validTempCount);
					qDebug("Tag %d read iteration %d tempCode: %f", t, r, tempCode);
				}
				if (tempCode > 0)
				{
					return tempCode;
				}
				if (tempAutoPower == true)
				{
					int ocRssi = TempTagList[t].getLastOnChipRssiReading();
					if (ocRssi<TempTargetOnChipRssiMin && ocRssi>0 && txPower > tempMaxPower)
						setPower(txPower-1);
					if (ocRssi>TempTargetOnChipRssiMax && ocRssi>0 && txPower < 18)
						setPower(txPower+1);
				}			
			}
		}		
	}
	return -1000;	
}
int KitModel::selectedTempTagIndex()
{
	for (int t=0;t<TempTagList.length();t++)
	{
		if (TempTagList[t].SelectedForMeasurement)
			return t;
	}
	return -1;
}
int KitModel::numberOfSelectedTempTags()
{
	int numSelectedTags=0;
	for (int t=0;t<TempTagList.length();t++)
	{
		if (TempTagList[t].SelectedForMeasurement)
			numSelectedTags++;
	}
	return numSelectedTags;
}
bool KitModel::tagInTempTagList(QString epc)
{
	for (int t=0;t<TempTagList.length();t++)
	{
		if (epc==TempTagList[t].getEpc())
			return true;
	}
	return false;
}
bool KitModel::tagInMoistTagList(QString epc)
{
	for (int t=0;t<MoistTagList.length();t++)
	{
		if (epc==MoistTagList[t].getEpc())
			return true;
	}
	return false;
}
int KitModel::exportMoistLog(QFile *file)
{
	if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
		return -1;
	QTextStream out(file);
	if (MoistTagList.length()==0)
	{
		out<< "-- No saved results --\n";
	}
	else
	{
		for (int t=0; t<MoistTagList.length(); t++)
		{		
			out << "TAG EPC: " << MoistTagList[t].getEpc() << "\n";
			out << "TID: " << MoistTagList[t].getTid() << "\n";
			out << "\n";
			out << "Time        Sensor Code   Wet/Dry   On-Chip RSSI\n";
			out << "------------------------------------------------\n";
			for (int m=0;m<MoistTagList[t].SensorMeasurementHistory.length();m++)
			{
				QString time = MoistTagList[t].SensorMeasurementHistory[m].getTimeString().leftJustified(11);			
				out << time << " ";
				float sens = MoistTagList[t].SensorMeasurementHistory[m].getValue();
				sens = UtilityFunctions::Round(sens,1);
				QString sensStr;
				if (sens<0)
					sensStr=QString("---").leftJustified(13);
				else
					sensStr = QString::number(sens).leftJustified(13);
				out << sensStr << " ";
				if (sens<0)
					out << "---       ";
				else
				{
					if ((sens >= moistThreshold && wetAbove) || (sens <= moistThreshold && !wetAbove))
						out << "wet       ";
					else
						out << "dry       ";
				}
				float rssi = MoistTagList[t].OnChipRssiMeasurementHistory[m].getValue();
				rssi = UtilityFunctions::Round(rssi,1);
				if (rssi >=0)
					out << rssi << "\n";
				else
					out << "---" << "\n";
			}
			if (t!=MoistTagList.length()-1)
				out << "================================================\n\n";		
		}
	}
	file->close();
	return 0;
}
int KitModel::exportTempLog(QFile *file)
{
	if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
		return -1;
	QTextStream out(file);
	if (TempTagList.length()==0)
	{
		out<< "-- No saved results --\n";
	}
	else
	{
		for (int t=0; t<TempTagList.length(); t++)
		{		
			out << "TAG EPC: " << TempTagList[t].getEpc() << "\n";
			out << "TID: " << TempTagList[t].getTid() << "\n";
			out << "Stored Temperature Calibration Data:\n";
			if (TempTagList[t].getCrcValid()==false)
				out << "-- No valid calibration data on tag --\n";
			else
			{
				out << "   Point 1: temperature code: " << TempTagList[t].getTempCalC1() << "  temperature: " << TempTagList[t].getTempCalT1() << " degC\n";
				out << "   Point 2: temperature code: " << TempTagList[t].getTempCalC2() << "  temperature: " << TempTagList[t].getTempCalT2() << " degC\n";
			}
			out << "\n";
			out << "Time        Temp (deg C)  On-Chip RSSI\n";
			out << "--------------------------------------\n";
			for (int m=0;m<TempTagList[t].TemperatureMeasurementHistory.length();m++)
			{
				QString time = TempTagList[t].TemperatureMeasurementHistory[m].getTimeString().leftJustified(11);			
				out << time << " ";
				float temp = TempTagList[t].TemperatureMeasurementHistory[m].getValue();
				temp = UtilityFunctions::Round(temp,1);
				QString tempStr;
				if (temp>-300)					
					tempStr = QString::number(temp).leftJustified(13);
				else
					tempStr = QString("---").leftJustified(13);
				out << tempStr << " ";
				float rssi = TempTagList[t].OnChipRssiMeasurementHistory[m].getValue();
				rssi = UtilityFunctions::Round(rssi,1);
				if (rssi>0)
					out << rssi << "\n";
				else
					out << "---" << "\n";
			}
			if (t!=TempTagList.length()-1)
				out << "=================================\n\n";		
		}
	}
	file->close();
	return 0;
}
void KitModel::continuousWave(char timeInSeconds)
{
	char mask;
	setPower(tempMaxPower);
	reader->clearListOfSelectCommands();
	reader->singulateATag(1, 0x08, timeInSeconds, 0, 0x0, 0x0, 0, &mask, 0);
	reader->performGen2Inventory(0, 0x05, 0x06);
}
int KitModel::autotune(int c)
{
	char status;
	char IChannel, QChannel;
	char c_in, c_len, c_out;
	char remainingSizeInTuningTable;
	status = reader->getReflectedPowerLevel(c, 0, IChannel, QChannel);
	if(status != 0)
		qDebug("obtaining reflected power failed\n");
	else 
	{
		qDebug("Reflected power before tuning: %d %d", IChannel, QChannel);
		status  = reader->performAutoTuning(0x01);
		if(status != 0)
			qDebug("auto tuning failed. Error code: %d\n", status);
		else
		{
			status = reader->getReflectedPowerLevel(c, 0, IChannel, QChannel);
			if(status != 0)
				qDebug("obtaining reflected power failed: error code %d\n", status);
			else 
			{
				qDebug("Reflected power after tuning: %d %d", IChannel, QChannel);
				if (IChannel>1 || QChannel>1)
				{
					status=reader->performAutoTuning(0x02);
					if (status!=0)
						qDebug("Multi-hill tuning failed. Error code: %d", status);
					reader->getReflectedPowerLevel(c, 0, IChannel, QChannel);
					qDebug("Reflected power after multi-hill climb tuning: %d %d", IChannel, QChannel);
				}
				status = reader->getAntennaTunerParams(c_in, c_len, c_out);
				if(status != 0)
					qDebug("obtaining tuner params failed. Error code: %d\n", status);
				else 
				{
					qDebug("Cin: %d  Clen: %d  Cout: %d", c_in, c_len, c_out);
					status = reader->addToTuningTable(c, 0, 0, 0, 0, 0, 0x01, c_in, c_len, c_out, (IChannel * IChannel) + (QChannel * QChannel), remainingSizeInTuningTable);
					if(status != 0)
						qDebug("add to tuning table failed\n");
					else
						qDebug("remaining size in tuning table: %i\n", (int)remainingSizeInTuningTable);		
				}
			}
		}
	}
	qDebug("Autotuning complete for freq %d", c);
	return 0;
}
void KitModel::performInventoriesCheck()
{
	//First Configure the reader
	char status;
	qDebug("Configuring reader...\n");
	status = reader->clearListOfSelectCommands();
	if(status != 0)
		qDebug("clear select list failed\n");
	else
		qDebug("clear select list ok\n");
	char storedAntennaID;
	status = reader->setAntennaID(2, storedAntennaID);
	if(status != 0)
		qDebug("anntenna ID set failed\n");
	else
		qDebug("anntenna ID set ok\n");
	signed char storedAntennaSensitivity;
	status = reader->setAntennaSensitivity((signed char)(-90), storedAntennaSensitivity);
	if(status != 0)
		qDebug("anntenna sensitivity set failed\n");
	else
		qDebug("anntenna sensitivity set ok\n");
	char reg0x00Value;
	status = reader->readAS3993Reg(0x00, &reg0x00Value);
	if(status != 0)
		qDebug("read reg0x00 failed\n");
	else
		qDebug("read reg0x00 ok\n");
	reg0x00Value &= ~(0x04);
	reg0x00Value |= 0x04;
	char operationStatus;
	status = reader->writeToAS3993Reg(0x00, reg0x00Value, operationStatus);
	if(status != 0)
		qDebug("write reg0x00 failed\n");
	else
		qDebug("write reg0x00 ok\n");
	reg0x00Value = 0;
	status  = reader->readAS3993Reg(0x00, &reg0x00Value);
	if(status != 0)
		qDebug("read reg0x00 failed\n");
	else
		qDebug("read reg0x00 ok\n");
	char reg0x15Value;
	status = reader->readAS3993Reg(0x15, &reg0x15Value);
	if(status != 0)
		qDebug("read reg0x15 failed\n");
	else
		qDebug("read reg0x15 ok\n");
	reg0x15Value &= 0xE0;
	reg0x15Value |= 0x10;
	status = reader->writeToAS3993Reg(0x15, reg0x15Value, operationStatus);
	if(status != 0)
		qDebug("write reg0x15 failed\n");
	else
		qDebug("write reg0x15 ok\n");
	reg0x15Value = 0;
	status = reader->readAS3993Reg(0x15, &reg0x15Value);
	if(status != 0)
		qDebug("read reg0x15 failed\n");
	else
		qDebug("read reg0x15 ok\n");
	char storedLinkFreq;
	status = reader->setLinkFrequency(6, storedLinkFreq);
	if(status != 0)
		qDebug("link freq set failed\n");
	else
		qDebug("link freq set ok\n");
	char storedCoding;
	status = reader->setCoding(2, storedCoding);
	if(status != 0)
		qDebug("coding freq set failed\n");
	else
		qDebug("coding freq set ok\n");
	char storedSession;
	status = reader->setSession(0, storedSession);
	if(status != 0)
		qDebug("coding freq set failed\n");
	else
		qDebug("coding freq set ok\n");
	char storedTrext;
	status = reader->setTrext(0, storedTrext);
	if(status != 0)
		qDebug("trext freq set failed\n");
	else
		qDebug("trext freq set ok\n");
	char storedTari;
	status = reader->setTari(1, storedTari);
	if(status != 0)
		qDebug("tari freq set failed\n");
	else
		qDebug("tari freq set ok\n");
	char storedQBegin;
	status = reader->setQBegin(6, storedQBegin);
	if(status != 0)
		qDebug("Qbegin set failed\n");
	else
		qDebug("Qbegin set ok\n");
	char storedSel;
	status = reader->setSel(0, storedSel);
	if(status != 0)
		qDebug("sel freq set failed\n");
	else
		qDebug("sel freq set ok\n");
	char storedTarget;
	status = reader->setTarget(0, storedTarget);
	if(status != 0)
		qDebug("target freq set failed\n");
	else
		qDebug("target freq set ok\n");
	status = reader->setFreqHoppingParams(1, 400, 0, -40, operationStatus);
	if(status != 0)
		qDebug("set hopping freq params failed\n");
	else
		qDebug("set hopping freq params ok\n");
	char IChannel, QChannel;
	char c_in, c_len, c_out;
	char remainingSizeInTuningTable;
	bool tuningTableNotCleared = true;
	char maxTuningTableSizeSupported;
	for(int c = 902750; c <= 927250; c += 500)
	{
		if(c == 902750)
		{
			status = reader->addHoppingFreq(c, 0x01, 1, operationStatus);
			if(status != 0)
				qDebug("add hopping freq set failed\n");
			else				
				qDebug("add hopping freq set ok, freq added: %i\n", c);
		}
		else
		{
			status = reader->addHoppingFreq(c, 0x00, 1, operationStatus);
			if(status != 0)
				qDebug("add hopping freq set failed\n");
			else
				qDebug("add hopping freq set ok, freq added: %i\n", c);
		}
		if(status == 0)
		{
			status = reader->getReflectedPowerLevel(c, 0, IChannel, QChannel);
			if(status != 0)
				qDebug("obtaining reflected power failed\n");
			else 
			{
				status  = reader->performAutoTuning(0x01);
				if(status != 0)
					qDebug("auto tuning failed\n");
				else
				{
					status = reader->getReflectedPowerLevel(c, 0x01, IChannel, QChannel);
					if(status != 0)
						qDebug("obtaining reflected power failed\n");
					else 
					{
						status = reader->getAntennaTunerParams(c_in, c_len, c_out);
						if(status != 0)
							qDebug("obtaining tuner params failed\n");
						else 
						{
							if(tuningTableNotCleared)
							{
								status = reader->deleteCurrentTuningTable(maxTuningTableSizeSupported);
								if(status != 0)
									qDebug("deleting tuning table failed\n");
							}
							if(status == 0)
							{
								if(tuningTableNotCleared)
								{
									qDebug("Maximum tuning table size supported: %i\n", (int)(maxTuningTableSizeSupported));
									tuningTableNotCleared = false;
								}
								status = reader->addToTuningTable(c, 0, 0, 0, 0, 0, 0x01, c_in, c_len, c_out, (IChannel * IChannel) + (QChannel * QChannel), remainingSizeInTuningTable);
								if(status != 0)
									qDebug("add to tuning table failed\n");
								else
									qDebug("remaining size in tuning table: %i\n", (int)remainingSizeInTuningTable);
							}
						}
					}
				}
			}
		}
	}
	char reg0x17Value;
	status = reader->readAS3993Reg(0x17, &reg0x17Value);
	if(status != 0)
		qDebug("read reg0x17 failed\n");
	else
		qDebug("read reg0x17 ok\n");
	char regRefDiv = 0x07 - 3;
	reg0x17Value &= 0x8F;
	reg0x17Value |= (regRefDiv << 4);
	status = reader->writeToAS3993Reg(0x17, reg0x17Value, operationStatus);
	if(status != 0)
		qDebug("write reg0x17 failed\n");
	else
		qDebug("write reg0x17 ok\n");
	reg0x17Value = 0;
	status = reader->readAS3993Reg(0x17, &reg0x17Value);
	if(status != 0)
		qDebug("read reg0x17 failed\n");
	else
		qDebug("read reg0x17 ok\n");
	qDebug("Performing CAL inventory...\n");
	vector<TagData> tags;
	char numberOfTagsFound;	
	char inventoryResult;
	char inventoryType;
	vector<TagData>::iterator iter;
	for(int i = 0; i < 0; i++) 
	{
		status = reader->performGen2Inventory(0, 0x03, 0x06);
		if(status != 0)
			qDebug("inventory cmd failed\n");
		else
		{
			status = reader->getTagData(tags, inventoryType, inventoryResult, numberOfTagsFound);
			if(status != 0)
				qDebug("obtaining tag data failed\n");
			else if(inventoryResult != 0)
				qDebug("inventory failed\n");
		}
	}
	qDebug("Setting up Select commands\n");
	char mask = 0x1F;
	status = reader->singulateATag(1, 0x03, 0x06, 3, 0xA0, 0x08, 0, &mask, 1);
	if(status != 0)
		qDebug("VFC Select set for magnus 2 failed\n");
	else
		qDebug("VFC Select set for magnus 2 ok\n");
	status = reader->singulateATag(1, 0x03, 0x06, 3, 0xD0, 0x08, 0, &mask, 1);
	if(status != 0)
		qDebug("VFC Select set for magnus 3 failed\n");
	else
		qDebug("VFC Select set for magnus 3 ok\n");
	mask = 0;
	status = reader->singulateATag(1, 0x03, 0x06, 3, 0xE0, 0, 0, &mask, 0);
	if(status != 0)
		qDebug("TEMP Select set for magnus 3 failed\n");
	else
		qDebug("TEMP Select set for magnus 3 ok\n");
	char numberOfSuccessfulInventoryCmds = 0;
	char numberOfSuccessfulInventoryReports = 0;
	char NumberOfTagsInInventory[NUMBER_OF_TEMP_INVENTORIES];
	QTime timer;
	qDebug("Performing TEMP inventories...\n");
	tags.clear();
	timer.start();
	for(int i = 0; i < NUMBER_OF_TEMP_INVENTORIES; i++) 
	{
		status = reader->performGen2Inventory(0, 0x05, 0x06);
		if(status != 0)
		{
			qDebug("inventory cmd failed\n");
		}
		else
		{
			numberOfSuccessfulInventoryCmds++;
			status = reader->getTagData(tags, inventoryType, inventoryResult, numberOfTagsFound);
			if(status != 0 )
			{
				qDebug( "obtaining tag data failed\n");
				NumberOfTagsInInventory[i] = -1;
			}
			else if(inventoryResult != 0)
			{
				qDebug( "inventory failed\n");
				NumberOfTagsInInventory[i] = -2;
			}
			else
			{
				numberOfSuccessfulInventoryReports++;
				NumberOfTagsInInventory[i] = numberOfTagsFound;
			}
		}
	}	
	int seconds = timer.elapsed() / 1000;
	short tagCount = 0;
	short totalNumberOfTagsFound = 0;
	for(int c = 0; c < NUMBER_OF_TEMP_INVENTORIES; c++)
	{
		if(NumberOfTagsInInventory[c] == char(-1))
			qDebug("Inventory %i, # of tags found = failed to obtain data\n", c);
		else if(NumberOfTagsInInventory[c] == char(-2))
			qDebug("Inventory %i, # of tags found = inventory failed\n", c);
		else
		{
			qDebug("Inventory %i, # of tags found = %i\n", c, NumberOfTagsInInventory[c]);
			totalNumberOfTagsFound += NumberOfTagsInInventory[c];
		}
	}
	qDebug("Total number of tags in inventories: %i, total numbers of tags in vector: %i\n", totalNumberOfTagsFound, tagCount);
	qDebug("Successful: inventory cmds %i, inventory reports %i, Time: %i seconds\n", numberOfSuccessfulInventoryCmds, numberOfSuccessfulInventoryReports, seconds);
}
