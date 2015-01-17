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


void nodeManage(char* request,char* responseMsg);
void makePageInfoResponse(char* response,int maxPage,int currentPage) ;
void makeNodeInfoResponse(char* response,NODE* pnodes,int nodeCount);
void actionAllNodeList(char* responseMsg,int pageNo);
void actionNodeInfo(char*  responseMsg,int nodeID);
void actionNodeControl(char* responseMsg,int nodeID,int order,int data,int mode);
void makeReturnMsg(char* responseMsg,int returnCd,char* message);
int setMotorParam(NODE *node);
void actionSaveNode(char* responseMsg, char paras[][1024], int iCount);
int checkNode(char* mac);

void actionTurnOffLed(char* responseMsg, char* nodeIDs);
void turnOffLed(int nodeId);
void actionGetNodeInfo(char* responseMsg, int nodeId) ;
void setNodeCaliberate( CALIBERATE *caliberate);
void actionSetNodeCaliberate(char* responseMsg, CALIBERATE *caliberate);
void actionResetNode(char* responseMsg, int nodeId);
void actionGetNodeStatus(char* responseMsg,int nodeId);
void actionSetLEDCurrent(char* responseMsg, int nodeId, float current1, float current2, float current3, float current4);
int setLEDCurrent(NODE *node);
void actionAllLEDNodeList(char* responseMsg);
//void actionNodeCtrl(char* responseMsg,char paras[][512],int iCount);
//void actionNodeOnOffTest(char* responseMsg, char paras[][512], int iCount);
void actionDeleteNode(char* responseMsg, char* nodeIDs);
//void actionSaveArea(char* responseMsg, char paras[][50], int iCount);
void getNodeStatus(char* responseMsg, NODE* node);
