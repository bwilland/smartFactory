#ifndef H_SYMBOL
  #define H_SYMBOL
	#include "symbol.h"
#endif

int sendData(BYTE* commandBuffer, int size, int cCount) ;
int serialData(BYTE* commandBuffer, int size, int cCount,BYTE* readBuffer);
int serialWriter(BYTE* commandBuffer, int size, BYTE* readBuffer,BOOL isWait);

int openSerial(const char *Dev);
void setParam(int serialPort_fd) ;

int startSerialServer(void);
int restartSerialServer();

