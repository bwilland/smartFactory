/*
 * Schedule.c
 *	计划任务
 *  Created on: Aug 20, 2013
 *      Author: root
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>

#include <unistd.h>

#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "scheduleThread.h"
#include "../shared/serialport.h"
#include "../actions/control.h"
#include "../shared/dbadapter.h"
#include "../shared/sharemem.h"
#include "../actions/node.h"
#include "../shared/lightingpack.h"
#include "../boa/boa.h"

//pthread_t t_Schedule;
//pthread_t t_deal;

BYTE cmd[50];

void startShceduleServer() {

	//pthread_create(&t_Schedule, NULL, schedule, NULL); //计划任务线程
	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		sleep(3);
		schedule();

	}

	fprintf(stderr, "=============Schedule server Ready ===============\n");
	fprintf(stderr, "=============Schedule server Ready ===============\n");
}

void* schedule() {

	int iCount = 0;
	long timeSpan = 0;
	SHAREMEM* shr_mem;
	time_t timeStamp_start, timeStamp_end; //保存开始/结束时间
	while (TRUE) {
		//fprintf(stderr, "--schedule");
		shr_mem = get_shm_Memory(TRUE); //必须在这里

		time(&timeStamp_start);
		//Save Time
		//SaveTime((long) timeStamp_start);
		SCHEDULE* p_schedule = shr_mem->p_Schedule;

		for (iCount = 0; iCount < shr_mem->schedulePlaceholder; iCount++) {
			//fprintf(stderr,"iCount==%d",iCount);
			/*	fprintf(stderr,
			 "id==%d,status==%d, timeleft==%d,taskSqueue == %d ,taskTimeLeft==%d,Area=%s\n",
			 p_schedule->id, p_schedule->status, p_schedule->timeleft,
			 p_schedule->currentSequence, p_schedule->taskTimeLeft,
			 p_schedule->areaList);*/

			if (p_schedule->id == -1) { //已经过时,被删除
				//shr_mem->schedulePlaceholder--;
				p_schedule++;
				continue;
			}

			switch (p_schedule->status) {
			case TASKRUNNING:
				//运行中的任务
				p_schedule->timeleft--; //任务剩余时间减去1
				p_schedule->taskTimeLeft--; //当前模式时间减去1
				/*				if (p_schedule->taskType == 0) {
				 //普通定时任务，保存子任务剩余时间，断电恢复用
				 setCurrentSquenceLeftTime(p_schedule->taskTimeLeft,
				 p_schedule->id);
				 }*/
				break;
			case TASKWAIT:
				//fprintf(stderr,"wait  timeStamp_start==%d,p_schedule->startTime==%d,%d",timeStamp_start,p_schedule->startTime,p_schedule->endTime);
				//等待中的任务
				timeSpan = timeStamp_start - p_schedule->startTime;

				if (timeSpan >= 0) {
					if (timeSpan > SCHEDULEINTERVALS) {
						//开始时间到
						p_schedule->timeleft = (p_schedule->endTime
								- timeStamp_start) / SCHEDULEINTERVALS;

						long timePassed = timeStamp_start
								- p_schedule->startTime; //过去的时间
						float iTimePassed = (float) timePassed
								/ (float) (SCHEDULEPOWER * SCHEDULEINTERVALS); //过去多少个0.5小时

						fprintf(stderr, "iTimePassed == %f\n", iTimePassed);

						char clause[64];
						sprintf(clause, "SCHEDULEID=%d", p_schedule->id);
						int iCount = getResultCount("TASK", clause);
						TASK tasks[iCount];
						getTaskList(tasks, p_schedule->id);

						int i = 0;
						float iTaskTimes = 0.0f; //运行到当前任务结束总共需要的时间
						BOOL isContinue = TRUE;
						while (isContinue) {
							//模拟时间pass
							for (i = 0; i < iCount; i++) {
								iTaskTimes += tasks[i].during;
								if (iTimePassed >= tasks[i].during) {
									iTimePassed -= tasks[i].during;
									//fprintf(stderr, "iTimePassed == %f\n", iTimePassed);
								} else {
									isContinue = FALSE;
									break;
								}

							}
						}

						p_schedule->currentSequence = tasks[i].sequence;

						long taskTimeLeft = (iTaskTimes * SCHEDULEPOWER
								* SCHEDULEINTERVALS) - timePassed;
						p_schedule->taskTimeLeft = taskTimeLeft
								/ SCHEDULEINTERVALS;

						fprintf(stderr,
								"taskTimeLeft==%d ,i==%d,currentSequence==%d\n",
								p_schedule->taskTimeLeft, i,
								p_schedule->currentSequence);

						p_schedule->status = TASKRUNNING;

						updateScheduleStatus(TASKRUNNING, p_schedule->id);

						executeScheduleImmediately(p_schedule, shr_mem,
								timeStamp_start);
					} else {
						p_schedule->taskTimeLeft = 0;
						p_schedule->currentSequence = 0;
						p_schedule->status = TASKRUNNING;
						updateScheduleStatus(TASKRUNNING, p_schedule->id);
					}

				} else {
					p_schedule++;
					continue;
				}
				break;
			case TASKPAUSED:
			case TASKERROR:
				//暂停/错误的任务
				p_schedule->timeleft--; //任务剩余时间减去1
				if (p_schedule->timeleft > 0) {
					p_schedule++;
					continue;
				}
				break;
			case TASKSTOPED:
				//停止的任务
				p_schedule->id = -1;
				p_schedule++;
				continue;
				break;
			case TASKRUNIMMEDIATELY:
				//立即执行的任务
				executeScheduleImmediately(p_schedule, shr_mem,
						timeStamp_start);
				p_schedule++;
				continue;
				break;
			}

			/*			fprintf(stderr,
			 "测试:R-->p_schedule->id==%d,status==%d, time left==%d, taskTimeLeft==%d\n",
			 p_schedule->id, p_schedule->status, p_schedule->timeleft,
			 p_schedule->taskTimeLeft);*/

			//if (p_schedule->status == TASKRUNNING) {
			if (p_schedule->timeleft <= 0) {
				//运行结束时间到
				if (p_schedule->taskType == 0) {
					//普通任务关闭掉
					closeSchedule(p_schedule, shr_mem, timeStamp_start);

					updateScheduleStatus(TASKSTOPED, p_schedule->id);
					p_schedule->status = TASKSTOPED; //结束
					p_schedule->id = -1; //设置成无效
				} else {
					//演示和查询不停止
					p_schedule->timeleft = 65535;
				}

			} else {
				//----------------------这里执行任务-----------------------------------
				if (p_schedule->taskTimeLeft <= 0) {
					int modelId = 0;
					executeSchedule(p_schedule, shr_mem, timeStamp_start,
							&modelId);
					//----------------------执行任务结束,查询执行结果-----------------------------------
					//fprintf(stderr,"shr_mem->needSendConfirm==%d\n",shr_mem->needSendConfirm);
					//if (shr_mem->needSendConfirm) {
					//	sleep(1);
					//	checkExecResult(p_schedule->areaList, shr_mem);
					//}
				}
			}

			p_schedule++;
		}
		//fprintf(stderr,"------------Schedule Thread Waiting-------------\n");
		time(&timeStamp_end);
		int time2sleep = timeStamp_start - timeStamp_end + SCHEDULEINTERVALS; //处理时间间隔
		//fprintf(stderr,"timeSleep==%d;;  \n", time2sleep);
		//fprintf(stderr, "--schedule");
		release_shm_Memory(shr_mem, TRUE);
		if (time2sleep > 0) {
			sleep(time2sleep);
		}

	}

	return NULL ;
}
//
//void checkExecResult(char *areaIds, SHAREMEM *share_mem) {
//	int i = 0, areaId = 0;
//	BYTE receiver[BUFFERSIZE], command[128], resultCMP[10];
//
//	char tmp[32], *token;
//
//	COMMANDINFO cInfo;
//	bzero(&cInfo, sizeof(COMMANDINFO));
//
//	while (areaIds != NULL ) {
//		token = strsep(&areaIds, ",");
//		areaId = atoi(token);
//
////fprintf(stderr,"areaId==%d\n",areaId);
//
//		sprintf(tmp, "AREA=%d AND ISVALID=1", areaId);
//		int nodeCount = getResultCount("NODES", tmp);
//		AREAINFO* p_area = share_mem->p_Area;
//		AREAINFO areaInfo;
//		BOOL isFound = FALSE;
//
//		for (i = 0; i < share_mem->areaPlaceholder; i++) {
//			//fprintf(stderr,"p_area->id==%d, red=%d, blue=%d \n",p_area->id,p_area->red,p_area->blue);
//
//			if (p_area->id == areaId) {
//				memcpy(&areaInfo, p_area, sizeof(AREAINFO));
//				isFound = TRUE;
//				break;
//			}
//			p_area++;
//		}
//
//		if (isFound) {
//			bzero(resultCMP, 10);
//			resultCMP[0] = (BYTE) areaInfo.onoffST;
//			resultCMP[1] = (BYTE) areaInfo.ppf1;
//			resultCMP[2] = (BYTE) areaInfo.ppf2;
//			resultCMP[3] = (BYTE) areaInfo.ppf3;
//			resultCMP[4] = (BYTE) areaInfo.ppf4;
//
//			if (nodeCount > 0) {
//				NODE nodes[nodeCount];
//				getNodeByAreaId(nodes, areaId);
//
//				for (i = 0; i < nodeCount; i++) {
//					int iCount = genQueryFrame(nodes[i].mac, command);
//					//printHEX(command, iCount);
//					int readCount = serialData(command, iCount, 1, receiver);
//					//printHEX(receiver, readCount);
//					/*if (!analysisResponse(receiver, readCount, &(nodes[i]),
//					 resultCMP)) {
//					 //查询结果不正确，再发一次
//					 strcpy(cInfo.mac, nodes[i].mac);
//					 cInfo.PPF1 = areaInfo.ppf1;
//					 cInfo.PPF2 = areaInfo.ppf2;
//					 cInfo.PPF3 = areaInfo.ppf3;
//					 cInfo.PPF4 = areaInfo.ppf4;
//					 cInfo.onoffSt = areaInfo.onoffST;
//					 iCount = genControlFrame(&cInfo, command);
//					 fprintf(stderr, "TRY again: %s \n", cInfo.mac);
//					 //printHEX(command, iCount);
//
//					 for (iResend = 0; iResend < 2; iResend++) {
//					 sendData(command, iCount, 1);
//					 }
//					 }
//					 */
//				}
//
//			}
//		}
//
//	}
//}

void executeSchedule(SCHEDULE *p_schedule, SHAREMEM *shr_mem,
		time_t timeStamp_start, int *modelID) {
	if (p_schedule->taskType < 2) {
		//*******************定时任务************************/
		//当前模式执行完毕，开始下一个模式
		p_schedule->currentSequence++;
		if (p_schedule->currentSequence > p_schedule->maxModelSequence) {
			//已经是最大的序号了，从头开始
			p_schedule->currentSequence = 1;
		}
		TASK task;
		int iCount = getTaskInfo(&task, p_schedule->id,
				p_schedule->currentSequence);
		if (iCount == 1) {
			if (p_schedule->taskType == 0) {
				p_schedule->taskTimeLeft = task.during * SCHEDULEPOWER; //设置下个模式的持续时间

				//保存当前任务序号和时间--断电用
				//	setCurrentSquence(p_schedule->currentSequence,
				//			p_schedule->taskTimeLeft, p_schedule->id);
				*modelID = task.modeId; //返回模式ID

			} else {
				//p_schedule->taskTimeLeft = 1; //TODO:演示持续时间 30“
				p_schedule->taskTimeLeft = task.during;
			}

			executeTask(shr_mem, p_schedule, &task, timeStamp_start); //执行

			//检查
//			if (shr_mem->needSendConfirm) {
//				sleep(1);
//				char areaList[256];
//				strcpy(areaList, p_schedule->areaList);
//				checkExecResult(areaList, shr_mem);
//			}

		} else {
			//此计划下没有子任务
			p_schedule->taskTimeLeft = 0;
			updateScheduleStatus(TASKERROR, p_schedule->id);
			p_schedule->status = TASKERROR; //设置成错误

			fprintf(stderr, "Error: task [%d] not exist。\n",
					p_schedule->currentSequence);
		}
	} else {

		//====================查询模式==========================

//		BYTE command[32];
//		int iCount = genBroadCastQueryFrame(command);
//		printHEX(command, iCount);
//		BYTE recBuff[10240];
//		iCount = serialData(command, iCount, 1, recBuff);
//		p_schedule->taskTimeLeft = QUERYINTERVALS;

	}
}

void executeScheduleImmediately(SCHEDULE *p_schedule, SHAREMEM *shr_mem,
		time_t timeStamp_start) {

//*******************立即任务************************/

	TASK task;
	getTaskInfo(&task, p_schedule->id, p_schedule->currentSequence);

//保存当前任务序号和时间--断电用
//	setCurrentSquence(p_schedule->currentSequence, p_schedule->taskTimeLeft,
//			p_schedule->id);

	p_schedule->status = TASKRUNNING;
	executeTask(shr_mem, p_schedule, &task, timeStamp_start); //执行

}

void executeTask(SHAREMEM* share_mem, SCHEDULE *p_schedule, TASK *pTask,
		time_t t_start) {
//fprintf(stderr,"Execute start!!!! \n");

	//fprintf(stderr, "shr_mem==%p \n", share_mem);
	int i = 0;
	AREAINFO areaInfo;
	BOOL isSuccess = FALSE;

//	if (p_schedule->taskType == 1) {
//		//演示模式
//		log_error_time();
//		fprintf(stderr, "Exec Reproduction hid == %d, sequence== %d  \n",
//				p_schedule->id, p_schedule->currentSequence);
//
//		bzero(&areaInfo, sizeof(AREAINFO));
//
//
////		isSuccess = sendModel2Area(0, pTask->modelId, 1, TRUE, &areaInfo,
////				share_mem->cmdSendTimes, TRUE);
//		isSuccess = sendModel2Area(0, pTask->modelId, 1, TRUE, &areaInfo,
//						share_mem->cmdSendTimes, FALSE);
//		//fprintf(stderr, "Exec Reproduction isSuccess== %d  \n", isSuccess);
//
//		if (isSuccess) {
//			//更新内存数据
//			AREAINFO *p_area;
//			p_area = share_mem->p_Area;
//			for (i = 0; i < share_mem->areaPlaceholder; i++) {
//
//				p_area->modelId = areaInfo.modelId;
//				p_area->ppf1 = areaInfo.ppf1;
//				p_area->ppf2 = areaInfo.ppf2;
//				p_area->ppf3 = areaInfo.ppf3;
//				p_area->ppf4 = areaInfo.ppf4;
//
//				strcpy(p_area->modelName, areaInfo.modelName);
//
//				//保存区域模式数据
//				p_area->onoffST = areaInfo.onoffST;
//
//				p_area++;
//			}
//
//		}
//	} else {
	//正常模式

	int areaId;
	char *token;
	BOOL isFound = FALSE;
	char areaList[256];
	strcpy(areaList, p_schedule->areaList);

	char *areaIds = areaList;

	while (areaIds != NULL ) {
		token = strsep(&areaIds, ",");
		areaId = atoi(token);

		log_error_time();
		fprintf(stderr, "Exec hid == %d, sequence== %d, areaId==%d  \n",
				p_schedule->id, p_schedule->currentSequence, areaId);
		isFound = FALSE;
		bzero(&areaInfo, sizeof(AREAINFO));
		AREAINFO* p_area = share_mem->p_Area;
		for (i = 0; i < share_mem->areaPlaceholder; i++) {
			if (p_area->id == areaId) {

				//保留原始数据
				areaInfo.id = areaId;
				strcpy(areaInfo.name, p_area->name);
				strcpy(areaInfo.memo, p_area->memo);
				areaInfo.Kr = p_area->Kr;
				areaInfo.Kb = p_area->Kb;
				areaInfo.optionType = 1; //定时通讯
				areaInfo.t_Start = t_start;
				areaInfo.isLocked = p_area->isLocked;

				if (p_schedule->id != REPRODUCTIONID) {
					areaInfo.isLocked = TRUE;
				}

				isFound = TRUE;
				break;
			}
			p_area++;
		}
		if (!isFound) {
			//这个区域数据不存在了
			fprintf(stderr, "Error: Area [%d] not exist。\n", areaId);
			continue;
		}
		fprintf(stderr, "area found %d\n", areaId);
		isSuccess = FALSE;

		isSuccess = sendModel2Area(areaId, pTask->modeId, 1, FALSE, &areaInfo,
				share_mem->cmdSendTimes, share_mem->isBroadcast);

		if (isSuccess) {
			//更新内存数据
			memcpy(p_area, &areaInfo, sizeof(AREAINFO));

		}

	}
	//}
//fprintf(stderr,"Execute end!!!! \n");
}

void closeSchedule(SCHEDULE *p_schedule, SHAREMEM *shr_mem,
		time_t timeStamp_start) {

//fprintf(stderr,"close schedule start!!!! \n");
	int i = 0;
	BOOL isSuccess = FALSE;
	AREAINFO areaInfo;

//	if (p_schedule->taskType == 1) {
//		//演示模式
//		log_error_time();
//		fprintf(stderr, "Exec Reproduction hid == %d, sequence== %d  \n",
//				p_schedule->id, p_schedule->currentSequence);
//
//		bzero(&areaInfo, sizeof(AREAINFO));
//
//		isSuccess = sendModel2Area(0, 1, 0, FALSE, &areaInfo,
//				shr_mem->cmdSendTimes, TRUE);
//
//		//fprintf(stderr, "Exec Reproduction isSuccess== %d  \n", isSuccess);
//
//		if (isSuccess) {
//			//更新内存数据
//			AREAINFO *p_area;
//			p_area = shr_mem->p_Area;
//			for (i = 0; i < shr_mem->areaPlaceholder; i++) {
//
//				p_area->modelId = areaInfo.modelId;
//				p_area->ppf1 = areaInfo.ppf1;
//				p_area->ppf2 = areaInfo.ppf2;
//				p_area->ppf3 = areaInfo.ppf3;
//				p_area->ppf4 = areaInfo.ppf4;
//				strcpy(p_area->modelName, areaInfo.modelName);
//
//				//保存区域模式数据
//				p_area->onoffST = areaInfo.onoffST;
//
//				p_area++;
//			}
//
//		}
//	} else {
	int areaId;
	char *token;
	BOOL isFound = FALSE;

	char areaList[256];
	strcpy(areaList, p_schedule->areaList);

	char *areaIds = areaList;

	while (areaIds != NULL ) {
		token = strsep(&areaIds, ",");
		areaId = atoi(token);

		isFound = FALSE;
		bzero(&areaInfo, sizeof(AREAINFO));
		AREAINFO* p_area = shr_mem->p_Area;
		for (i = 0; i < shr_mem->areaPlaceholder; i++) {
			if (p_area->id == areaId) {

				//保留原始数据
				areaInfo.id = areaId;
				strcpy(areaInfo.name, p_area->name);
				strcpy(areaInfo.memo, p_area->memo);
				areaInfo.Kr = p_area->Kr;
				areaInfo.Kb = p_area->Kb;
				areaInfo.optionType = 1; //定时通讯
				areaInfo.t_Start = timeStamp_start;
				areaInfo.isLocked = FALSE;

				isFound = TRUE;
				break;
			}
			p_area++;
		}
		if (!isFound) {
			//这个区域数据不存在了
			fprintf(stderr, "Error: Area [%d] not exist。\n", areaId);
			continue;
		}
		isSuccess = FALSE;
		//关闭
		isSuccess = sendModel2Area(areaId, 1, 0, FALSE, &areaInfo,
				shr_mem->cmdSendTimes, shr_mem->isBroadcast);

		if (isSuccess) {
			//更新内存数据
			memcpy(p_area, &areaInfo, sizeof(AREAINFO));

		}

	}
//fprintf(stderr,"close schedule end!!!! \n");analysisResponse
	//}
}
int analysisResponse(BYTE* p_Response, int resp_size, NODE* node,
		BYTE* resultCMP) {
	char addr[5], *tmp_Addr;
	int i = 0;
	if (resp_size != 25) {
		return FALSE;
	}
	/*fprintf(stderr,"ananly cmp==\n");
	 printHEX(resultCMP,10);
	 printHEX(&(p_Response[13]),10);*/
	if (p_Response[0] == 0x48 && p_Response[1] == 0x59) {
		if (p_Response[12] == 0x0A) {
			sprintf(addr, "%02X%02X", p_Response[6], p_Response[7]);
			//printf("addr==%s \n", addr);
			tmp_Addr = node->mac;
			tmp_Addr += 4;		//后4位
			//printf("tmp_Addr==%s \n", tmp_Addr);

			if (strcmp(tmp_Addr, addr) == 0) {
				for (i = 0; i < 10; i++) {

					if (p_Response[13 + i] != resultCMP[i]) {
						return FALSE;
					}
				}

			} else {
				return FALSE;
			}

		}
	} else {
		return FALSE;
	}

	return TRUE;
	//return FALSE;
}
