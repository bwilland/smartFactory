/*
 * scheduleact.c
 *
 *  Created on: Aug 22, 2013
 *      Author: root
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "schedule.h"
#include "node.h"
#include "control.h"
#include "../shared/serialport.h"
#include "../shared/sharemem.h"
#include "../shared/lightingpack.h"

void scheduleManage(char* request, char* responseMsg) {
	char* p_Paras;
	char* token;

	char dealPara[1024];
	memcpy(dealPara, request, 1024); // for save
//fprintf(stderr,"request==%s\n",request);
//fprintf(stderr,"dealPara==%s\n",dealPara);

	p_Paras = dealPara;
	token = strsep(&p_Paras, "?");
	char paras[10][256];
	int i = 0, iCount = 0;
	while (p_Paras != NULL ) {
		i++;
		token = strsep(&p_Paras, "&");
		if (token != NULL ) {
			strcpy(paras[iCount], token);
			iCount++;
		} else {
			break;
		}
	}

	char* actionPara = NULL;
	for (i = 0; i < iCount; i++) {
		if (strstr(paras[i], "METHOD") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			fprintf(stderr, "METHOD==%s\n", actionPara);
			break;
		}
	}
	//////actions////////////////////////////
	if (strcmp(actionPara, "SaveSchedule") == 0) {
		actionSaveSchedule(responseMsg, paras, iCount, request);
		return;
	}

	//get schedule list
	if (strcmp(actionPara, "ScheduleList") == 0) {
		char clause[16];
		int pageNo = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "CLAUSE") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(clause, actionPara);
				continue;
			}
			if (strstr(paras[i], "PAGENO") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				pageNo = atoi(actionPara);
				continue;
			}

		}
		//fprintf(stderr, "CLAUSE==%s,PAGENO==%d \n", clause, pageNo);
		actionScheduleList(responseMsg, clause, pageNo);
		return;
	}

	if (strcmp(actionPara, "DeleteSchedule") == 0) {
		int scheduleId = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "SCHEDULEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				scheduleId = atoi(actionPara);
				break;
			}

		}

		//fprintf(stderr,"scheduleId==%d \n", scheduleId);
		actionDeleteSchedule(responseMsg, scheduleId);
		return;
	}
	if (strcmp(actionPara, "ModifySchedule") == 0) {
		int scheduleId = 0, option = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "SCHEDULEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				scheduleId = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "OPTION") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				option = atoi(actionPara);
				continue;
			}

		}

		//fprintf(stderr,"scheduleId==%d,,%d \n", scheduleId, option);
		actionModifySchedule(responseMsg, scheduleId, option);
		return;
	}

	if (strcmp(actionPara, "ScheduleInfo") == 0) {
		int scheduleId = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "SCHEDULEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				scheduleId = atoi(actionPara);
				break;
			}

		}
		//fprintf(stderr,"scheduleId Info==%d \n", scheduleId);
		actionScheduleTaskInfo(responseMsg, scheduleId);
		return;
	}

	if (strcmp(actionPara, "ScheduleDetail") == 0) {
		int scheduleId = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "SCHEDULEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				scheduleId = atoi(actionPara);
				break;
			}

		}
		//fprintf(stderr,"scheduleId==%d \n", scheduleId);
		actionScheduleDetail(responseMsg, scheduleId);
		return;
	}

}

void actionScheduleList(char *responseMsg, char* clause, int pageNo) {
	char tmp[1024];
	char statusName[16];
	int i = 0;
	strcpy(responseMsg, "<root>");

	if (clause[strlen(clause) - 1] == ',') {
		clause[strlen(clause) - 1] = '\0';
	}
	sprintf(tmp, "STATUS IN (%s)", clause);
	int scheduleCount = getResultCount("SCHEDULE", tmp);
	int iCount = 10;
	//fprintf(stderr,"logCount==%d \n", logCount);
	if (pageNo == 0) {
		pageNo = 1;
	}

	if (scheduleCount > 0) {

		if (scheduleCount < (10 * pageNo)) { //一页不满
			iCount = scheduleCount - 10 * (pageNo - 1);
			makePageInfoResponse(responseMsg, pageNo, pageNo);
		} else {
			int maxPage = scheduleCount / 10;
			if (scheduleCount % 10 > 0) {
				maxPage++;
			}
			makePageInfoResponse(responseMsg, maxPage, pageNo);
		}

		//fprintf(stderr, "iCount==%d,pageNo==%d\n", scheduleCount, pageNo);

		SCHEDULEINFO scheduleList[iCount];
		getScheduleListbyPage(scheduleList, clause, (pageNo - 1) * 10);
		for (i = 0; i < iCount; i++) {
			getStatusName(scheduleList[i].status, statusName);
			char areanames[512];
			bzero(areanames, 512);
			getAreaNames(areanames, scheduleList[i].areaList);
			sprintf(tmp,
					"<schedule><id>%d</id><domain>%s</domain><statusid>%d</statusid><status>%s</status><starttime>%ld</starttime><endtime>%ld</endtime><description>%s</description><modifytime>%d</modifytime></schedule>",
					scheduleList[i].id, areanames, scheduleList[i].status,
					statusName, scheduleList[i].startTime,
					scheduleList[i].endTime, scheduleList[i].description,
					scheduleList[i].modifyTime);

			strcat(responseMsg, tmp);
		}

	} else {
		makePageInfoResponse(responseMsg, 1, 1);
	}

	strcat(responseMsg, "</root>");
	//fprintf(stderr, "%s\n", responseMsg);
}

void actionModifySchedule(char* responseMsg, int scheduleId, int option) {
	int i = 0;
	time_t timeNow;
	char areaList[256];
	SHAREMEM* share_mem = get_shm_Memory(TRUE);
	SCHEDULE *p_schedule = share_mem->p_Schedule;
	for (i = 0; i < share_mem->schedulePlaceholder; i++) {
		if (p_schedule->id == scheduleId) {
			break;
		}
		p_schedule++;
	}
//fprintf(stderr,"option=%d areaIds=%s\n",option,p_schedule->areaList);
	switch (option) { //option: 0:暂停 1:继续 2:结束
	case 0:
		p_schedule->status = TASKPAUSED;
		updateScheduleStatus(p_schedule->status, scheduleId);
		strcpy(areaList, p_schedule->areaList);
		fprintf(stderr, "Modi pause hid == %d, sequence== %d,  areaList==%s\n",
				p_schedule->id, p_schedule->currentSequence, areaList);
		release_shm_Memory(share_mem, TRUE);
		actionManufacture(responseMsg, areaList, 1, 2, TRUE); //进入观察模式
		makeReturnMsg(responseMsg, scheduleId, "modify schedule successes.");
		return;
		break;
	case 1:
		time(&timeNow);
		fprintf(stderr, "MOdi resume hid == %d, sequence== %d,  \n",
				p_schedule->id, p_schedule->currentSequence);
		executeScheduleImmediately(p_schedule, share_mem, timeNow);
		p_schedule->status = TASKRUNNING;

		break;
	case 2:
		time(&timeNow);
		//关闭任务
		closeSchedule(p_schedule, share_mem, timeNow);

		p_schedule->status = TASKSTOPED;
		p_schedule->timeleft = 0;
		break;
	}
	updateScheduleStatus(p_schedule->status, scheduleId);
	release_shm_Memory(share_mem, TRUE);
	makeReturnMsg(responseMsg, scheduleId, "modify schedule successes.");

}

void actionScheduleTaskInfo(char *responseMsg, int scheduleID) {
	fprintf(stderr, "actionScheduleInfo.. \n");
	char tmp[256];
	strcpy(responseMsg, "<root>");
	SCHEDULEINFO schedule;
	bzero(&schedule, sizeof(SCHEDULEINFO));

	getSchedule(&schedule, scheduleID);
	//fprintf(stderr,"schedule =%d \n", schedule.startTime);
	sprintf(tmp,
			"<schedule><starttime>%ld</starttime><endtime>%ld</endtime><description>%s</description><domain>%s</domain></schedule>",
			schedule.startTime, schedule.endTime, schedule.description,
			schedule.areaList);
	//fprintf(stderr,"tmp==%s \n", tmp);
	strcat(responseMsg, tmp);
	sprintf(tmp, "SCHEDULEID=%d", scheduleID);
	int taskCount = getResultCount("TASK", tmp);
	fprintf(stderr, "taskCount==%d\n", taskCount);
	if (taskCount > 0) {
		TASK task[taskCount];
		getTaskList(task, scheduleID);
		int i = 0;
		for (i = 0; i < taskCount; i++) {
			sprintf(tmp,
					"<task><sequence>%d</sequence><modelid>%d</modelid><name>%s</name><during>%.2f</during></task>",
					task[i].sequence, task[i].modeId, task[i].modeName,
					task[i].during);
			//fprintf(stderr,"tmp==%s \n", tmp);
			strcat(responseMsg, tmp);
		}

	}
	strcat(responseMsg, "</root>");
}

void actionScheduleDetail(char *responseMsg, int scheduleID) {
	char tmp[1024];
	char areanames[512], statusName[64];
	strcpy(responseMsg, "<root>");
	SCHEDULEINFO schedule;
	bzero(&schedule, sizeof(SCHEDULEINFO));

	getSchedule(&schedule, scheduleID);
	bzero(areanames, 512);
	getAreaNames(areanames, schedule.areaList);
	SHAREMEM* share_mem = get_shm_Memory(FALSE);
	SCHEDULE *p_schedule = share_mem->p_Schedule;
	int i = 0;
	BOOL isFound = FALSE;
	for (i = 0; i < share_mem->schedulePlaceholder; i++) {
		if (p_schedule->id == scheduleID) {
			isFound = TRUE;
			break;
		}
		p_schedule++;
	}
	int currentSequence = 0;
	if (isFound) {
		currentSequence = p_schedule->currentSequence;
	}
	bzero(statusName, 64);
	getStatusName(schedule.status, statusName);

	sprintf(tmp,
			"<schedule><description>%s</description><status>%s</status><domain>%s</domain><currentSequence>%d</currentSequence></schedule>",
			schedule.description, statusName, areanames, currentSequence);
	strcat(responseMsg, tmp);

	sprintf(tmp, "SCHEDULEID=%d", scheduleID);
	int taskCount = getResultCount("TASK", tmp);
	if (taskCount > 0) {
		TASK task[taskCount];
		getTaskList(task, scheduleID);
		int i = 0;
		for (i = 0; i < taskCount; i++) {
			sprintf(tmp,
					"<task><sequence>%d</sequence><modelid>%d</modelid><name>%s</name><during>%.2f</during></task>",
					task[i].sequence, task[i].modeId, task[i].modeName,
					task[i].during);
			strcat(responseMsg, tmp);
		}

	}
	release_shm_Memory(share_mem, FALSE);
	strcat(responseMsg, "</root>");
}

void actionSaveSchedule(char* responseMsg, char paras[][256], int iCount,
		char* p_task) {
	fprintf(stderr, "actionSaveschedule Action... \n");
	SCHEDULEINFO scheduleInfo;
	bzero(&scheduleInfo, sizeof(SCHEDULEINFO));

	char areaIds[64];
	char* p_tmp;
	int i = 0, maxSquence = 0;
	char* actionPara;
	//char* token;
	for (i = 0; i < iCount; i++) {
		if (strstr(paras[i], "SCHEDULEID") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			scheduleInfo.id = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "STARTTIME") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			scheduleInfo.startTime = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "ENDTIME") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			scheduleInfo.endTime = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "DESCRIPTION") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			strcpy(scheduleInfo.description, actionPara);
			continue;
		}
		if (strstr(paras[i], "AREAID") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			strcpy(areaIds, actionPara);
			continue;
		}
		if (strstr(paras[i], "MAXSQUENCE") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			maxSquence = atoi(actionPara);
			continue;
		}

	}

	if (areaIds[strlen(areaIds) - 1] == ',') {
		areaIds[strlen(areaIds) - 1] = '\0';
	}

	time_t timeNow;
	time(&timeNow);
	scheduleInfo.modifyTime = timeNow; //修改时间
	scheduleInfo.status = 0;
	strcpy(scheduleInfo.areaList, areaIds);

	SHAREMEM* share_mem = get_shm_Memory(TRUE);
	SCHEDULE *p_schedule = share_mem->p_Schedule;
	SCHEDULE schedule;
	bzero(&schedule, sizeof(SCHEDULE));
	//处理定时任务

	schedule.id = scheduleInfo.id;
	schedule.startTime = scheduleInfo.startTime;
	schedule.endTime = scheduleInfo.endTime;
	schedule.taskType = 0; //不是演示

	if (schedule.startTime > timeNow) {
		schedule.status = TASKWAIT; //等待 开始时间的到来
		//本次定时任务总剩余时间
		schedule.timeleft = (scheduleInfo.endTime - scheduleInfo.startTime)
				/ SCHEDULEINTERVALS;
		schedule.taskTimeLeft = schedule.timeleft + 1;

	} else {
		schedule.status = TASKRUNNING; //运行
		//本次定时任务总剩余时间
		schedule.timeleft =
				(scheduleInfo.endTime - timeNow) / SCHEDULEINTERVALS;
		schedule.taskTimeLeft = 0;
	}

	//fprintf(stderr,"schedule.status==%d\n", schedule.status);

	//schedule.taskTimeLeft = 0;
	scheduleInfo.status = schedule.status;
	schedule.currentSequence = 0;
	strcpy(schedule.areaList, scheduleInfo.areaList);
	schedule.maxModelSequence = maxSquence;
	scheduleInfo.maxSquence = maxSquence;

	if (scheduleInfo.id == -1) {

		//===new schedule====
		scheduleInfo.id = insertSchedule(&scheduleInfo);
		schedule.id = scheduleInfo.id;
		//更新共享内存内的计划任务
		BOOL isUpdated = FALSE; //是否插入空闲处
		for (i = 0; i < share_mem->schedulePlaceholder; i++) {

			if (p_schedule->id == -1) { //空闲处
				isUpdated = TRUE;
				break;
			}
			p_schedule++;
		}

		memcpy(p_schedule, &schedule, sizeof(SCHEDULE));
		if (!isUpdated) {
			//拷贝到最后一个
			share_mem->schedulePlaceholder++;
		}

	} else {
		//====update DB===
		updateSchedule(&scheduleInfo);
		deleteTask(scheduleInfo.id); //remove all devices in schedule
		BOOL isFound = FALSE;
		//更新共享内存内的计划任务
		for (i = 0; i < share_mem->schedulePlaceholder; i++) {
			if (p_schedule->id == scheduleInfo.id) {
				isFound = TRUE;
				memcpy(p_schedule, &schedule, sizeof(SCHEDULE));
				break;
			}
			p_schedule++;
		}

		if (!isFound) { //已经停止的任务重新开始
			//更新共享内存内的计划任务
			p_schedule = share_mem->p_Schedule;
			BOOL isUpdated = FALSE; //是否插入空闲处
			for (i = 0; i < share_mem->schedulePlaceholder; i++) {

				if (p_schedule->id == -1) { //空闲处
					isUpdated = TRUE;
					break;
				}
				p_schedule++;
			}

			memcpy(p_schedule, &schedule, sizeof(SCHEDULE));
			if (!isUpdated) {
				//拷贝到最后一个
				share_mem->schedulePlaceholder++;
			}
		}
	}

	//处理子任务
	p_task = strstr(p_task, "MODELLIST");
	if (p_task != NULL ) {
		//找到开始位置
		while (*p_task != '\0') {
			if (*p_task == '[') {
				break;
			}
			p_task++;
		}

		//找到结束位置
		p_tmp = p_task;
		while (*p_tmp != '\0') {
			if (*p_tmp == '&') {
				*p_tmp = '\0';
				break;
			}
			p_tmp++;
		}

		//保存设备
		p_tmp = p_task;
		char* p_tmp_end;
		while (*p_tmp != '\0') {
			if (*p_tmp == '{') { //子任务开始标志
				p_tmp_end = ++p_tmp;
				while (*p_tmp_end != '\0') {
					if (*p_tmp_end == '}') { //子任务结束标志
						*p_tmp_end = '\0';
						//save device;
						//fprintf(stderr,"Save devices==%s\n", p_tmp);
						saveTask(scheduleInfo.id, p_tmp);

						p_tmp = p_tmp_end;
						break;
					}
					p_tmp_end++;
				}
			}

			p_tmp++;
		}
	}
	//fprintf(stderr,"p_schedule->status==%d\n", p_schedule->status);
	int returnCode = 1;
	//立即启动schedule
	if (p_schedule->status == TASKRUNNING) {
		//int modelId = 0;
		//executeSchedule(p_schedule, share_mem, timeNow, &modelId);

		//p_schedule->status = TASKRUNIMMEDIATELY;
		/*p_schedule->currentSequence = 1;
		 TASK task;
		 getTaskInfo(&task, p_schedule->id, p_schedule->currentSequence);

		 p_schedule->taskTimeLeft = task.during * SCHEDULEPOWER; //设置模式的持续时间*/

		//开始时间 早于当前时间，处理当前task持续时间
		long timePassed = timeNow - p_schedule->startTime; //过去的时间
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

		long taskTimeLeft = (iTaskTimes * SCHEDULEPOWER * SCHEDULEINTERVALS)
				- timePassed;
		p_schedule->taskTimeLeft = taskTimeLeft / SCHEDULEINTERVALS;

		fprintf(stderr, "taskTimeLeft==%d ,,currentSequence==%d\n",
				p_schedule->taskTimeLeft, p_schedule->currentSequence);

		//开始时间 早于当前时间，处理当前task持续时间 end

		//returnCode = 2;
		executeScheduleImmediately(p_schedule, share_mem, timeNow);
	}

	release_shm_Memory(share_mem, TRUE);
	makeReturnMsg(responseMsg, returnCode, "save schedule successes.");

	fprintf(stderr, "actionSaveschedule Action...END \n");
}

BOOL saveTask(int scheduleID, char* str_task) {

//insert tasks into table and Hard-task;
	char c_task[strlen(str_task) + 1];
	char *p_task;
	p_task = c_task;
	//fprintf(stderr,"str_task==%s \n", str_task); //"id":2,"onoffst":0,"ligst":0,"coltmp":0,"color":0,"night":0

	while (*str_task != '\0') {
		if (*str_task != '\"') { //remove (")
			memcpy(p_task, str_task, 1);
			p_task++;
		}
		str_task++;
	}
	*p_task = '\0';
	p_task = c_task;
	//fprintf(stderr,"c_task==%s \n", c_task); //id:2,onoffst:0,ligst:0,coltmp:0,color:0,night:0

	TASK task;
	memset(&task, 0, sizeof(TASK));
	task.scheduleId = scheduleID;

	char* nodeName;
	char* nodeValue;
	while (p_task != NULL ) {
		nodeName = strsep(&p_task, ":");
		nodeValue = strsep(&p_task, ",");

		if (nodeName != NULL ) {
			if (strcmp(nodeName, "id") == 0) {
				task.sequence = atoi(nodeValue);
				continue;
			}
			if (strcmp(nodeName, "modelId") == 0) {
				task.modeId = atoi(nodeValue);
				continue;
			}
			if (strcmp(nodeName, "during") == 0) {
				task.during = atof(nodeValue);
				continue;
			}

		} else {
			break;
		}
	}
	//fprintf(stderr,"scheduleID==%d,task.during==%d \n", scheduleID, task.during);

	insertTask(&task, scheduleID);

	return TRUE;

}

void actionDeleteSchedule(char* responseMsg, int scheduleID) {
	removeDBSchedule(scheduleID);
	makeReturnMsg(responseMsg, scheduleID, "delete schedule successes.");
}

void getStatusName(int statusId, char* name) {
	switch (statusId) { //0:等待 1:运行 2:暂停 3:结束
	case 0:
		strcpy(name, "Wait");
		break;
	case 1:
		strcpy(name, "Running");
		break;
	case 2:
		strcpy(name, "Pause");
		break;
	case 3:
		strcpy(name, "End");
		break;
	case 4:
		strcpy(name, "*Error*");
		break;
	}
}
