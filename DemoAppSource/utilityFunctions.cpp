#include "utilityFunctions.h"

namespace UtilityFunctions
{
	QString Crc16(QString data)
	{
		QByteArray byteArray = QByteArray::fromHex(data.toLatin1());
		QBitArray bitArray;
		bitArray.resize(8*byteArray.count());
		for (int i=0; i<byteArray.count(); i++)
			for (int b=0; b<8; ++b)
				bitArray.setBit(i*8+b, byteArray.at(byteArray.count()-1-i)&(1<<b));
		QBitArray crc(16, true);
		for (int j=bitArray.count()-1; j>=0; j--)
		{
			bool newBit = crc[15] ^ bitArray[j];
			for (int i=15; i>=1; i--)
			{
				if (i==12 || i==5)
				{
					crc[i] = crc[i-1] ^ newBit;
				}
				else
				{
					crc[i]=crc[i-1];
				}	
			}
			crc[0]=newBit;
		}
		crc=~crc;
		QByteArray output(2,0);
		for (int b=0; b<16; ++b)
		{
			output[b/8] = ( output.at(b/8) | ((crc[15-b]?1:0)<<((15-b)%8)));
		}
		QString qs=output.toHex().toUpper().rightJustified(4,'0');
		return qs;
	}
	double LinearFit(double X[], double Y[], int size, double arg)
	{
		if (size < 2)
		{
			return -1000;
		}
		double SumX=0;
		double SumY=0;
		double SumXX = 0;
		double SumXY = 0;
		for (int i=0; i<size; i++)
		{
			SumX=SumX+X[i];
			SumY=SumY+Y[i];
			SumXX=SumXX+X[i]*X[i];
			SumXY=SumXY+X[i]*Y[i];
		}
		double slope=(size*SumXY-SumX*SumY)/(size*SumXX-SumX*SumX);
		double intercept=(SumY-slope*SumX)/size;
		return (slope*arg+intercept);
	}
	QString IntToHexWord(int d)
	{
		if (d>=0)
		{
			return QString::number(d, 16).toUpper().rightJustified(4,'0');
		}
		return "0000";
	}
	QString LongIntToHex(long long d)
	{
		if (d>=0)
		{
			return QString::number(d, 16).toUpper().rightJustified(12,'0');
		}
		return "000000000000";	
	}
	int HexWordToInt(QString h)
	{
		bool ok;
		int d = h.toInt(&ok, 16);
		if (ok)
		{
			return d;
		}
		return -1;
	}
	bool HexStringToCharArray(QString hexString, char data[30])
	{
		if (hexString.length() % 4 != 0 || hexString.length()>60)
			return false;
		for (int i=0; i<=hexString.length()-2; i=i+2)
		{
			QString byteString = hexString.mid(i, 2);
			int integer = HexWordToInt(byteString);
			if (integer<0)
				return false;
			char byteData = (char)integer;
			data[i/2]=byteData;
		}
		return true;
	}
	QBitArray IntToQBitArray(int data, int bitLength)
	{
		QBitArray bits(bitLength);
		if (data >= pow(2, bitLength))
		{
			bits.fill(true);
			return bits;
		}
		for (int i=bitLength-1;i>=0;i--)
		{
			if (data==0)
			{
				break;
			}
			if (data % 2 == 1)
			{
				bits.setBit(i, true);
			}
			data=data / 2;
		}
		return bits;
	}
	int QBitArrayToInt(QBitArray bits)
	{
		int result=0;
		for (int i=bits.size()-1; i>=0; i--)
		{
			if (bits.testBit(i))
			{
				result+=pow(2,bits.size()-1-i);
			}
		}
		return result;
	}
	long long QBitArrayToLongInt(QBitArray bits)
	{
		long long result=0;
		for (int i=bits.size()-1; i>=0; i--)
		{
			if (bits.testBit(i))
			{
				result+=pow(2,bits.size()-1-i);
			}
		}
		return result;
	}
	int RoundToInt(double r)
	{
		return (r > 0) ? (r + 0.5) : (r - 0.5);
	}
	double Round(double x, int digitsAfterDecimal)
	{
		int factor = pow(10, digitsAfterDecimal);
		int y=RoundToInt(x*factor);
		return 1.0*y/factor;
	}
	void DebugPrintQBitArray(QBitArray bits)
	{
		QString qs = "";
		for (int i=0; i<bits.size(); i++)
		{
			if (bits.testBit(i))
				qs += "1";
			else
				qs += "0";
		}
		qDebug("%s", qPrintable(qs));
	}
	QString AbbreviatedEpc(QString epc)
	{
		if (epc.length() <= 8)
			return epc;
		else
			return epc.left(2)+".."+epc.right(4);
	}
}
