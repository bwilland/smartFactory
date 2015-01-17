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

int getGroupInfo(GROUP *group);
int getAllGroup(GROUPAUTODIMMER* groups);
int getProgramList(PROGRAM* p_program, char* status) ;
int saveProgram(PROGRAM* program);
int deleteProgram(PROGRAM* program);
int getSensorLevel(int sensorType, int sensorDimmer, int* iDimmer) ;


int getLastNodeId();
int insertNode(NODE* pNodes);
int deleteAllNode();
int deleteNode(char* deviceID) ;
int getAllNode(NODE* pNodes);
int getAllNodeWithPage(NODE* pNodes,int startNo);
int getDBNodeInfo(NODE* pNodes, int nodeID);
int getAllNodeList(NODE* pNodes);
int updateNode(NODE* pNodes);
int invalidAllNodes();
int validNode(char* mac);





void writeDBLog(COMMLOG *log);
int getEnergeCost(ENERGECOST* cost, int startTime, int endTime) ;
int modifyLogs(int timeStoped);






