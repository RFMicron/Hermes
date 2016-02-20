/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// utilityFunctions.h
/// This class implements functions for performing simple data-conversion tasks.
/// 
/// Author: Greg Pitner, RFMicron
///-----------------------------------------------------------------------------

#include <QString>
#include <QByteArray>
#include <QBitArray>
#include <cmath>

namespace UtilityFunctions
{
	QString Crc16(QString data);
	double LinearFit(double X[], double Y[], int size, double arg);
	QString IntToHexWord(int d);
	QString LongIntToHex(long long d);
	int HexWordToInt(QString h);
	bool HexStringToCharArray(QString hexString, char data[30]);
	QBitArray IntToQBitArray(int data, int bitLength);
	int QBitArrayToInt(QBitArray bits);
	long long QBitArrayToLongInt(QBitArray bits);
	int RoundToInt(double x);
	double Round(double x, int digitsAfterDecimal);
	void DebugPrintQBitArray(QBitArray bits);
	QString AbbreviatedEpc(QString epc);
}
