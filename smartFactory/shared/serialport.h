#ifndef H_SYMBOL
  #define H_SYMBOL
	#include "symbol.h"
#endif


int sendSingleData(BYTE* commandBuffer, int size);
int serialSingleData(BYTE* commandBuffer, int size, BYTE* readBuffer);
int readSerialData(BYTE* readBuffer);
int serialWriter(BYTE* commandBuffer, int size, BYTE* readBuffer,BOOL isWait);


int openSerial(const char *Dev);
void setParam(int serialPort_fd) ;

int startSerialServer(void);
int restartSerialServer();

int Serial_Sem_V();
int Serial_Sem_P();
int CreateSerialSem();
