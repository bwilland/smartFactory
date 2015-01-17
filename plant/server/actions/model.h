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

void modelManage(char* request, char* responseMsg) ;

void actionGetKRKB(char* responseMsg);

void actionDeleteModel(char* responseMsg, int modelID) ;

void actionSaveModel(char* responseMsg, char paras[][128], int iCount);

void actionModelInfo(char* responseMsg, int modelID) ;
void actionModelInfoList(char* responseMsg);
void actionAllModelList(char* responseMsg) ;
void makeModelInfoResponse(char* response, MODE* pModels, int modelCount) ;
void getModeName(char *Name,int modeID);
void strReplace(char* src, char dist, char rep);
