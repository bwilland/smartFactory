#ifndef H_SYMBOL
  #define H_SYMBOL
	#include "symbol.h"
#endif
#ifndef DATASTRUCT
  #define DATASTRUCT
	#include "datastruct.h"
#endif


int genLEDControlFrame(COMMANDINFO* commandInfo,BYTE* command);
int genQueryFrame(char* mac,BYTE* bFrame);
int sum(BYTE* bFrame,int iFrom, int iEnd);
int atohex(BYTE a);
BOOL atoMac(BYTE *aMac,BYTE *bMac);
BOOL atoAddr(BYTE *aAddr,BYTE *bAddr);
void printHEX(BYTE* comm,int len);
int genBroadCastQueryFrame(BYTE* bFrame) ;
int genMotoSetControl(COMMANDINFO* commandInfo, BYTE* bFrame) ;
int genHeader(COMMANDINFO* commandInfo, BYTE* bFrame);
int genFooter(COMMANDINFO* commandInfo, BYTE* bFrame, int i);
int genGetNodeInfo(COMMANDINFO* commandInfo, BYTE* bFrame);
int genReset(COMMANDINFO* commandInfo, BYTE* bFrame);
int genSetLEDCurrent(COMMANDINFO* commandInfo, BYTE* bFrame);
int genSetLEDGroupsMask(COMMANDINFO* commandInfo, BYTE* bFrame);
int genLEDManualControl(COMMANDINFO* commandInfo, BYTE* bFrame);
int genGetLEDInfo(COMMANDINFO* commandInfo, BYTE* bFrame) ;
int genMotorStatus(COMMANDINFO* commandInfo, BYTE* bFrame) ;
int genSetLEDCalibrate(COMMANDINFO* commandInfo, BYTE* bFrame);
int genSetMotorParameters(COMMANDINFO* commandInfo, BYTE* bFrame);
