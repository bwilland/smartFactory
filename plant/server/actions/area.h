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

void areaManage(char* request,char* responseMsg);
void addAreaNodes(char* responseMsg,int areaID,char* nodeIDs,int nodeType);
void addNewArea(char* areaName,char* imagePath,char* areaID);
void actionAreaQuery(char* responseMsg, char *mac);
void actionAreaInfo(char* responseMsg, int areaID);
void actionSaveArea(char paras[][1024],char* responseMsg,int iCount);
void actionAddAreaNode(char paras[][1024], char* responseMsg, int iCount);
void actionAreaNode4Setting(char* responseMsg,int areaID,int nodeType);
void actionAreaList(char* responseMsg);
void actionAreaNode(char* responseMsg,int areaID);
void actionAreaSaveSort(char* responseMsg, char* sortedAreaIDs);
void actionDeleteArea(char* responseMsg, int areaID);
void setGroupMask(int areaID, int nodeType) ;
