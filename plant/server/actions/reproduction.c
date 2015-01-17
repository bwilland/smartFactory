/*
 * reproduction.c
 *
 *  Created on: 2013年9月30日
 *      Author: root
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "reproduction.h"
#include "node.h"
int i;
char* token;
char* actionPara;

void reproductionManage(char* request, char* responseMsg) {
	char* p_Paras;
	int reproductionID = 0;
	p_Paras = request;
	//fprintf(stderr, "p_Paras==%s \n", p_Paras);

	token = strsep(&p_Paras, "?");
	char paras[15][128];
	int i = 0, iCount = 0;

	while (p_Paras != NULL )
	//for (i = 0;i < 5;i++)
	{
		i++;
		token = strsep(&p_Paras, "&");
		if (token != NULL ) {
			strcpy(paras[iCount], token);
//	fprintf(stderr,"%s\n",token);
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

	//get all reproduction action
	if (strcmp(actionPara, "ReproductionList") == 0) {
		actionAllReproductionList(responseMsg);
		return;
	}

	//get reproduction info
	if (strcmp(actionPara, "ReproductionInfo") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "MODELID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				reproductionID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "reproductionID==%d\n", reproductionID);
		actionReproductionInfo(responseMsg, reproductionID);
		return;
	}
	//save reproduction info
	if (strcmp(actionPara, "SaveReproduction") == 0) {
		actionSaveReproduction(responseMsg, paras, iCount);
		return;
	}
	if (strcmp(actionPara, "DeleteReproduction") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "MODELID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				reproductionID = atoi(actionPara);
				break;
			}
		}
		//fprintf(stderr, "reproductionID==%d\n", reproductionID);
		actionDeleteReproduction(responseMsg, reproductionID);
		return;
	}
	if (strcmp(actionPara, "TimeSpan") == 0) {
		actionGetTimeSpan(responseMsg);
		return;
	}

}

void actionGetTimeSpan(char* responseMsg) {

	sprintf(responseMsg, "<root><TimeSpan><span>%d</span></TimeSpan></root>",
			getTimeSpan());

}
void actionDeleteReproduction(char* responseMsg, int reproductionID) {

	deleteTask(REPRODUCTIONID);
	deleteReproduction(reproductionID);
	insertReproductionTask();
	makeReturnMsg(responseMsg, 1, "delete schedule successes.");
}

void insertReproductionTask() {
	int reproductionCount = getResultCount("STATICMODE", NULL );
	if (reproductionCount > 0) {
		MODE reproductions[reproductionCount];
		getAllReproduction(reproductions);
		TASK task;
		task.scheduleId = REPRODUCTIONID;
		int during = getTimeSpan();
		task.during = during;

		for (i = 0; i < reproductionCount; i++) {
			task.modeId = reproductions[i].id;
			task.sequence = i + 1;
			insertTask(&task, REPRODUCTIONID);
		}
	}
}

void actionSaveReproduction(char* responseMsg, char paras[][128], int iCount) {
	MODE reproduction;
	int timeSpan = 0;
	bzero(&reproduction, sizeof(MODE));
	for (i = 0; i < iCount; i++) {
		//fprintf(stderr, "%d: %s \n", i, paras[i]);
		if (strstr(paras[i], "MODELID") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.id = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "MODELNAME") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			strcpy(reproduction.name, actionPara);
			continue;
		}
		if (strstr(paras[i], "PPF1") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.ppf1 = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "PPF2") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.ppf2 = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "PPF3") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.ppf3 = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "PPF4") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.ppf4 = atoi(actionPara);
			continue;
		}

		if (strstr(paras[i], "PPF1") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.ppf1 = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "MODE") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.mode = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "DIRECTION") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.direction = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "SPEED") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.speed = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "LENGTH") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			reproduction.distance = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "TIMESPAN") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			timeSpan = atoi(actionPara);
			continue;
		}

	}

	//fprintf(stderr, "timeSpan==%d \n", timeSpan);

	if (timeSpan != 0) {
		//updateKRKB(&krkb);
		updateTimeSpan(timeSpan);
	}

	int reproductionID = -1;
	if (reproduction.id == -1) {
		reproductionID = insertReproduction(&reproduction);
		deleteTask(REPRODUCTIONID);
		insertReproductionTask();
		reproduction.id = reproductionID;
	} else {
		updateReproduction(&reproduction);
		if (timeSpan != 0) {
			updateReproductionTask(timeSpan);
		}
	}

	makeReturnMsg(responseMsg, reproduction.id, "save reproduction successes.");
	return;
}

void actionReproductionInfo(char* responseMsg, int reproductionID) {
	strcpy(responseMsg, "<root>");

	MODE reproduction;
	getStaticModelInfo(&reproduction, reproductionID);

	makeReproductionInfoResponse(responseMsg, &reproduction, 1);

	strcat(responseMsg, "</root>");

}

void actionAllReproductionList(char* responseMsg) {
	char tmp[256];
	strcpy(responseMsg, "<root>");
	int reproductionCount = getResultCount("STATICMODE", NULL );
	if (reproductionCount > 0) {
		MODE reproductions[reproductionCount];
		getAllReproduction(reproductions);
		for (i = 0; i < reproductionCount; i++) {
			sprintf(tmp,
					"<reproduction><id>%d</id><name>%s</name><type>%d</type></reproduction>",
					reproductions[i].id, reproductions[i].name,
					reproductions[i].type);
			strcat(responseMsg, tmp);

		}
	} else {
		strcat(responseMsg, "No reproduction");
	}

	strcat(responseMsg, "</root>");

}
void makeReproductionInfoResponse(char* response, MODE* pReproductions,
		int reproductionCount) {
	char tmp[256];
	int i = 0;
	for (i = 0; i < reproductionCount; i++) {

		sprintf(tmp,
				"<reproduction><id>%d</id><name>%s</name><type>%d</type><ppf1>%.2f</ppf1><ppf2>%.2f</ppf2><ppf3>%.2f</ppf3><ppf4>%.2f</ppf4><mode>%d</mode><direction>%d</direction><speed>%.2f</speed><distance>%.2f</distance></reproduction>",
				pReproductions->id, pReproductions->name, pReproductions->type, pReproductions->ppf1,
				pReproductions->ppf2, pReproductions->ppf3, pReproductions->ppf4, pReproductions->mode,
				 pReproductions->direction, pReproductions->speed,
				pReproductions->distance);

		strcat(response, tmp);
		if (reproductionCount > 1) {
			pReproductions++;
		}
	}
}

