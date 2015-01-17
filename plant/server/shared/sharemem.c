#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/sem.h>  
#include <sys/ipc.h>  

#include "sharemem.h"
#include "../thread/scheduleThread.h"

int shm_id = 0, sem_id = 0, usr_shm_id = 0;
const int MEM_HEAD_LEN = 256;
const int MEM_AREA_LEN = 15360; //15K
const int MEM_SCHEDULE_LEN = 10240; //10k
const int MEM_USER_LEN = 4096;

int createShareMem() {

	SHAREMEM* share_mem;

	shm_id = shmget(IPC_PRIVATE, 30720, IPC_CREAT | IPC_EXCL | 0600);
	if (shm_id == -1) {
		perror("shmget error");
		return -1;
	}
	fprintf(stderr, "shm_id ==%d \n", shm_id);

	char* p_mem = (char*) shmat(shm_id, NULL, 0); //加载内存
	bzero(p_mem, 30720);

	share_mem = (SHAREMEM*) p_mem;
	share_mem->p_Area = (AREAINFO*) (p_mem + MEM_HEAD_LEN);
	share_mem->p_Schedule = (SCHEDULE*) (p_mem + MEM_HEAD_LEN + MEM_AREA_LEN);
	//share_mem->p_User = (USER*) (p_mem + MEM_HEAD_LEN + MEM_AREA_LEN
	//		+ MEM_SCHEDULE_LEN);
	//share_mem->serialLock = FALSE;
	getSendSetting(&(share_mem->cmdSendTimes), &(share_mem->isBroadcast),
			&(share_mem->needSendConfirm));
	fprintf(stderr, "cmdSendTimes==%d \n", share_mem->cmdSendTimes);

	if ((int) share_mem == -1) {
		perror("shmat error");
		return -1;
	}

	fill_shm_Areas(share_mem);
	fill_shm_Schedule(share_mem);

	shmdt(share_mem); //脱离共享
	fprintf(stderr, "==========createShareNodes OK==========\n");

//信号量
	sem_id = CreateSem();

	//用户存储区
	usr_shm_id = shmget(IPC_PRIVATE, MEM_USER_LEN, IPC_CREAT | IPC_EXCL | 0600);
	if (usr_shm_id == -1) {
		perror("shmget2");
		exit(1);
	}
	p_mem = (char*) shmat(usr_shm_id, NULL, 0); //加载内存
	bzero(p_mem, MEM_USER_LEN);
	shmdt(p_mem);

	return 0;
}

/*初始化共享内存中的区域*/
void fill_shm_Areas(SHAREMEM* shr_mem) {

	fprintf(stderr, "Fill share memory by areas...\n");
	AREAINFO* pArea = shr_mem->p_Area;
	int areaCount = getResultCount("AREA", NULL );
	fprintf(stderr, "area count == %d  \n", areaCount);

	//int i=0;

	if (areaCount > 0) {
		AREAINFO areas[areaCount];
		bzero(areas, sizeof(AREAINFO) * areaCount);
		getAllArea(areas);

//		for (i = 0; i < areaCount; i++) {
//			fprintf(stderr,"name==%s \n",areas[i].name);
//		}

		memcpy(pArea, areas, sizeof(AREAINFO) * areaCount);

		//pArea++;

	}
	shr_mem->areaPlaceholder = areaCount;

//	for (i = 0; i < shr_mem->areaPlaceholder; i++) {
//						fprintf(stderr,"name==%s \n",pArea->name);
//						pArea++;
//			}
	fprintf(stderr, "Fill share memory by areas  END.\n");
}

/*初始化共享内存中的schedule*/
void fill_shm_Schedule(SHAREMEM* shr_mem) {

	fprintf(stderr, "Fill share memory by Schedule...\n");
	SCHEDULE *p_schedule = shr_mem->p_Schedule;
	SCHEDULE schedule;
	int i = 0, count = 0;

	bzero(&schedule, sizeof(SCHEDULE));

	//状态检测(默认)
	/*	schedule.id = 0;
	 schedule.maxModelSequence = 0; //当前设备 offset，临时借用maxModelSequence
	 schedule.taskTimeLeft = QUERYINTERVALS;
	 schedule.timeleft = 65535;
	 schedule.taskType = 2;
	 schedule.status = TASKRUNNING;
	 memcpy(p_schedule, &schedule, sizeof(SCHEDULE));

	 p_schedule++;
	 count++;
	 */
	//-- 状态检测(默认) end--
	int schduleCount = getResultCount("SCHEDULE", "STATUS IN(0,1,2,5)");
	fprintf(stderr, "SCHEDULE count == %d  \n", schduleCount);
	if (schduleCount > 0) {
		time_t timeNow;
		time(&timeNow);

		/*		//检测是否断电
		 long savedTime = 0;
		 getSavedTime(&savedTime);
		 if (savedTime > 0) {
		 long timeSpan = timeNow - savedTime;

		 //fprintf(stderr,"timeSpan==%ld \n", timeSpan);

		 if (timeSpan > SCHEDULEINTERVALS) {
		 //间隔超过60秒，处理数据库数据，延长定时结束时间
		 modifyLogs(timeSpan); //修改日志

		 //修改计划
		 modifyWaitSchedule4Stop(timeSpan);
		 modifyRunningSchedule4Stop(timeSpan);

		 }
		 }*/

		//断电
		modifySchedule4Exception(); //running --》 wait
		SCHEDULEINFO scheduleInfo[schduleCount];
		getScheduleList(scheduleInfo, "0,1,2");

		SCHEDULE schedule;

		for (i = 0; i < schduleCount; i++) {

			if (timeNow > scheduleInfo[i].endTime) {
				//已过时,设置为结束
				updateScheduleStatus(TASKSTOPED, scheduleInfo[i].id);
				continue;
			}

			bzero(&schedule, sizeof(SCHEDULE));
			schedule.id = scheduleInfo[i].id;

			schedule.maxModelSequence = scheduleInfo[i].maxSquence;
			//schedule.currentSequence = scheduleInfo[i].currentSquence;

			schedule.taskType = 0;
			schedule.startTime = scheduleInfo[i].startTime;
			schedule.endTime = scheduleInfo[i].endTime;
			strcpy(schedule.areaList, scheduleInfo[i].areaList);
			schedule.status = scheduleInfo[i].status;

			schedule.timeleft = (scheduleInfo[i].endTime
					- scheduleInfo[i].startTime) / SCHEDULEINTERVALS;
			schedule.status = TASKWAIT;
			schedule.taskTimeLeft = schedule.timeleft + 1;

			/*		if (scheduleInfo[i].startTime <= timeNow) {
			 //开始时间已过
			 if (scheduleInfo[i].status == TASKWAIT) {
			 scheduleInfo[i].status = TASKRUNNING;
			 updateScheduleStatus(TASKRUNNING, scheduleInfo[i].id);

			 }
			 schedule.timeleft = (scheduleInfo[i].endTime - timeNow)
			 / SCHEDULEINTERVALS;
			 if (scheduleInfo[i].status == TASKRUNNING) {
			 long timePassed = timeNow - scheduleInfo[i].startTime; //过去的时间
			 float iTimePassed = (float)timePassed
			 / (float)(SCHEDULEPOWER * SCHEDULEINTERVALS); //过去多少个0.5小时

			 fprintf(stderr, "iTimePassed == %f\n", iTimePassed);

			 char clause[64];
			 sprintf(clause, "SCHEDULEID=%d", schedule.id);
			 int iCount = getResultCount("TASK", clause);
			 TASK tasks[iCount];
			 getTaskList(tasks, schedule.id);

			 int i = 0, iTaskTimes = 0; //运行到当前任务结束总共需要的时间
			 BOOL isContinue = TRUE;
			 while (isContinue) {
			 //模拟时间pass
			 for (i = 0; i < iCount; i++) {
			 iTaskTimes += tasks[i].during;
			 if(iTimePassed >= tasks[i].during){
			 iTimePassed -= tasks[i].during;
			 //fprintf(stderr, "iTimePassed == %f\n", iTimePassed);
			 }else{
			 isContinue = FALSE;
			 break;
			 }

			 }
			 }

			 schedule.currentSequence = tasks[i].sequence;

			 long taskTimeLeft = (iTaskTimes * SCHEDULEPOWER
			 * SCHEDULEINTERVALS) - timePassed;
			 schedule.taskTimeLeft = taskTimeLeft / SCHEDULEINTERVALS;

			 fprintf(stderr, "taskTimeLeft==%d ,,currentSequence==%d\n",
			 schedule.taskTimeLeft, schedule.currentSequence);
			 schedule.status = TASKRUNIMMEDIATELY;


			 iCount = getTaskInfo(&task, schedule.id,
			 schedule.currentSequence);
			 if (iCount == 1) {

			 fprintf(stderr, "currSquLeftTime==%d \n",
			 scheduleInfo[i].currSquLeftTime);
			 //设置模式的剩余持续时间
			 //schedule.taskTimeLeft = scheduleInfo[i].currSquLeftTime;

			 schedule.status = TASKRUNIMMEDIATELY;

			 } else {
			 schedule.timeleft = 0;
			 schedule.taskTimeLeft = 65535;
			 }

			 } else {
			 //其他状态
			 schedule.taskTimeLeft = schedule.timeleft + 1;
			 }

			 } else {
			 //没到开始时间
			 schedule.timeleft = (scheduleInfo[i].endTime
			 - scheduleInfo[i].startTime) / SCHEDULEINTERVALS;
			 schedule.status = TASKWAIT;
			 schedule.taskTimeLeft = schedule.timeleft + 1;
			 }*/

			//fprintf(stderr,"schedule.status==%d ,,%d\n", schedule.status,schedule.endTime);
			memcpy(p_schedule, &schedule, sizeof(SCHEDULE));
			fprintf(stderr,
						 "shared id==%d,status==%d, timeleft==%d,taskSqueue == %d ,taskTimeLeft==%d,Area=%s\n",
						 p_schedule->id, p_schedule->status, p_schedule->timeleft,
						 p_schedule->currentSequence, p_schedule->taskTimeLeft,
						 p_schedule->areaList);
			p_schedule++;
			count++;
		}
	}
	shr_mem->schedulePlaceholder = count;

	fprintf(stderr, "Fill share memory by Schedule  END.\n");
}

SHAREMEM* get_shm_Memory(BOOL bLocked) {

	if (bLocked) {
		Sem_P();
	}
	char* p_mem = (char*) shmat(shm_id, NULL, 0);
	SHAREMEM* share_mem = (SHAREMEM*) p_mem;
	share_mem->p_Area = (AREAINFO*) (p_mem + MEM_HEAD_LEN);
	share_mem->p_Schedule = (SCHEDULE*) (p_mem + MEM_HEAD_LEN + MEM_AREA_LEN);
	//share_mem->p_User = (USER*) (p_mem + MEM_HEAD_LEN + MEM_AREA_LEN
	//		+ MEM_SCHEDULE_LEN);

	return share_mem;
}
SHAREUSER* get_shm_User() {

	char* p_mem = (char*) shmat(usr_shm_id, NULL, 0);
	SHAREUSER* share_mem = (SHAREUSER*) p_mem;
	share_mem->p_User = (USERLOGIN*) (p_mem + MEM_HEAD_LEN);

	return share_mem;
}

void release_shm_User(SHAREUSER* share_mem) {

	shmdt(share_mem);
}
void release_shm_Memory(SHAREMEM* share_mem, BOOL bUnlocked) {
	shmdt(share_mem); //脱离共享
	if (bUnlocked) {
		Sem_V();
	}
}

int checkLoginStaues(char* clientIp,int* iRole) {

	BOOL result = FALSE;
	SHAREUSER* shr_user = get_shm_User();

	if (shr_user->userPlaceholder == 0) {
		release_shm_User(shr_user);
		return FALSE;
	}

	USERLOGIN *p_user = shr_user->p_User;
	int i = 0;

	/*	for (i = 0; i < shr_user->userPlaceholder; i++) {
	 fprintf(stderr, "check===%s, %d, %d\n", p_user->clientIp,
	 p_user->isLogin, p_user->lastLogin);
	 p_user++;
	 }
	 p_user = shr_user->p_User;*/

	for (i = 0; i < shr_user->userPlaceholder; i++) {

		if (strcmp(p_user->clientIp, clientIp) == 0) {

			if (p_user->isLogin) {
				time_t timeNow;
				time(&timeNow);
				if (timeNow - p_user->lastLogin > LOGINTIMEOVER) {
					//登录超时,清除数据
					fprintf(stderr, "登录超时,清除数据\n");
					p_user->isLogin = FALSE;
					strcpy(p_user->clientIp, "");
					p_user->lastLogin = 0;
					result = FALSE;
				} else {
					*iRole = p_user->role;
					p_user->lastLogin = timeNow;
					result = TRUE;
				}

			} else {
				result = FALSE;
			}
			break;
		}
		p_user++;
	}

	release_shm_User(shr_user);
	return result;

}

int Sem_V() // 增加信号量
{
	struct sembuf sops = { 0, +1, SEM_UNDO }; // 建立信号量结构值
	//fprintf(stderr,"--unlock---\n");
	return (semop(sem_id, &sops, 1));    // 发送命令
}

int Sem_P()    //减少信号量值
{
	struct sembuf sops = { 0, -1, SEM_UNDO };    // 建立信号量结构值
	//fprintf(stderr,"--lock---\n");
	return (semop(sem_id, &sops, 1));
}

int CreateSem()    // 建立信号量, 键值key 及信号量的初始值value
{
	union semun sem;
	sem.val = 1;

	sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666); //获得信号量ID
	if (-1 == sem_id) {
		printf("create semaphore error\n");
		return -1;
	}

	semctl(sem_id, 0, SETVAL, sem); // 发送命令，建立value个初始值的信号量

	return sem_id;
}
