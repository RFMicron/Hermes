#include "sensorTag.h"
#include "utilityFunctions.h"

SensorRead::SensorRead()
{
	FrequencyKHz = -1;
	ReadPower = -1;
	SensorCode = -1;
	OnChipRssiCode = -1;
	TemperatureCode = -1;
}
SensorRead::SensorRead(int freq, int power, int sensor, int onChipRssi, int temp)
{
	FrequencyKHz = freq;
	ReadPower = power;
	SensorCode = sensor;
	OnChipRssiCode = onChipRssi;
	TemperatureCode = temp;
}
void SensorRead::setFrequencyKHz(int freq)
{
	FrequencyKHz = freq;
}
void SensorRead::setReadPower(int power)
{
	ReadPower = power;
}
void SensorRead::setSensorCode(int sensor)
{
	SensorCode = sensor;
}
void SensorRead::setOnChipRssiCode(int rssi)
{
	OnChipRssiCode = rssi;
}
void SensorRead::setTemperatureCode(int temp)
{
	TemperatureCode = temp;
}
int SensorRead::getFrequencyKHz()
{
	return FrequencyKHz;
}
int SensorRead::getReadPower()
{
	return ReadPower;
}
int SensorRead::getSensorCode()
{
	return SensorCode;
}
int SensorRead::getOnChipRssiCode()
{
	return OnChipRssiCode;
}
int SensorRead::getTemperatureCode()
{
	return TemperatureCode;
}
//==================================
SensorMeasurement::SensorMeasurement()
{
	Value=-1000;
	ValidPowerReadCount=0;
	InvalidPowerReadCount=0;
	Number=0;
	TimeStamp = QDateTime::currentDateTime();
}
void SensorMeasurement::setValue(float v)
{
	Value=v;
}
void SensorMeasurement::setValidPowerReadCount(int c)
{
	ValidPowerReadCount=c;
}
void SensorMeasurement::setInvalidPowerReadCount(int c)
{
	InvalidPowerReadCount=c;
}
void SensorMeasurement::setReadPowerCode(int p)
{
	ReadPowerCode=p;
}
void SensorMeasurement::setTime()
{
	TimeStamp=QDateTime::currentDateTime();
}
void SensorMeasurement::setNumber(int n)
{
	Number = n;
}
float SensorMeasurement::getValue()
{
	return Value;
}
int SensorMeasurement::getValidPowerReadCount()
{
	return ValidPowerReadCount;
}
int SensorMeasurement::getInvalidPowerReadCount()
{
	return InvalidPowerReadCount;
}
int SensorMeasurement::getReadPowerCode()
{
	return ReadPowerCode;
}
int SensorMeasurement::getNumber()
{
	return Number;
}
QDateTime SensorMeasurement::getFullTimeStamp()
{
	return TimeStamp;
}
QString SensorMeasurement::getTimeString()
{
	return TimeStamp.toString("hh.mm.ss");
}
//==================================
SensorTag::SensorTag()
{
	Epc="";
	Label="";
	Tid="";
	SelectedForMeasurement = true;
	for (int i=0;i<=11;i++)
	{
		UserMemory[i]=-1;
	}
	TempCalC1=-1;
	TempCalT1=-1000.0;
	TempCalC2=-1;
	TempCalT2=-1000.0;
	CrcValid=false;
	ReadHistoryMaxLength=1000;
	MeasurementHistoryMaxLength=500;
}
void SensorTag::setEpc(QString epc)
{
	Epc = epc;
}
void SensorTag::setUserMemory(int addr, int data)
{
	if (addr>=0 && addr<=11)
	{
		UserMemory[addr]=data;
	}
}
void SensorTag::setTid(QString tid)
{
	Tid = tid;
}
void SensorTag::setTempCalC1(int c1)
{
	TempCalC1=c1;
}
void SensorTag::setTempCalT1(float t1)
{
	TempCalT1=t1;
}
void SensorTag::setTempCalC2(int c2)
{
	TempCalC2=c2;
}
void SensorTag::setTempCalT2(float t2)
{
	TempCalT2=t2;
}
void SensorTag::setReadHistoryMaxLength(int l)
{
	ReadHistoryMaxLength=l;
}
void SensorTag::addSensorRead(SensorRead r)
{
	if (SensorReadHistory.count() >= ReadHistoryMaxLength)
	{
		SensorReadHistory.removeFirst();
	}
	SensorReadHistory.append(r);
	qDebug("  %s %d p: %d s: %d r: %d t: %d", qPrintable(Label), r.getFrequencyKHz(), r.getReadPower(), r.getSensorCode(), r.getOnChipRssiCode(), r.getTemperatureCode());
}
void SensorTag::clearSensorReads()
{
	SensorReadHistory.clear();
}
void SensorTag::addSensorMeasurement(SensorMeasurement m)
{
	if (SensorMeasurementHistory.count() >= MeasurementHistoryMaxLength)
	{
		SensorMeasurementHistory.removeFirst();
	}
	SensorMeasurementHistory.append(m);
}
void SensorTag::addTemperatureMeasurement(SensorMeasurement m)
{
	if (TemperatureMeasurementHistory.count() >= MeasurementHistoryMaxLength)
	{
		TemperatureMeasurementHistory.removeFirst();
	}
	TemperatureMeasurementHistory.append(m);
}
void SensorTag::addOnChipRssiMeasurement(SensorMeasurement m)
{
	if (OnChipRssiMeasurementHistory.count() >= MeasurementHistoryMaxLength)
	{
		OnChipRssiMeasurementHistory.removeFirst();
	}
	OnChipRssiMeasurementHistory.append(m);
}
void SensorTag::clearMeasurementHistory()
{
	SensorMeasurementHistory.clear();
	TemperatureMeasurementHistory.clear();
	OnChipRssiMeasurementHistory.clear();
}
QString SensorTag::getEpc()
{
	return Epc;
}
int SensorTag::getUserMemory(int addr)
{
	if (addr>=0 && addr<=11)
	{
		return UserMemory[addr];
	}
	return -1;
}
QString SensorTag::getTid()
{
	return Tid;
}
int SensorTag::getTempCalC1()
{
	return TempCalC1;
}
float SensorTag::getTempCalT1()
{
	return TempCalT1;
}
int SensorTag::getTempCalC2()
{
	return TempCalC2;
}
float SensorTag::getTempCalT2()
{
	return TempCalT2;
}
bool SensorTag::getCrcValid()
{
	if (CrcValid)
		return true;
	else
		return false;
}
QString SensorTag::getModel()
{
	if (Tid.length()<8)
		return "---";
	else
		return Tid.mid(5,3);
}
int SensorTag::getLastOnChipRssiReading()
{
	int count = SensorReadHistory.length();
	if (count < 1)
		return -1;
	return SensorReadHistory[count-1].getOnChipRssiCode();
}
float SensorTag::linearFitSensorCode(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int freqKHz, int &validCount)
{
	QList<int> codes;
	QList<int> freqs;
	int minFreq=1000000;
	int maxFreq=0;
	for (int i=0; i<SensorReadHistory.length(); i++)
	{
		int sensor = SensorReadHistory[i].getSensorCode();
		int ocRssi = SensorReadHistory[i].getOnChipRssiCode();
		int freq= SensorReadHistory[i].getFrequencyKHz();
		if (sensor >= 0 && sensor <= 512 && ocRssi > 0 && ocRssi >= minOnChipRssi && ocRssi <= maxOnChipRssi)
		{
			codes.append(sensor);
			freqs.append(freq);
			if (freq<minFreq)
				minFreq=freq;
			if (freq>maxFreq)
				maxFreq=freq;
		}
	}
	int size = codes.length();
	validCount=size;
	if (size<minNumberValidDataPoints || size==0)
		return -1000;
	if (size==1)
	{
		float avg=avgSensorCode(minOnChipRssi, maxOnChipRssi, minNumberValidDataPoints, validCount);
		return avg;
	}
	if (minFreq>902000 && (maxFreq-minFreq<1000))
		return -1000;
	double SumX=0;
	double SumY=0;
	double SumXX = 0;
	double SumXY = 0;
	for (int i=0; i<size; i++)
	{
		double f=freqs[i];
		double c=codes[i];
		SumX=SumX+f;
		SumY=SumY+c;
		SumXX=SumXX+f*f;
		SumXY=SumXY+f*c;
	}
	double slope=(size*SumXY-SumX*SumY)/(size*SumXX-SumX*SumX);
	double intercept=(SumY-slope*SumX)/size;
	double value=slope*freqKHz+intercept;
	if (value < 0)
		value=0;
	return value;
}
float SensorTag::avgSensorCode(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int &validCount)
{
	float sum=0;
	int count=0;
	for (int i=0; i<SensorReadHistory.length(); i++)
	{
		int sensor = SensorReadHistory[i].getSensorCode();
		int ocRssi = SensorReadHistory[i].getOnChipRssiCode();
		if (sensor >= 0 && sensor <= 512 && ocRssi > 0 && ocRssi >= minOnChipRssi && ocRssi <= maxOnChipRssi)
		{
			sum=sum+sensor;
			count++;
		}
	}
	validCount = count;
	if (count<1 || count < minNumberValidDataPoints)
	{
		return -1000.0;
	}
	return sum/count;
}
float SensorTag::avgOnChipRssiCode(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int &validCount)
{
	float sum=0;
	int count=0;
	for (int i=0; i<SensorReadHistory.length(); i++)
	{
		int ocRssi = SensorReadHistory[i].getOnChipRssiCode();
		if (ocRssi > 0 && ocRssi <= 31 && ocRssi >= minOnChipRssi && ocRssi <= maxOnChipRssi)
		{
			sum=sum+ocRssi;
			count++;
		}
	}
	validCount=count;
	if (count<1 || count < minNumberValidDataPoints)
	{
		return -1000.0;
	}
	return sum/count;
}
float SensorTag::calculateTemperature(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int &validCount)
{
	if (CrcValid==false)
		return -1000;
	float codeAvg = calculateTempCode(minOnChipRssi, maxOnChipRssi, minNumberValidDataPoints, validCount);
	if (codeAvg>0)
		return tempCodeToDegC(codeAvg);
	else
		return -1000;
}
float SensorTag::calculateTempCode(int minOnChipRssi, int maxOnChipRssi, int minNumberValidDataPoints, int &validCount)
{
	QList<float> temps;
	for (int i=0; i<SensorReadHistory.length(); i++)
	{
		int tempCode = SensorReadHistory[i].getTemperatureCode();
		int onChipRssi = SensorReadHistory[i].getOnChipRssiCode();
		if (onChipRssi < minOnChipRssi || onChipRssi > maxOnChipRssi)
			continue;
		if (tempCode < 1600 || tempCode > 2900)
			continue;
		temps.append(tempCode);
	}
	validCount = temps.length();
	if (temps.length()<1 || temps.length() < minNumberValidDataPoints)
		return -1000.0;
	if (temps.length() >= 3)
	{
		qSort(temps.begin(), temps.end());
		temps.removeFirst();
		temps.removeLast();	
	}
	float codeSum=0;
	for (int i=0; i<temps.length(); i++)
	{
		codeSum=codeSum+temps[i];
	}
	float codeAvg = codeSum/temps.length();
	return codeAvg;	
}
float SensorTag::tempCodeToDegC(float tempCode)
{
	if (TempCalC1 == TempCalC2)
	{
		return -1000.0;
	}
	double slope = (TempCalT2 - TempCalT1)/(TempCalC2 - TempCalC1);
	return slope * (tempCode - TempCalC1) + TempCalT1;
}
void SensorTag::decodeTemperatureCalWords()
{
	QString crcWord = UtilityFunctions::IntToHexWord(UserMemory[8]);
	QString calWord1 = UtilityFunctions::IntToHexWord(UserMemory[9]);
	QString calWord2 = UtilityFunctions::IntToHexWord(UserMemory[10]);
	QString calWord3 = UtilityFunctions::IntToHexWord(UserMemory[11]);
	QString calWords = calWord1 + calWord2 + calWord3;
	if (crcWord == UtilityFunctions::Crc16(calWords))
		CrcValid=true;
	else
		CrcValid=false;
	QByteArray bytes = QByteArray::fromHex(calWords.toLatin1());
	QBitArray bits(48);
	for (int i=0; i<6; ++i)
		for (int b=0; b<8; ++b)
			bits.setBit(i*8+b, bytes.at(i)&(1<<(7-b)));
	QBitArray code1bits(12);
	QBitArray temp1bits(11);
	QBitArray code2bits(12);
	QBitArray temp2bits(11);
	for (int i=0; i<=11; i++)
	{
		if (bits.testBit(i))
			code1bits.setBit(i, true);
	}
	for (int i=12; i<=22; i++)
	{
		if (bits.testBit(i))
			temp1bits.setBit(i-12, true);
	}
	for (int i=23; i<=34; i++)
	{
		if (bits.testBit(i))
			code2bits.setBit(i-23, true);
	}
	for (int i=35; i<=45; i++)
	{
		if (bits.testBit(i))
			temp2bits.setBit(i-35, true);
	}
	TempCalC1 = UtilityFunctions::QBitArrayToInt(code1bits);
	TempCalC2 = UtilityFunctions::QBitArrayToInt(code2bits);
	int temp1 = UtilityFunctions::QBitArrayToInt(temp1bits);
	int temp2 = UtilityFunctions::QBitArrayToInt(temp2bits);
	TempCalT1 = 0.1*(temp1-800);
	TempCalT2 = 0.1*(temp2-800);
}
QString SensorTag::calculateTempCal2Point(float c1, float t1, float c2, float t2)
{
	t1=10*t1+800;
	t2=10*t2+800;
	int t1int=UtilityFunctions::RoundToInt(t1);
	int t2int=UtilityFunctions::RoundToInt(t2);
	int c1int=UtilityFunctions::RoundToInt(c1);
	int c2int=UtilityFunctions::RoundToInt(c2);
	QBitArray calBits(48);
	QBitArray qba;
	qba = UtilityFunctions::IntToQBitArray(c1int,12);
	for (int i=0; i<=11; i++)
		calBits.setBit(i, qba.at(i));
	qba = UtilityFunctions::IntToQBitArray(t1int,11);
	for (int i=12; i<=22; i++)
		calBits.setBit(i, qba.at(i-12));
	qba = UtilityFunctions::IntToQBitArray(c2int,12);
	for (int i=23; i<=34; i++)
		calBits.setBit(i, qba.at(i-23));
	qba = UtilityFunctions::IntToQBitArray(t2int,11);
	for (int i=35; i<=45; i++)
		calBits.setBit(i, qba.at(i-35));
	long long cal = UtilityFunctions::QBitArrayToLongInt(calBits);
	QString calString = UtilityFunctions::LongIntToHex(cal);
	QString crc = UtilityFunctions::Crc16(calString);
	return crc + calString;
}
QString SensorTag::calculateTempCal1Point(float c1, float t1)
{
	float c2;
	float t2;
	if (t1 <= 40)
		t2 = 80;
	else
		t2=0;
	float correctionFactor=1.01;
	float slope = correctionFactor*(t1+273.15)/c1;
	c2 = (t2 - t1)/slope + c1;
	return calculateTempCal2Point(c1, t1, c2, t2);
}
