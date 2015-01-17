#ifndef H_SYMBOL
  #define H_SYMBOL
	#include "symbol.h"
#endif

int sendSocketData(BYTE* commandBuffer, int size, int cCount) ;
int socketData(BYTE* commandBuffer, int size, int cCount,BYTE* readBuffer);
int socketWriter(BYTE* commandBuffer, int size, BYTE* readBuffer,BOOL isWait);

int readSocketData(BYTE* readBuffer, int bufferSize);
int socketWriterOnly(BYTE* commandBuffer, int size);

int openSocket();
int closeSocket();


int startSocketServer(void);
int restartSocketServer();

