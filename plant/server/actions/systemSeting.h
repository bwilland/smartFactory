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
void systemManage(char* request, char* responseMsg);
void actionSystemUpdate(char* responseMsg, int isW3,char* url);
void actionSaveSendSetting(char* responseMsg, int resendTimes,int isBroasdcast,int isSendConfirm);
void actionGetSendSetting(char* responseMsg);
void saveServerName(char* responseMsg, char* serverName);
void actionGetServerName(char* responseMsg);
void actionGetSystemDate(char* responseMsg);
void actionSetSystemDate(char* responseMsg,int dt);
void makeDBInfoResponse(char* response, BACKDB* dbInfo, int dbCount);
