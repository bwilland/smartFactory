#ifndef H_SYMBOL
  #define H_SYMBOL
	#include "symbol.h"
#endif
#ifndef DATASTRUCT
  #define DATASTRUCT
	#include "datastruct.h"
#endif

int genDeviceControlFrame(SERVERCOMMAND* commandInfo, BYTE* bFrame);
int genSingleGroupControlFrame(SERVERCOMMAND* sCommand, BYTE* bFrame);
int genGroupsControlFrame(SERVERCOMMAND* sCommand, BYTE* bFrame);
int genGroupSetFrame(GroupSettingCommand* sCommand, BYTE* bFrame);
int genSensorQueryFrame(SERVERCOMMAND* commandInfo, BYTE* bFrame);
//***************************************************************


int genQueryFrame(char* mac,BYTE* bFrame);
int sum(BYTE* bFrame,int iFrom, int iEnd);
int atohex(BYTE a);
BOOL atoMac(BYTE *aMac,BYTE *bMac);
BOOL atoAddr(BYTE *aAddr,BYTE *bAddr);
void printHEX(BYTE* comm,int len);
int genBroadCastQueryFrame(BYTE* bFrame) ;

