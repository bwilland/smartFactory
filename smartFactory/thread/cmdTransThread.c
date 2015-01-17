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
#include "cmdTransThread.h"
#include "../shared/socket.h"
#include "../shared/serialport.h"

#include "../shared/lightingpack.h"
#include "../actions/autosearch.h"
#include "../actions/node.h"
#include "scheduleThread.h"
#include "../shared/sharemem.h"
#include "heartBeatThread.h"

void startTransServer() {

	//pthread_create(&t_Schedule, NULL, schedule, NULL); //计划任务线程
	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		sleep(3);
		cmdTrans();

	}

	fprintf(stderr,
			"=============startTransServer server Ready ===============\n");
	fprintf(stderr,
			"=============startTransServer server Ready ===============\n");
}

void* cmdTrans() {
	BYTE readBuff[1024], writeBuff[1024];
	//SHAREMEM* shr_mem;
	int length = 0;
	while (TRUE) {
		memset(readBuff, 0, 1024);
		if ((length = readSocketData(readBuff, 1024)) > 0) {
			//sprintf(stderr, (const char*)readBuff);

			fprintf(stderr, "readBuff==%s\n", readBuff);

			//	fprintf(stderr, "Here\n\n");
			xmlDocPtr pdoc = xmlParseMemory((const char *) readBuff, length);
			//fprintf(stderr, "Here\n\n");
			if (cmdHandler(pdoc, writeBuff) == 1) {
				fprintf(stderr, "writeBuff==%s\n", writeBuff);
				sendSocketData(writeBuff, strlen((const char *) writeBuff), 1);
			}
			xmlFreeDoc(pdoc);
		}
		//shr_mem = get_shm_Memory(TRUE); //必须在这里

	}
	return NULL ;
}

/**
 * return 1:send back to server     0:no response
 */
int cmdHandler(xmlDocPtr pdoc, BYTE *writeBuff) {
	int iLength, iResult;
	BYTE command[64];
	xmlChar *xmlbuff;
	int buffersize;

	int iType = parseXMLType(pdoc);

	fprintf(stderr, "iType==%d\n", iType);

	switch (iType) {
	case ASK_REGISTION: {
		char response[256];
		SHAREMEM* shr_mem = get_shm_Memory(FALSE);

		//fprintf(stderr,"shr_mem==%p \n",shr_mem);
		sprintf(response,
				"<ROOT><Type>REGISTION</Type><GateUID>%s</GateUID><FrameType>001</FrameType></ROOT>",
				shr_mem->uuid);
		release_shm_Memory(shr_mem, FALSE);

//fprintf(stderr,"response==%s \n",response);

		strcpy((char *) writeBuff, (const char *) response);

		break;
	}
	case TCP_OK: {
		SHAREMEM* shr_mem = get_shm_Memory(FALSE);
		time_t timeStamp_now; //保存开始/结束时间
		time(&timeStamp_now);

		shr_mem->lastHeartBeat = timeStamp_now;

		release_shm_Memory(shr_mem, FALSE);
		return 0;
		break;
	}

	case CTRL_COMMAND: {
		SERVERCOMMAND cmdInfo;
		bzero(&cmdInfo, sizeof(SERVERCOMMAND));
		parseCommadXML(pdoc, &cmdInfo);

		controlCMD(pdoc, &cmdInfo);

		xmlIndentTreeOutput = 0;
		//xmlKeepBlanksDefault(1);
		xmlDocDumpFormatMemory(pdoc, &xmlbuff, &buffersize, 1);
		//fprintf(stderr,"%s \n",xmlbuff);

		//removeServerNode((char *) xmlbuff);
		strcpy((char *) writeBuff, (const char *) xmlbuff);
		free(xmlbuff);
		break;
	}
	case REGISTION: {
		REGISTIONINFO registion;
		bzero(&registion, sizeof(REGISTIONINFO));
		parseRegistionXML(pdoc, &registion);

		if (registion.frameType == 2) {
			//registed OK
			fprintf(stderr, "Regist OK!\n");
		} else {
			char response[256];
			SHAREMEM* shr_mem = get_shm_Memory(FALSE);
			sprintf(response,
					"<ROOT><Type>REGISTION</Type><GateUID>%s</GateUID><FrameType>001</FrameType></ROOT>",
					shr_mem->uuid);
			release_shm_Memory(shr_mem, FALSE);
			strcpy((char *) writeBuff, (const char *) response);
		}
		return 0;
		break;
	}
	case SYNCHRONIZATION: {
		int frameType = 0;
		parseSynchronizationXML(pdoc, &frameType);
		switch (frameType) {
		case SYNCHRONIZATION_AUTOSEARCH: {
			if (autoSearch(0) == 1) {
				actionAllNodeList(pdoc);

			} else {
				addErrorMsg(pdoc, 400);
			}
			xmlDocDumpFormatMemory(pdoc, &xmlbuff, &buffersize, 0);
			strcpy((char *) writeBuff, (const char *) xmlbuff);
			break;
		}//SYNCHRONIZATION_AUTOSEARCH

		}//switch

		break;
	}
	case SETTING_COMMAND: {
		GroupSettingCommand settingInfo;
		bzero(&settingInfo, sizeof(GroupSettingCommand));
		parseSettingXML(pdoc, &settingInfo);
		if (settingInfo.frameType == GROUPSETTING) {
			//group control
			//char devices[1024];

			iLength = genGroupSetFrame(&settingInfo, command);
			iResult = sendSingleData(command, iLength);
			if (iResult == 0) {
				//Error: Time over
				addErrorMsg(pdoc, 400);
			}
		}
		//
		xmlDocDumpFormatMemory(pdoc, &xmlbuff, &buffersize, 0);
		//removeServerNode((char *) xmlbuff);
		strcpy((char *) writeBuff, (const char *) xmlbuff);
		break;
	}
	case PROGRAM_SETTING: {
		PROGRAM program;
		bzero(&program, sizeof(PROGRAM));
		//fprintf(stderr, "program \n");
		if (parseProgramXML(pdoc, &program) == 1) {
			fprintf(stderr, "program frameType==%d\n", program.frameType);
			program.isLoop = 1; //default
			switch (program.frameType) {
			case 1:
			case 2:
				//save & not start
				program.status = TASKPAUSED;
				saveProgram(&program);
				addProgram(&program);
				break;
			case 3:
				deleteProgram(&program);
				removeProgram(&program);
				break;
			case 4:
				//start program;
				startProgram(&program);
				break;
			case 5:
				stopProgram(&program);
				break;
			}

		} else {
			addErrorMsg(pdoc, 400);
		}

		//removeServerNode((char *) xmlbuff);

		xmlDocDumpFormatMemory(pdoc, &xmlbuff, &buffersize, 0);

		strcpy((char *) writeBuff, (const char *) xmlbuff);

		break;
	}
	}
	return 1;
}

void removeProgram(PROGRAM *program) {

	SHAREMEM* shr_mem = get_shm_Memory(TRUE);
	int i = 0;

	PROGRAM *p_program = shr_mem->p_Program;
	for (i = 0; i < shr_mem->programPlaceholder; i++) {
		if (p_program->id == program->id) {
			memcpy(program, p_program, sizeof(PROGRAM));
			p_program->id = -1;

			break;
		}
		p_program++;
	}

//stop program;
	executeSchedule(program, shr_mem, TRUE);

	release_shm_Memory(shr_mem, TRUE);

}

void addProgram(PROGRAM *program) {
	time_t timeNow;
	time(&timeNow);
	//setProgramTime(program, timeNow);

	SHAREMEM* shr_mem = get_shm_Memory(TRUE);
	int i = 0;

	int isFound = 0;
	PROGRAM *p_program = shr_mem->p_Program;
	for (i = 0; i < shr_mem->programPlaceholder; i++) {
		if (p_program->id == program->id) {
			memcpy(p_program, program, sizeof(PROGRAM));
			isFound = 1;
			break;
		}
		p_program++;
	}

	if (isFound == 0) {
		p_program = shr_mem->p_Program;
		for (i = 0; i < shr_mem->programPlaceholder; i++) {
			if (p_program->id == -1) {
				memcpy(p_program, program, sizeof(PROGRAM));
				isFound = 1;
				break;
			}
			p_program++;
		}
	}
	if (isFound == 0) {
		memcpy(p_program, program, sizeof(PROGRAM));
		shr_mem->programPlaceholder++;
	}
	/*
	 fprintf(stderr, "timeNow==%ld --->---- program->nextStartTime ==%ld \n",
	 timeNow, program->nextStartTime);
	 if (program->nextStartTime < timeNow) {
	 executeSchedule(program, shr_mem, FALSE);
	 }
	 */
	release_shm_Memory(shr_mem, TRUE);

}

void startProgram(PROGRAM *program) {
	fprintf(stderr, " *****startProgram*****\n");

	time_t timeNow;
	time(&timeNow);

	SHAREMEM* shr_mem = get_shm_Memory(FALSE);
	//fprintf(stderr, " shr_mem  %p\n", shr_mem);
	int i = 0;

	int isFound = 0;
	PROGRAM *p_program = shr_mem->p_Program;
	for (i = 0; i < shr_mem->programPlaceholder; i++) {
		if (p_program->id == program->id) {
			//get program from sharemem
			//memcpy(program, p_program, sizeof(PROGRAM));
			isFound = 1;
			break;
		}
		p_program++;
	}
	//fprintf(stderr, " p_program  %d  isFound==%d\n", p_program->id, isFound);
	if (isFound == 1) {
		setProgramTime(p_program, timeNow);

		fprintf(stderr, "timeNow==%ld --->---- program->nextStartTime ==%ld \n",
				timeNow, p_program->nextStartTime);
		if (program->nextStartTime < timeNow) {
			executeSchedule(p_program, shr_mem, FALSE);
			p_program->status = TASKRUNNING;
		} else {
			p_program->status = TASKWAIT;
		}
	}

	release_shm_Memory(shr_mem, FALSE);
	fprintf(stderr, " *****startProgram end*****\n");
}

void stopProgram(PROGRAM *program) {
	time_t timeNow;
	time(&timeNow);

	SHAREMEM* shr_mem = get_shm_Memory(FALSE);
	int i = 0;

	int isFound = 0;
	PROGRAM *p_program = shr_mem->p_Program;
	for (i = 0; i < shr_mem->programPlaceholder; i++) {
		if (p_program->id == program->id) {
			//get program from sharemem
			//memcpy(program, p_program, sizeof(PROGRAM));
			isFound = 1;
			break;
		}
		p_program++;
	}

	if (isFound == 1) {

		executeSchedule(p_program, shr_mem, TRUE);
		p_program->status = TASKPAUSED;
	}

	release_shm_Memory(shr_mem, FALSE);

}

void setProgramTime(PROGRAM *program, time_t time2Program) {

	struct tm *p_programTime;
	p_programTime = gmtime(&time2Program);
	char weekDay[2];

	int hasday = 0;
	char *p;
	fprintf(stderr, "program->week==%s\n", program->week);
	int i = 0;

	while (!hasday || i == 6) {
		p = program->week;
		sprintf(weekDay, "%d", p_programTime->tm_wday);

		fprintf(stderr, "weekDay==%s\n", weekDay);

		while (*p != '\0') {
			if (*p == weekDay[0]) {
				hasday = 1;
				break;
			}
			p++;
		}

		if (hasday == 0) {
			//add 1day
			time2Program += 24 * 60 * 60;
			p_programTime = gmtime(&time2Program);
		}
		i++;
	}

	program->nextStartTime = setTimeStruct(time2Program, program->startTime);
	program->nextEndTime = setTimeStruct(time2Program, program->endTime);

	time_t timeNow;
	time(&timeNow);

	fprintf(stderr,
			"timeNow==%ld;;;;program->nextstartTime=%ld;;;;program->nextEndTime=%ld\n",
			timeNow, program->nextStartTime, program->nextEndTime);
	if (program->nextEndTime < timeNow) {
		time2Program += 24 * 60 * 60;
		setProgramTime(program, time2Program);
	}

}
time_t setTimeStruct(const time_t t_time, char *ptime) {
	char time[64], *p, *token;
	strcpy(time, ptime);
	p = time;
	int iTime[2];
	int i = 0;
	while (p != NULL ) {
		token = strsep(&p, ":");
		if (token != NULL ) {
			iTime[i] = atoi(token);
			i++;
		} else {
			break;
		}
	}
	struct tm *tm_org = gmtime(&t_time);
	struct tm tm_time;
	memcpy(&tm_time, tm_org, sizeof(struct tm));

	tm_time.tm_hour = iTime[0];
	tm_time.tm_min = iTime[1];
	tm_time.tm_sec = 0;
	//fprintf(stderr, "tm_time ==%ld\n", mktime(&tm_time));

	return mktime(&tm_time);

}
void removeServerNode(char *sSrc) {

	int StringLen;
	char caNewString[2048];
	char *sMatchStr = "Server";
	char *sReplaceStr = "Client";
	char *FindPos = strstr(sSrc, sMatchStr);
	if ((!FindPos) || (!sMatchStr))
		return;

	while (FindPos) {
		memset(caNewString, 0, sizeof(caNewString));
		StringLen = FindPos - sSrc;
		strncpy(caNewString, sSrc, StringLen);
		strcat(caNewString, sReplaceStr);
		strcat(caNewString, FindPos + strlen(sMatchStr));
		strcpy(sSrc, caNewString);

		FindPos = strstr(sSrc, sMatchStr);
	}

	return;

}

