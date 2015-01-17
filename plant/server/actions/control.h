/*
 * home.h
 *
 *  Created on: Aug 7, 2013
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
#ifndef DBADAPTER
#define DBADAPTER
#include "../shared/dbadapter.h"
#endif

void controlManage(char* request, char* responseMsg);
void actionTask(char* responseMsg, int startTime, int endTime, char* areaIds,
		char* modelList);
void makeControlReturn(char* responseMsg, int code, AREAINFO* areaInfo);
BOOL saveTaskModel(char*responseMsg, int scheduleID, char* str_model);
void actionReproduction(char* responseMsg);
void actionStopReproduction(char* responseMsg);
void actionManufacture(char* responseMsg, char* areaIds, int modelId, int option,BOOL openLocked);


BOOL sendModel2Area(int areaId, int modelId, int option, BOOL isStatic, AREAINFO* p_area,int resendTimes, int isBroadcast);
void writeLog(AREAINFO* areaInfo);
