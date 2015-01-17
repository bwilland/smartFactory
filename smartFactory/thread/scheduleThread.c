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

#include "../shared/dbadapter.h"
#include "../shared/sharemem.h"
#include "../actions/node.h"
#include "../shared/lightingpack.h"
#include "cmdTransThread.h"
#include "heartBeatThread.h"

//pthread_t t_Schedule;
//pthread_t t_deal;

BYTE cmd[50];

void startProgramServer() {

	//pthread_create(&t_Schedule, NULL, schedule, NULL); //计划任务线程
	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		sleep(10);
		program();

	}

	fprintf(stderr, "=============Schedule server Ready ===============\n");
	fprintf(stderr, "=============Schedule server Ready ===============\n");
}

void* program() {

	int iCount = 0;
	long timeSpan = 0;
	SHAREMEM* shr_mem;
	time_t timeStamp_start, timeStamp_end; //保存开始/结束时间
	while (TRUE) {
		//fprintf(stderr, "--schedule");
		shr_mem = get_shm_Memory(TRUE); //必须在这里

		time(&timeStamp_start);
		fprintf(stderr, "-----------timeStamp_start==%ld ------------\n",
				timeStamp_start);
		//Save Time
		//SaveTime((long) timeStamp_start);
		PROGRAM* p_program = shr_mem->p_Program;

		for (iCount = 0; iCount < shr_mem->programPlaceholder; iCount++) {
			//fprintf(stderr,"iCount==%d",iCount);

			fprintf(stderr,
					"id==%d,status==%d, name==%s,nextStartTime == %ld ,nextEndTime==%ld,groups=%s\n",
					p_program->id, p_program->status, p_program->name,
					p_program->nextStartTime, p_program->nextEndTime,
					p_program->groupList);

			if (p_program->id == -1) { //已经过时,被删除
				//shr_mem->schedulePlaceholder--;
				p_program++;
				continue;
			}

			switch (p_program->status) {
			case TASKRUNNING:
				timeSpan = timeStamp_start - p_program->nextEndTime;
				if (timeSpan >= 0) {
					//closeSchedule(p_program);
					executeSchedule(p_program, shr_mem, TRUE);

					if (p_program->isLoop == 0) {

						p_program->status = TASKSTOPED;
						p_program->id = -1;
					} else {
						//Wait next startTime
						p_program->status = TASKWAIT;
						setProgramTime(p_program, timeStamp_start);
					}
				}
				break;

			case TASKWAIT:
				//current time

				timeSpan = timeStamp_start - p_program->nextStartTime;

				if (timeSpan >= 0) {
					executeSchedule(p_program, shr_mem, FALSE);
					p_program->status = TASKRUNNING;
				}
				break;
			case TASKPAUSED:
			case TASKERROR:
				//暂停/错误的任务

				break;
			case TASKSTOPED:
				//停止的任务
				p_program->id = -1;
				p_program++;
				continue;
				break;

			}

			p_program++;
		}
		//fprintf(stderr,"------------Schedule Thread Waiting-------------\n");
		time(&timeStamp_end);
		int time2sleep = timeStamp_start - timeStamp_end + SCHEDULEINTERVALS; //处理时间间隔
		release_shm_Memory(shr_mem, TRUE);
		if (time2sleep > 0) {
			sleep(time2sleep);
		}

	}

	return NULL ;
}

void executeSchedule(PROGRAM *p_program, SHAREMEM *shr_mem, int isStopSchedule) {
	fprintf(stderr, "-----------executeSchedule  %s ------\n", p_program->name);

	char p_Paras[256];
	char *token;
	strcpy(p_Paras, p_program->groupList);

	if (isStopSchedule
			|| (p_program->roomSensor == 0 && p_program->DaylightSenesor == 0)) {
		SERVERCOMMAND cmd;
		BYTE command[64];

		if (isStopSchedule) {
			//close Schedule
			cmd.dimmer = 0;
			cmd.onOff = 0;

		} else {
			cmd.dimmer = p_program->dimmer;
			if (cmd.dimmer > 0) {
				cmd.onOff = 1;
			} else {
				cmd.onOff = 0;
			}
		}
		strcpy(cmd.groupList, p_program->groupList);

		//fprintf(stderr, "%s,%d,%d\n", cmd.groupList,cmd.dimmer,cmd.onOff);

		int iLength = genGroupsControlFrame(&cmd, command);

		sendSingleData(command, iLength);

		if (isStopSchedule) {
			p_program->status = TASKWAIT;
		} else {
			p_program->status = TASKRUNNING;
		}

//LOG

		time_t timeNow;
		time(&timeNow);



		COMMLOG log;
		log.startTime = p_program->nextStartTime;
		log.dimmer = p_program->dimmer;
		log.endTime = timeNow;
		//strcpy(log.groupName ,"");//not used

		char *p_para = p_Paras;
		while (p_para != NULL ) {

			token = strsep(&p_para, ",");
			if (token != NULL ) {
				int groupID = atoi(token);
				log.groupId = groupID;

				writeDBLog(&log);

			}
		}
		fprintf(stderr, " writeDBLog OK.\n");
	} else {
		p_program->status = TASKRUNNING;

		fprintf(stderr, " p_program->roomSensor==%d \n", p_program->roomSensor);
		GROUPAUTODIMMER *groupAutoDimmer;
		int i = 0;
		groupAutoDimmer = shr_mem->p_groupDimmer;

		char *p_para = p_Paras;
		while (p_para != NULL ) {

			token = strsep(&p_para, ",");
			if (token != NULL ) {
				int groupID = atoi(token);
				for (i = 0; i < shr_mem->groupPlaceholder; i++) {
					if (groupAutoDimmer->groupID == groupID) {
						groupAutoDimmer->currentDimmer = p_program->dimmer;
//						fprintf(stderr,
//								" groupAutoDimmer->roomSensorMAC==%s;;;groupAutoDimmer->daynightSensorMAC ==%s\n",
//								groupAutoDimmer->roomSensorMAC,
//								groupAutoDimmer->daynightSensorMAC);

						if (p_program->roomSensor == 1) {
							groupAutoDimmer->isSensored = 1;
							groupAutoDimmer->sensorType = 1;
						} else {
							groupAutoDimmer->isSensored = 1;
							groupAutoDimmer->sensorType = 2;
						}

						fprintf(stderr,
								" groupAutoDimmer->isSensored==%d ;;;groupAutoDimmer->sensorType ==%d\n",
								groupAutoDimmer->isSensored,
								groupAutoDimmer->sensorType);

						break;
					}

					groupAutoDimmer++;
				}

			}
		}
		checkDimmer();
	}

}

void executeScheduleImmediately(PROGRAM *p_program, SHAREMEM *shr_mem,
		time_t timeStamp_start) {

}

void closeSchedule(PROGRAM *p_program) {
	char groups[256];
	strcpy(groups, p_program->groupList);

}

