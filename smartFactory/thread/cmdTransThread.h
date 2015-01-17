/*
 * cmdTransThread.h
 *
 *  Created on: 2014-10-26
 *      Author: root
 */

#ifndef H_SYMBOL
#define H_SYMBOL
#include "../shared/symbol.h"
#endif
#ifndef DATASTRUCT
#define DATASTRUCT
#include "../shared/datastruct.h"
#endif
#include "xmlParse.h"


void startTransServer();
void* cmdTrans();
int cmdHandler(xmlDocPtr pdoc, BYTE *writeBuff) ;

void removeServerNode(char *sSrc);
void setProgramTime(PROGRAM *program, time_t time2Program);

time_t setTimeStruct(const time_t t_time, char *ptime);
void addProgram(PROGRAM *program);
void startProgram(PROGRAM *program);
void stopProgram(PROGRAM *program);
void removeProgram(PROGRAM *program);
