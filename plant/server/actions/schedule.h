/*
 * scheduleact.h
 *
 *  Created on: Aug 22, 2013
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
void scheduleManage(char* request, char* responseMsg);
void actionScheduleList(char *responseMsg,char* clause,int pageNo);
void getStatusName(int statusId,char* name);
void actionSaveSchedule(char* responseMsg, char paras[][256], int iCount,char* taskList);
BOOL saveTask(int scheduleID, char* str_device);


void getScheduleInfo(char *responseMsg,int scheduleID);
void actionDeleteSchedule(char* responseMsg, int scheduleID);
void actionModifySchedule(char* responseMsg, int scheduleId,int option);
void actionScheduleTaskInfo(char *responseMsg, int scheduleID) ;
void actionScheduleDetail(char *responseMsg, int scheduleID);
void getStatusName(int statusId, char* name);
