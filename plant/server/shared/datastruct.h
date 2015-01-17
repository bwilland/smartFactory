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

#define SCHEDULEPOWER 120 //120 //任务间隔时间120(1小时=60min=SCHEDULEINTERVALS * SCHEDULEPOWER)与检测间隔时间的倍率
#define SCHEDULEINTERVALS  30//30//任务检测间隔时间 30s
#define QUERYINTERVALS  20 //20查询任务间隔时间 10分钟 激活设备用
#define REPRODUCTIONID  655350 //演示专用ID
#define LOGINTIMEOVER  600 //登录超时 10分钟 == 600s
//**********设备类型定义********

typedef struct nodeTable {
	int id;
	int areaId;
	char areaName[50];
	char mac[9];
	char memo[50];
	int type;
	float speed;
	float distance; //max distance
	float pulses;
	BOOL isValid;
	float current1;
	float current2;
	float current3;
	float current4;
} NODE;

typedef struct userTable {
	int userId;
	char userName[50];

	char userPwd[50];
	int role;
} USER;

typedef struct areaTable {
	int id;
	char name[50];
	char memo[50];
	int Kr;
	int Kb;
	int nodeCount;
} AREA;

typedef struct areainfoTable {
	int id;
	char name[50];
	char memo[50];
	int modeId;
	char modeName[50];
	char nextModeName[50];

	long t_Start; //开始时间
	long t_end; //结束时间

	int onoffST; //开关状态
	int optionType; //0:手动，1:自动

	int ppf1;
	int ppf2;
	int ppf3;
	int ppf4;

	int Kr;
	int Kb;

	BOOL isLocked; //定时任务执行中

} AREAINFO;

typedef struct modeTable {
	int id;
	char name[50];
	int type;

	float ppf1;
	float ppf2;
	float ppf3;
	float ppf4;

	int mode;
	int direction;
	float speed;
	float distance;

} MODE;

//typedef struct krkbTable {
//	int Kr;
//	int Kb;
//} KRKB;

typedef struct taskInfo {
	int scheduleId;
	int sequence;
	int modeId;
	char modeName[50];
	float during;
} TASK;

typedef struct commLoshr_memg {
	int id;
	long dateTime;
	long endTime;
	int modeId;
	char modeName[50];
	int areaId;
	char areaName[50];
	float PPF1;
	float PPF2;
	float PPF3;
	float PPF4;
	int optionType; //0:手动；1:定时
	int timeStoped; //断电时长;
} COMMLOG;

typedef struct userLog {
	int id;
	long logTime;
	int logType;
	char userName[64];
	char IP[64];
	char logResult[64];
	int isSucceed;
} USERLOG;

typedef struct schedule {
	int id; //任务编号
	long startTime; //开始时间
	long endTime;
	int status; //0:等待 1:运行 2:暂停 3:结束
	int timeleft; //任务总剩余时间 x 30"
	int taskTimeLeft; //当前分任务（模式）剩余时间 x 30"
	int currentSequence; //当前模式序号
	int maxModelSequence; //最大的模式序号
	char areaList[256];
	int taskType; //0:正常模式 1:演示模式 2:查询模式
} SCHEDULE;


typedef struct scheduleinfo {
	int id; //任务编号
	long startTime; //开始时间
	long endTime;
	int status; //0:等待 1:运行 2:暂停 3:结束
	char description[512];
	int modifyTime;
	char areaList[64];
	int maxSquence; //最大的序号
	int currentSquence; //当前运行的任务序号
	int currSquLeftTime; //当前任务剩余时间
} SCHEDULEINFO;

typedef struct userLogin {
	char clientIp[32];
	BOOL isLogin; //登录标志
	time_t lastLogin; //最后一次登录时间
	int role;
} USERLOGIN;

typedef struct shareMem {
	//BOOL serialLock;
	int areaPlaceholder; //占位数量
	int schedulePlaceholder; //占位数量
	//int userPlaceholder; //占位数量

	AREAINFO* p_Area; //设备
	SCHEDULE* p_Schedule; //定时任务
	//USER *p_User;

	int cmdSendTimes; //指令发送次数
	BOOL needSendConfirm; //是否查询定时任务执行结果
	BOOL isBroadcast; //是否广播发送

	BOOL dbLocked;

} SHAREMEM;

typedef struct shareUser {
	int userPlaceholder; //占位数量
	USERLOGIN *p_User;
	int role;

} SHAREUSER;

typedef struct requestStruct {
	char mac[9]; // MAC
	int onoffSt;
	float PPF1;
	float PPF2;
	float PPF3;
	float PPF4;

	int mode;
	int direction;
	float speed;
	float distance;

	float pulse;
	float maxSpeed;
	float maxLength;

	int channel;
	unsigned int pwm;
	float current;

	char groupIDs[64];

	int isBroadCast; //广播
} COMMANDINFO;

typedef struct ledInfoStruct {

	int ledNumber;
	int ledType;
	float ppf;
	float caliberated;

	float current;
	float miniCurrent;
	float fullCurrent;

} LEDSTATUS;

typedef struct motorInfoStruct {

	int mode;
	int direction;
	int switchFlag;
	int error;

	float distance;
	float currentDistance;

	float speed;
	float pulses;
	float maxSpeed;
	float maxLength;

} MOTORSTATUS;

typedef struct caliberateTable {
	int nodeId;
	char LEDS[64];
	int channel;
	char nodeIDs[256];
	int pwm;
	float current;
	float ppf;
} CALIBERATE;

typedef struct DBInfoTable {
	
	char dbName[64];
	char dbDate[64];
	
} BACKDB;

union semun { // 信号量操作的联合结构
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

