#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#ifndef H_SYMBOL
#define H_SYMBOL
#include "symbol.h"
#endif
#include <time.h>
#define BUFFERSIZE 3072
#define BUFLEN 256
#define CTRL_CMD_LEN 35

//任务状态  0:等待 1:运行 2:暂停 3:结束 4:错误 5:立即执行
#define TASKWAIT 0
#define TASKRUNNING 1
#define TASKPAUSED 2
#define TASKSTOPED 3
#define TASKERROR 4
#define TASKRUNIMMEDIATELY 5

#define SENSORLEVELRATE 4 //1.2


#define SCHEDULEPOWER 60 //60 //任务间隔时间60(0.5小时=30min=SCHEDULEINTERVALS * SCHEDULEPOWER)与检测间隔时间的倍率
#define SCHEDULEINTERVALS  60//60//任务检测间隔时间 60s

#define CONNECTIONTIMEOUT  50//socket connection break time check

//指令类型
#define ASK_REGISTION  0
#define REGISTION  1
#define CTRL_COMMAND  2
#define SETTING_COMMAND  4
#define SYNCHRONIZATION 5
#define PROGRAM_SETTING 6
#define SYNCHRONIZATION_AUTOSEARCH 7
#define GROUPSETTING 8
#define TCP_OK 9


#define ONEDAYSECOND 86000 //how many seconds in 1 day.

//**********类型定义********

//Command Typedef
typedef struct cmdTable {
	char type[40];
	char MID[18];
	int groupID;
	int deviceID;
	char MAC[9];
	char appUID[20];
	int frameType;
	int onOff;
	int dimmer;
	int errorCode;
	char groupList[256];
	int CurrentDimmer;

	int isSensored;
	int roomSensor;
	int dayLightSensor;
	int sensorType;
} SERVERCOMMAND;

typedef struct groupsettingTable {
	char type[40];
	char MID[18];
	int groupID;
	char devices[1024];
	char MAC[9];
	int frameType;
} GroupSettingCommand;

//Auto check Dimmer by Sensor
typedef struct groupAutoDimmer {
	int groupID;
	int currentDimmer;
	char name[64];
	//int dist_Dimmer;   //
	//int miniSensorLevel;
	//int maxSensorLevel;
	int isSensored; //sensor is used or not
	int sensorType; //room sensor =1 / dayLight =2
	char roomSensorMAC[20];
	char daynightSensorMAC[20];

} GROUPAUTODIMMER;


typedef struct registionTable {
	char type[20];
	char appUID[32];
	int frameType;
	int orgID;
} REGISTIONINFO;


typedef struct nodeTable {
	int id;
	int groupId;
	char groupName[50];
	char mac[9];
	char memo[50];
	BOOL isValid;
	int nodeType;
	int model;
} NODE;

typedef struct groupTable {
	int id;
	char name[50];
	char roomSensorMAC[20];
	char dayNightSensorMAC[20];

} GROUP;

typedef struct commLoshr_memg {
	int id;
	long startTime;
	long endTime;
	int groupId;
	//char groupName[50];
	int dimmer;
	int optionType; //0:手动；1:定时
} COMMLOG;

typedef struct program {
	int id; //任务编号
	char name[64]; //schedule name
	char startTime[64]; //开始时间
	char endTime[64];
	int status; //0:等待 1:运行 2:暂停 3:结束

	long nextStartTime; //next run time
	long nextEndTime;  //next stop time

	char groupList[256];

	int dimmer; // dimmer

	int sensorLevel;

	int isLoop; //run once or more

	char week[64];

	int roomSensor;
	int DaylightSenesor;

	int motion;
	int motionDelayTime;
	int MotionDimmer;
	int MotionRoomSensor;
	int MotionSecureAlarm;



	int frameType; //for server command

} PROGRAM;


typedef struct shareMem {
	//BOOL serialLock;
	int programPlaceholder; //占位数量
	int groupPlaceholder;//占位数量

	PROGRAM* p_Program;//定时任务

	GROUPAUTODIMMER* p_groupDimmer;

	BOOL dbLocked;
	char uuid[32];
	long lastHeartBeat;
	long lastEnergeCostTime;

} SHAREMEM;


typedef struct EnergeCostTable{
	int groupID;
	long costValue;
}ENERGECOST;

union semun {// 信号量操作的联合结构  
 int val;  
 struct semid_ds *buf;  
 unsigned short *array;  
};

