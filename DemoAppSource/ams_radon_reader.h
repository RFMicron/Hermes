/// ----------------------------------------------------------------------------
/// This software is in the public domain, furnished "as is", without technical
/// support, and with no warranty, express or implied, as to its usefulness for
/// any purpose.
///
/// ams_radon_reader.h
/// This class implements an Application Programming Interface for the AMS Radon
/// RFID reader.
/// 
/// Author: Frank Miranda, RFMicron
///-----------------------------------------------------------------------------

#ifndef _AMS_RADON_READER_H_
#define _AMS_RADON_READER_H_

#include "uart.h"
#include <string>
#include <vector>

class TagData;

class AMSRadonReader 
{
	private:
		UART *uart;
	protected:
		unsigned short calculateCRC(const void *buf, unsigned short len);
	public:
		AMSRadonReader(string uartFileName);
		short initialize();
		short resetPIC();
		short resetAS3993();
		short enterBootloader();
		short getFirmwareVersion(int &firmwareVersion);
		short getFirmwareInformation(string &firmwareInfo);
		short writeToAS3993Reg(char regAddr, char regValue, char &status);
		short readAS3993Reg(char regAddr, char *regValue);
		short readAllAS3993Regs(char *regValues);
		short setReaderConfiguration(char powerMode, char *readerConfig);
		short getReaderConfiguration(char *readerConfig);
		short antennaPower(char state, char &status);
		short getReflectedPowerLevel(int freq, char tunerSettings, char &IChannel, char &QChannel);
		short addHoppingFreq(int freq, char clearList, char profileID, char &status);
		short getFreqListParams(char &profileID, int &minFreq, int &maxFreq, char &currNumFreqs, char &hostNumFreqs);
		short setFreqHoppingParams(short listeningTIme, short maxSendingTime, short idleTime, signed char  rssi, char &status);
		short getFreqHoppingParams(short &listeningTime, short &maxSendingTime, short &idleTime);
		short performContinuousModulationTest(int freq, short duration, char random, char *randomData);
		short setLinkFrequency(char linkFreq, char &storedLinkFreq);
		short setCoding(char coding, char &storedCoding);
		short setSession(char session, char &storedSession);
		short setTrext(char trext,char &storedTrext);
		short setTari(char tari, char &storedTari);
		short setQBegin(char qBegin, char &storedQBegin);
		short setSel(char set, char &storedSet);
		short setTarget(char target, char &storedTarget);
		short getGen2Settings(char &linkFreq, char &coding, char &session, char &trext, char &tari, char &qBegin, char &set, char &target);
		short setAntennaSensitivity(signed char sensitivity, signed char &storedSensitivity);
		short getAntennaSensitivity(char &sensitivity);
		short setAntennaID(char antennaID, char &storedAntennaID);
		short getAntennaID(char &antennaID);
		short performGen2Inventory(char autoAck, char tidAndFast, char rssi);
		short getTagData(vector<TagData> &tags, char &inventoryType, char &inventoryResult, char &numberOfTagsFound);
		short clearListOfSelectCommands();
		short singulateATag(char clear, char target, char action, char memBank, short address, char maskLen, char truncate, char *mask, short maskSize);
		short writeToTag(char memBank, int address, int accessPW, char *data, short dataLen, short &numOfWordsWritten, char &tagErrorCode);
		short readFromTag(char memBank, int address, int password, char *data, char &dataLen);
		short lockUnlockTag(int maskAndAction, int accessPassword, char &tagCode);
		short killTag(int killPassword, char recom, char &status);
		short startStop(char update, char start, char autoAck, char tidAndFast, char rssi, char &currStartValue);
		short getCurrentTuningTableSize(char &maxTuningTableSizeSupported, char &currTuningTableSize);
		short deleteCurrentTuningTable(char &maxTuningTableSizeSupported);
		short addToTuningTable(int freq, 
				char ant1TuneEnable, 
				char ant1Cin, 
				char ant1Clen, 
				char ant1Cout, 
				short ant1I_Q, 
				char ant2TuneEnable,
				char ant2Cin,
				char ant2Clen,
				char ant2Cout,
				short ant2I_Q,
				char &remainingSizeInTuningTable);
		short performAutoTuning(char autoTune);
		short setAntennaCin(char cin, char &storedCin);
		short setAntennaClen(char clen, char &storedClen);
		short setAntennaCout(char cout, char &storedCout);
		short getAntennaTunerParams(char &cin, char &clen, char &cout);
		short sendCommand(int password, 
				short lengthTransmitData, 
				short lengthRecieveData, 
				char directCommand, 
				char noResponseTime, 
				char *transmitData,
				char &status,
				char &dataLength,
				char *receivedData);
};
class TagData 
{
	private:
		unsigned short readerAGC;
		unsigned short readerRSSI;
		unsigned int commFrequency;
		unsigned short EPCLength;
		string EPC;
		unsigned short PC;
		unsigned short TIDLength;
		string TID;
		string tempCalibrationParams;
		unsigned short MMS;
		unsigned short VFC;
		unsigned short TEMP;
	public:
		TagData();
		void clear();
		void setReaderAGC(char readerAGC);
		void setReaderRSSI(char readerRSSI);
		void setCommFrequency(char *commFrequency);
		void setEPCAndEPCLength(char *EPC, char EPCLength);
		void setPC(char *PC);
		void setTIDAndTIDLength(char *TID, char TIDLength);
		void setTempCalibrationParams(char *tempCalibrationParams);
		void setMMS(char *MMS);
		void setVFC(char *VFC);
		void setTEMP(char *TEMP);
		unsigned short getReaderAGC();
		unsigned short getReaderRSSI();
		unsigned int getCommFrequency();
		unsigned short getEPCLength();
		string getEPC();
		unsigned short getPC();
		unsigned short getTIDLength();
		string getTID();
		string getTempCalibrationParams();
		unsigned short getMMS();
		unsigned short getVFC();
		unsigned short getTEMP();
};
#endif
