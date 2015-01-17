#ifndef DATASTRUCT
  #define DATASTRUCT
	#include "datastruct.h"
#endif
#ifndef H_SYMBOL
  #define H_SYMBOL
	#include "shared/symbol.h"
#endif
void openDB();
void closeDB();

int getResultCount(char* table,char* clause);
int count_callback(void *arg, int nr, char **values, char **names) ;

int getLastNodeId();
int insertNode(NODE* pNodes);
int deleteAllNode();
int deleteNode(char* deviceID) ;
int getAllNode(NODE* pNodes);
int getAllNodeByType(NODE* pNodes,int type);
int getAllNodeWithPage(NODE* pNodes,int startNo);
int getDBNodeInfo(NODE* pNodes, int nodeID);
int updateNode(NODE* pNodes);
int invalidAllNodes();
int validNode(char* mac);
int updateNodeCurrent(int nodeID,float current1, float current2, float current3,
		float current4);

int getAllArea(AREAINFO* pArea);
int getAreaIDbyMac(char* mac) ;
int getAreaInfo(AREA* pArea,int areaid);
int getAllAreaIDs(char* areaIds);
int getAreaList(AREA* pArea);
int updateArea(int areaID, char* areaName, char* memo,int kr,int kb);
int updateAreaOrder(int areaID, int orderId);
int insertNewArea(char* areaName, char* memo,int kr,int kb);
int deleteArea(int areaID);
int deleteAreaNodeWithType(int areaID,int nodeType);
int deleteAreaNode(int areaID);
int updateAreaNodeCount();

int getNodeByAreaIdType(NODE* pNode, int areaID,int type);

int updateAreaNode(int areaID, char* deviceID);
int getAreaNames(char* areaName,char *areaIds);
int getNodeByAreaId(NODE* pNode, int areaID);
int getNodesMacs(char mac[][9], int offset);
int getNodes4Area(NODE* pNodes,int areaID,int nodeType);
int deleteAllReproductionTask();
int getTimeSpan();
int updateTimeSpan(int timeSpan);
int getAllReproduction(MODE* p_mode) ;



int getAllModel(MODE* p_mode) ;
int getDBModelInfo(MODE* p_mode, int modeID) ;
int getStaticModelInfo(MODE* p_mode, int modeID);
int updateModel(MODE* pModels) ;
int updateReproduction(MODE* pModels);
int insertReproduction(MODE* pModels) ;
int insertModel(MODE* pModels) ;
int deleteModel(int modeID);
int deleteReproduction(int modeID);

int getScheduleListbyPage(SCHEDULEINFO* p_schedule, char* status,int startNo);
int getScheduleList(SCHEDULEINFO* p_schedule, char* status);
int getSchedule(SCHEDULEINFO* p_schedule, int scheduleId);
int insertSchedule(SCHEDULEINFO* p_schedule);
int updateSchedule(SCHEDULEINFO* p_schedule);
int updateScheduleStatus(int status, int scheduleId);
int removeDBSchedule(int scheduleID);

int getTaskList(TASK* pTask,int scheduleId);
int getTaskInfo(TASK* pTask,int scheduleId,int squenece);
int insertTask(TASK* task, int scheduleID);
int updateReproductionTask(float during);
int deleteTask(int scheduleID);

int loginDB(char* userName, char* password,int *role);
int updateDB(char* userName, char* password);
int updateUser(char* userName, char* password,int role);
int saveUser(char* userName, char* password,int role);
int getAllUsers(USER* users);
int getUserInfo(USER* users, int userID) ;
int deleteUser(int userID);

void writeDBLog(COMMLOG *log);
int getCommLog(COMMLOG *log, int startTime, int endTime, int areaId,int startNo);

int getCommLogDetail(COMMLOG *log,int logId);

int updateServerName(char* serverName);
int getServerName(char* serverName);

int updateSendSetting(int reSendTimes, int iBroadcast, int sendConfirm);
int getSendSetting(int* reSendTimes,int *iBroadcast, int* sendConfirm);

int SaveTime(long timeNow);
int getSavedTime(long* savedTime);
int modifyLogs(int timeStoped);
int modifyWaitSchedule4Stop(int timeStoped);
int modifyRunningSchedule4Stop(int timeStoped);
int modifySchedule4Exception();

int setCurrentSquence(int squence, int squenceLeftTime, int scheduleId);
int setCurrentSquenceLeftTime(int squenceLeftTime, int scheduleId);
int getCurrentSquence(int* squence, int* squenceLeftTime, int scheduleId) ;
int insertBackDB(char* dbName, char* backDate);
int getBackDBList(BACKDB* dbInfo,char *order);
int setBackDBList(BACKDB* dbInfo, int recordCount);


void writeUserLog(USERLOG *log);
int getUserLog(USERLOG *log, int startTime, int endTime, char* userName,int startNo);

