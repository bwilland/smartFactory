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

void reproductionManage(char* request, char* responseMsg) ;


void actionDeleteReproduction(char* responseMsg, int reproductionID) ;
void actionGetTimeSpan();
void actionSaveReproduction(char* responseMsg, char paras[][128], int iCount);
void insertReproductionTask();
void actionReproductionInfo(char* responseMsg, int reproductionID) ;
void actionReproductionInfoList(char* responseMsg);
void actionAllReproductionList(char* responseMsg) ;
void makeReproductionInfoResponse(char* response, MODE* pReproductions, int reproductionCount) ;
