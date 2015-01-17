/*
 * cmdTransThread.c
 *
 *  Created on: 2014-10-26
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
#include "heartBeatThread.h"
#include "../shared/socket.h"
#include "../shared/sharemem.h"

#include "../shared/lightingpack.h"
#include "cmdTransThread.h"

#include "../shared/serialport.h"

void startHeartBeatServer() {

	//pthread_create(&t_Schedule, NULL, schedule, NULL); //计划任务线程
	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		sleep(15);
		heartBeat();

	}

	fprintf(stderr,
			"=============startHeartBeatServer server Ready ===============\n");

}

void* heartBeat() {
	SHAREMEM* shr_mem = get_shm_Memory(FALSE);
	time_t timeStamp_start; //保存开始/结束时间
	time(&timeStamp_start);
	shr_mem->lastHeartBeat = timeStamp_start;
	release_shm_Memory(shr_mem, FALSE);

	while (TRUE) {
		fprintf(stderr, "=============HeartBeat===============\n");

//check fixture and keep dimmer
		checkDimmer();

		//checkConnetion();
		sendGroupDimmer();

		energeCost();

		sleep(600);
	}
	return NULL ;
}
void checkConnetion() {

	fprintf(stderr, "************checkConnetion**************\n");
	SHAREMEM* shr_mem = get_shm_Memory(FALSE);
	time_t timeStamp_now; //保存开始/结束时间
	time(&timeStamp_now);
	fprintf(stderr, "timeStamp_now - shr_mem->lastHeartBeat==%ld\n",
			timeStamp_now - shr_mem->lastHeartBeat);
	if (timeStamp_now - shr_mem->lastHeartBeat > CONNECTIONTIMEOUT) {
		char response[256];
		BYTE writeBuff[1024];

//send close
//		sprintf(response, "<ROOT><Type>CLOSE</Type></ROOT>");
//		strcpy((char *) writeBuff, (const char *) response);
//		sendSocketData(writeBuff, strlen((const char *) writeBuff), 1);
//		sleep(5);
		closeSocket();

//open
		sleep(5);
		openSocket();
		sleep(3);

		//send  registion

		sprintf(response,
				"<ROOT><Type>Registion</Type><GateUID>%s</GateUID><FrameType>001</FrameType></ROOT>",
				shr_mem->uuid);

		strcpy((char *) writeBuff, (const char *) response);
		sendSocketData(writeBuff, strlen((const char *) writeBuff), 1);

		sleep(3);
		startTransServer();
	}
	release_shm_Memory(shr_mem, FALSE);
}
void checkDimmer() {
	SHAREMEM* shr_mem = get_shm_Memory(FALSE);
	GROUPAUTODIMMER* p_groupDimmer = shr_mem->p_groupDimmer;
	int i = 0;

	SERVERCOMMAND cmd;

	for (i = 0; i < shr_mem->groupPlaceholder; i++) {

		fprintf(stderr,
				" groupAutoDimmer->groupid=%d,,groupAutoDimmer->isSensored==%d ;;;groupAutoDimmer->sensorType ==%d\n",
				p_groupDimmer->groupID, p_groupDimmer->isSensored,
				p_groupDimmer->sensorType);
//		fprintf(stderr,
//						" groupAutoDimmer->roomSensorMAC==%s;;;groupAutoDimmer->daynightSensorMAC ==%s\n",
//						p_groupDimmer->roomSensorMAC, p_groupDimmer->daynightSensorMAC);
		if (p_groupDimmer->isSensored == 1) {
			if (p_groupDimmer->sensorType == 1) {
				strcpy(cmd.MAC, p_groupDimmer->roomSensorMAC);
			} else {
				strcpy(cmd.MAC, p_groupDimmer->daynightSensorMAC);
			}

			fprintf(stderr, " cmd.MAC==%s \n", cmd.MAC);

			cmd.groupID = p_groupDimmer->groupID;

			ctrlDimmer(p_groupDimmer, &cmd);

			COMMLOG log;
			time_t timeStamp_now; //保存开始/结束时间
			time(&timeStamp_now);

			log.startTime = timeStamp_now;
			log.endTime = timeStamp_now;
			//	strcpy(log.groupName, "");
			log.dimmer = p_groupDimmer->currentDimmer;

			log.groupId = p_groupDimmer->groupID;

			writeDBLog(&log);

		}
		p_groupDimmer++;
	}
	release_shm_Memory(shr_mem, FALSE);
}

void sendGroupDimmer() {
	fprintf(stderr, "************sendGroupDimmer**************\n");
	BYTE writeBuff[1024];
	SHAREMEM* shr_mem = get_shm_Memory(FALSE);
	GROUPAUTODIMMER* p_groupDimmer = shr_mem->p_groupDimmer;

	char response[2048], temp[512];
	sprintf(response, "<ROOT><Type>TCP_OK</Type><GateUID>%s</GateUID><Groups>",
			shr_mem->uuid);
	int i = 0;
	for (i = 0; i < shr_mem->groupPlaceholder; i++) {
		int roomSensor = 0, daylightSensor = 0;
		if (p_groupDimmer->isSensored == 1) {
			if (p_groupDimmer->sensorType == 1) {
				roomSensor = 1;
			} else {
				daylightSensor = 1;
			}
		}
		sprintf(temp,
				"<Group><GroupID>%d</GroupID><OnOff>%d</OnOff><Brightness>%d</Brightness><RoomSensor>%d</RoomSensor><DaylightSensor>%d</DaylightSensor></Group>",
				p_groupDimmer->groupID,
				p_groupDimmer->currentDimmer == 0 ? 0 : 1,
				p_groupDimmer->currentDimmer, roomSensor, daylightSensor);

		strcat(response, temp);
		p_groupDimmer++;
	}

	strcat(response, "</Groups></ROOT>");

	strcpy((char *) writeBuff, (const char *) response);
	socketWriterOnly(writeBuff, strlen((const char *) writeBuff));

	release_shm_Memory(shr_mem, FALSE);
}
void ctrlDimmer(GROUPAUTODIMMER* p_groupDimmer, SERVERCOMMAND *cmd) {
	BYTE receiveBuff[BUFFERSIZE];
	BYTE command[64];
//int subDimmer = 0;
	int iCount = genSensorQueryFrame(cmd, command);
	iCount = serialSingleData((BYTE*) command, iCount, receiveBuff);
	if (iCount == 43) { //length of response data
		//if (iCount == 0) {
		//32,33
		//printHEX(receiveBuff, iCount);

		printHEX(&receiveBuff[37], 2);

		if (receiveBuff[32] == 0x94 && receiveBuff[33] == 0x2A) {
			//37,38,39
			int sensorDimmer = ((receiveBuff[38] << 8) & 0xFF00)
					+ (receiveBuff[37] & 0x00FF);

			//int SENSORTOPLEVEL
			int dimmerPresent = 0;

			getSensorLevel(p_groupDimmer->sensorType, sensorDimmer,
					&dimmerPresent);

			cmd->dimmer = dimmerPresent;
			if (dimmerPresent == 0) {
				cmd->onOff = 0;
			} else {
				cmd->onOff = 1;
			}

			fprintf(stderr, "Group %d sensorDimmer = %d,set dimmer to %d.\n",
					p_groupDimmer->groupID, sensorDimmer, cmd->dimmer);

			int iLength = genSingleGroupControlFrame(cmd, command);
			sendSingleData(command, iLength);
			//LOG:
			COMMLOG log;
			time_t timeNow;
			time(&timeNow);
			log.dimmer = cmd->dimmer;
			log.groupId = cmd->groupID;
			log.optionType = cmd->onOff;
			log.endTime = timeNow;
			log.startTime = timeNow;
			//strcpy(log.groupName, "");		//not used

			writeDBLog(&log);

			p_groupDimmer->currentDimmer = dimmerPresent;

		}
	} else {
		return;
	}
//sleep(1);
//ctrlDimmer(p_groupDimmer,cmd);

}

void energeCost() {

	fprintf(stderr, "************sendenergeCost**************\n");

	BYTE writeBuff[1024];
	char response[4096], temp[128], today[32];
	time_t timeStamp_Now, time_Start;

	SHAREMEM* shr_mem = get_shm_Memory(FALSE);

	time(&timeStamp_Now);

	if (timeStamp_Now-shr_mem->lastEnergeCostTime < ONEDAYSECOND) {
		return;
	}


	struct tm timeStart;
	struct tm *timenow;    //实例化tm结构指针
	timenow = localtime(&timeStamp_Now);
	memcpy(&timeStart, timenow, sizeof(struct tm));
	timeStart.tm_hour = 0;
	timeStart.tm_min = 0;

	sprintf(today, "%d/%d/%d", timeStart.tm_year + 1900, timeStart.tm_mon + 1,
			timeStart.tm_mday);

	time_Start = mktime(&timeStart);

	int iCount = getResultCount("GROUPS", NULL );
	ENERGECOST energeCost[iCount];
	bzero(&energeCost, sizeof(ENERGECOST) * iCount);
	getEnergeCost(energeCost, time_Start, timeStamp_Now);

	sprintf(response,
			"<ROOT><Type>Synchronization</Type><FrameType>EnergyCost</FrameType><GateUID>%s</GateUID><Date>%s</Date><Groups>",
			shr_mem->uuid, today);

	release_shm_Memory(shr_mem, FALSE);
	int i = 0;
	for (i = 0; i < iCount; i++) {
		sprintf(temp,
				"<Group><GroupID>%d</GroupID><EnergyCost>%ld</EnergyCost></Group>",
				energeCost[i].groupID, energeCost[i].costValue);

		strcat(response, temp);

	}

	strcat(response, "</Groups></ROOT>");

	strcpy((char *) writeBuff, (const char *) response);
	socketWriterOnly(writeBuff, strlen((const char *) writeBuff));

}
