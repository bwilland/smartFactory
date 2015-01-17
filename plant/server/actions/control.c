/*
 * control.c
 *
 *  Created on: 2013年10月6日
 *      Author: root
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "control.h"
#include "node.h"
#include "area.h"
#include "../shared/serialport.h"
#include "../shared/sharemem.h"
#include "../shared/lightingpack.h"
#include "../thread/scheduleThread.h"
#include <unistd.h>

char* token;
char* actionPara;

void controlManage(char* request, char* responseMsg) {

	char* p_Paras;
	p_Paras = request;
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

	for (i = 0; i < iCount; i++) {
		if (strstr(paras[i], "METHOD") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			fprintf(stderr, "METHOD==%s\n", actionPara);
			break;
		}
	}
	if (strcmp(actionPara, "Manufacture") == 0) {
		char* areaIds = NULL;
		int modelId = 0; //0:观察模式
		int option = 0; //0:关，1：开；

		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAID") != NULL ) {
				areaIds = paras[i];
				token = strsep(&areaIds, "=");
				continue;
			}
			if (strstr(paras[i], "MODELID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				modelId = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "OPTION") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				option = atoi(actionPara);
				continue;
			}
		}

		if (areaIds[strlen(areaIds) - 1] == ';') {
			areaIds[strlen(areaIds) - 1] = '\0';
		}
		//fprintf(stderr,"%s  %d  %d \n", areaIds, modelId, option);

		actionManufacture(responseMsg, areaIds, modelId, option, FALSE);

		return;
	}
	if (strcmp(actionPara, "Reproduction") == 0) {
		int option = 1; //0:关，1：开；

		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "OPTION") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				option = atoi(actionPara);
				break;
			}
		}
		if (option == 1) {
			actionReproduction(responseMsg);
		} else {
			actionStopReproduction(responseMsg);
		}
		return;
	}

}

//演示
void actionReproduction(char* responseMsg) {
	int i = 0;
	SHAREMEM *share_mem = get_shm_Memory(TRUE);
	SCHEDULE *p_schedule = share_mem->p_Schedule;
	for (i = 0; i < share_mem->schedulePlaceholder; i++) {
		if (p_schedule->id == REPRODUCTIONID) {
			makeReturnMsg(responseMsg, 1, "Demo is started already.");
			release_shm_Memory(share_mem, TRUE);
			return;
			break;
		}
		p_schedule++;
	}

	time_t timeNow;
	time(&timeNow);
	//removeDBSchedule(REPRODUCTIONID);

	SCHEDULE schedule;
	bzero(&schedule, sizeof(SCHEDULE));
	schedule.id = REPRODUCTIONID;
	schedule.taskType = 1;
	schedule.startTime = timeNow;
	schedule.endTime = 0; //不停止
	schedule.currentSequence = 0;
	schedule.status = TASKRUNNING;
	schedule.taskTimeLeft = 0;
	schedule.timeleft = 65535;
	//schedule.maxModelSequence = 13;//TODO

	char tmp[32];
	sprintf(tmp, "SCHEDULEID=%d", REPRODUCTIONID);
	schedule.maxModelSequence = getResultCount("TASK", tmp);

	char areaIds[256];
	bzero(areaIds, 256);
	getAllAreaIDs(areaIds);
	areaIds[strlen(areaIds) - 1] = '\0';
	strcpy(schedule.areaList, areaIds);
	//strcpy(schedule.areaList, "0");
	//tasks
	int modelCount = getResultCount("STATICMODEL", NULL );

	//delete old reproduction task;
	deleteAllReproductionTask();

	TASK task;
	bzero(&task, sizeof(TASK));
	for (i = 1; i <= modelCount; i++) {
		task.modeId = i;
		task.sequence = i;
		task.during = 1;
		insertTask(&task, REPRODUCTIONID);
	}

	p_schedule = share_mem->p_Schedule;
	BOOL isUpdated = FALSE; //是否插入空闲处
	for (i = 0; i < share_mem->schedulePlaceholder; i++) {
		if (p_schedule->id == -1) {
			isUpdated = TRUE;
			break;
		}
		p_schedule++;
	}
	memcpy(p_schedule, &schedule, sizeof(SCHEDULE));
	if (!isUpdated) {
		share_mem->schedulePlaceholder++;
	}
	int modelId = 0;
	executeSchedule(p_schedule, share_mem, timeNow, &modelId);

	release_shm_Memory(share_mem, TRUE);

	makeReturnMsg(responseMsg, 1, "Open Demo OK.");
}
void actionStopReproduction(char* responseMsg) {
	SHAREMEM* share_mem = get_shm_Memory(TRUE);
	SCHEDULE *p_schedule = share_mem->p_Schedule;
	int i = 0;
	time_t timeNow;
	for (i = 0; i < share_mem->schedulePlaceholder; i++) {
		fprintf(stderr, "p_schedule->id==%d \n", p_schedule->id);
		if (p_schedule->id == REPRODUCTIONID) {
			time(&timeNow);
			//关闭任务
			closeSchedule(p_schedule, share_mem, timeNow);

			p_schedule->id = -1;
			//share_mem->schedulePlaceholder--;
			break;
		}
		p_schedule++;
	}
	release_shm_Memory(share_mem, TRUE);
	//removeDBSchedule(REPRODUCTIONID);
	makeReturnMsg(responseMsg, 1, "Close Demo OK.");
}

//------------------手工通讯--------------------------
void actionManufacture(char* responseMsg, char* areaIds, int modelId,
		int option, BOOL openLocked) {
	int i = 0;
	int areaId;
	strcpy(responseMsg, "<root>");
	SHAREMEM* share_mem = get_shm_Memory(TRUE);
	AREAINFO areaInfo;
	BOOL isFound;

	if (areaIds[strlen(areaIds) - 1] == ',') {
		areaIds[strlen(areaIds) - 1] = '\0';
	}

	fprintf(stderr, "actionManufacture areaIds==%s \n", areaIds);

	while (areaIds != NULL ) {
		token = strsep(&areaIds, ",");
		areaId = atoi(token);

		fprintf(stderr, "areaId==%d \n", areaId);
		isFound = FALSE;

		bzero(&areaInfo, sizeof(AREAINFO));
		AREAINFO* p_area = share_mem->p_Area;
		for (i = 0; i < share_mem->areaPlaceholder; i++) {
			//fprintf(stderr, "p_area--areaId==%s \n", p_area->name);
			if (p_area->id == areaId) {
				isFound = TRUE;
				//保留原始数据
				areaInfo.id = areaId;
				strcpy(areaInfo.name, p_area->name);
				strcpy(areaInfo.memo, p_area->memo);
				//areaInfo.Kr = p_area->Kr;
				//areaInfo.Kb = p_area->Kb;
				areaInfo.optionType = p_area->optionType;
				areaInfo.isLocked = p_area->isLocked;

				break;
			}
			p_area++;
		}

		if (!isFound) {
			fprintf(stderr, "area：%s not found!\n", areaInfo.name);
			continue;
		}
		if (p_area->isLocked && !openLocked) {
			//正在进行定时通信的区域未结束时禁用手工通信
			makeControlReturn(responseMsg, -1, &areaInfo);
			continue;
		}
		BOOL isSuccess = FALSE;
		switch (option) { //0:关闭 1:普通模式 2:观察模式
		case 0:
			isSuccess = sendModel2Area(areaId, 1, 0, FALSE, &areaInfo,
					share_mem->cmdSendTimes, share_mem->isBroadcast);
			break;
		case 1:
			isSuccess = sendModel2Area(areaId, modelId, 1, FALSE, &areaInfo,
					share_mem->cmdSendTimes, share_mem->isBroadcast);
			break;
		case 2:
			isSuccess = sendModel2Area(areaId, 2, 1, FALSE, &areaInfo,
					share_mem->cmdSendTimes, share_mem->isBroadcast);
			break;
		}
		if (isSuccess) {
			//更新内存数据
			memcpy(p_area, &areaInfo, sizeof(AREAINFO));
			makeControlReturn(responseMsg, 1, &areaInfo);
		} else {
			makeControlReturn(responseMsg, 0, &areaInfo);
		}

	}
	release_shm_Memory(share_mem, TRUE);
	strcat(responseMsg, "</root>");

}

BOOL sendModel2Area(int areaId, int modelId, int onoffST, BOOL isStatic,
		AREAINFO* p_area, int resendTimes, int isBroadcast) {
	char tmp[64];
	int i = 0, iCount = 0, iCMDLength = 0;

	//BYTE readBuffer[BUFFERSIZE];

	BOOL isSuccess = TRUE;
	COMMANDINFO cInfo;
	bzero(&cInfo, sizeof(COMMANDINFO));

	MODE model;
	if (isStatic) { //演示从预置的固定模式表取数据
		iCount = getStaticModelInfo(&model, modelId);
	} else {
		iCount = getDBModelInfo(&model, modelId);
	}

	fprintf(stderr, "modelId==%d  \n", modelId);

	if (iCount != 1) {
		fprintf(stderr, "modelId==%d not exist \n", modelId);
		return FALSE;
	}
	//LED
	cInfo.PPF1 = model.ppf1;
	cInfo.PPF2 = model.ppf2;
	cInfo.PPF3 = model.ppf3;
	cInfo.PPF4 = model.ppf4;

	//motor
	cInfo.mode = model.mode;
	cInfo.direction = model.direction;
	cInfo.distance = model.distance;
	cInfo.speed = model.speed;

	//保存区域模式数据
	p_area->modeId = modelId;
	p_area->ppf1 = model.ppf1;
	p_area->ppf2 = model.ppf2;
	p_area->ppf3 = model.ppf3;
	p_area->ppf4 = model.ppf4;

	strcpy(p_area->modeName, model.name);

	switch (onoffST) {
	case 0:
		cInfo.onoffSt = 0x00;
		break;
	case 1:
		cInfo.onoffSt = 0x0F;
		break;
	}
	//保存区域模式数据
	p_area->onoffST = cInfo.onoffSt;

	/*if (isBroadcast) {
	 strcpy(cInfo.mac, "FFFFFFFF");
	 BYTE command[128];
	 cInfo.isBroadCast = 1;

	 iCMDLength=genLEDControlFrame(&cInfo, command);
	 int iSend = 0;
	 for (iSend = 0; iSend < resendTimes; iSend++) {
	 //printHEX(command, CTRL_CMD_LEN);
	 sendData(command, iCMDLength, 1);
	 }

	 iCMDLength=genMotoSetControl(&cInfo, command);
	 for (iSend = 0; iSend < resendTimes; iSend++) {
	 //printHEX(command, CTRL_CMD_LEN);
	 sendData(command, iCMDLength, 1);
	 }

	 } else {
	 */
	sprintf(tmp, "AREA=%d AND ISVALID=1", areaId);
	int nodeCount = getResultCount("NODES", tmp);

	if (nodeCount > 0) {
		NODE nodes[nodeCount];
		BYTE command[128];
		//BYTE cmdBuff[CTRL_CMD_LEN * nodeCount];
		//BYTE *p_cmd = cmdBuff;

		getNodeByAreaId(nodes, areaId);

		sprintf(cInfo.groupIDs, "%d,", areaId);

		//CLOSE ALL
		COMMANDINFO closeAllInfo;
		bzero(&closeAllInfo, sizeof(COMMANDINFO));
		sprintf(closeAllInfo.groupIDs, "%d,", areaId);

		for (i = 0; i < nodeCount; i++) {

			fprintf(stderr, "nodes[i].type==%d  \n", nodes[i].type);

			//close all;
			strcpy(closeAllInfo.mac, nodes[i].mac);

			if (nodes[i].type == 0) {		//LED
				iCMDLength = genLEDControlFrame(&closeAllInfo, command);
				sendData(command, iCMDLength, 1);

			} else {		//MOTOR
				iCMDLength = genMotoSetControl(&closeAllInfo, command);
				sendData(command, iCMDLength, 1);

			}
			//memcpy(p_cmd, command, CTRL_CMD_LEN);
			//p_cmd += CTRL_CMD_LEN;
		}

		for (i = 0; i < nodeCount; i++) {
			strcpy(cInfo.mac, nodes[i].mac);
			fprintf(stderr, "nodes[i].type==%d  \n", nodes[i].type);

			if (nodes[i].type == 0) {

				//LED
				iCMDLength = genLEDControlFrame(&cInfo, command);
				sendData(command, iCMDLength, 1);
			} else {

				//MOTOR
				iCMDLength = genMotoSetControl(&cInfo, command);
				sendData(command, iCMDLength, 1);
			}
			//memcpy(p_cmd, command, CTRL_CMD_LEN);
			//p_cmd += CTRL_CMD_LEN;
		}

//			int iSend = 0;
//			for (iSend = 0; iSend < resendTimes; iSend++) {
		//printHEX(cmdBuff, CTRL_CMD_LEN * nodeCount);
//				serialData(cmdBuff, CTRL_CMD_LEN, nodeCount,readBuffer);
//			}

	} else {
		fprintf(stderr, "Error: areaId==%d without any nodes. \n", areaId);
		isSuccess = FALSE;
	}
	//}
	if (isSuccess) {
		writeLog(p_area);
	}
	return isSuccess;
}

void writeLog(AREAINFO* areaInfo) {
	COMMLOG log;
	time_t timeNow;
	time(&timeNow);
	log.dateTime = timeNow;
	log.areaId = areaInfo->id;
	strcpy(log.areaName, areaInfo->name);
	log.modeId = areaInfo->modeId;
	strcpy(log.modeName, areaInfo->modeName);
	log.PPF1 = areaInfo->ppf1;
	log.PPF2 = areaInfo->ppf2;
	log.PPF3 = areaInfo->ppf3;
	log.PPF4 = areaInfo->ppf4;
	log.optionType = areaInfo->optionType;
	log.timeStoped = 0;
	writeDBLog(&log);

}

void makeControlReturn(char* responseMsg, int code, AREAINFO* areaInfo) {
	char tmp[256];
	if (code == 1) {
		sprintf(tmp,
				"<return><code>%d</code><message> Command is successfully sent：  Area：[%s]    Mode：[%s] </message></return>",
				code, areaInfo->name, areaInfo->modeName);
	} else if (code == 0) {
		sprintf(tmp,
				"<return><code>%d</code><message> Command failed：  Area：[%s]    Mode：[%s] </message></return>",
				code, areaInfo->name, areaInfo->modeName);
	} else {
		sprintf(tmp,
				"<return><code>%d</code><message> Command is rejected：  area[%s]:in scheduling.  </message></return>",
				code, areaInfo->name);
	}

	strcat(responseMsg, tmp);
}
