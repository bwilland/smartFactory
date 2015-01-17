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

void homeManage(char* request, char* responseMsg);
void actionHome(char* responseMsg, int PageNo);
void actionLogLoginQuery(char* responseMsg, int startTime, int endTime, char* userName,
		int pageNo) ;
void actionLogQuery(char* responseMsg, int startTime, int endTime, int areaId,
		int pageNo);
void actionLogDetail(char* responseMsg, int logId);
void actionLogin(char* responseMsg, char* userName, char* password,int loginTime,
		char* clientIp);
void actionLogOut(char* responseMsg,char* clientIp);
void actionModiPwd(char* responseMsg, char* userName, char* password,
		char* newPwd);
void actionSaveUser(char* responseMsg, char* userName, char* password, int role);
void actionAllUser(char* responseMsg);
void actionUserInfo(char* responseMsg, int userID);
void actionDeleteUser(char* responseMsg, int userID);
void logActionLog(USERLOG* userLog) ;
