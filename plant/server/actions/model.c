/*
 * model.c
 *
 *  Created on: 2013年9月30日
 *      Author: root
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include "node.h"
int i;
char* token;
char* actionPara;

void modelManage(char* request, char* responseMsg) {
	char* p_Paras;
	int modelID = 0;
	p_Paras = request;
//	fprintf(stderr,"p_Paras==%s \n", p_Paras);

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

	//get all model action
	if (strcmp(actionPara, "ModelList") == 0) {
		actionAllModelList(responseMsg);
		return;
	}

	//get all model action
	if (strcmp(actionPara, "ModelInfoList") == 0) {
		actionModelInfoList(responseMsg);
		return;
	}

	//get model info
	if (strcmp(actionPara, "ModelInfo") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "MODELID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				modelID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "modelID==%d\n", modelID);
		actionModelInfo(responseMsg, modelID);
		return;
	}
	//save model info
	if (strcmp(actionPara, "SaveModel") == 0) {
		actionSaveModel(responseMsg, paras, iCount);
		return;
	}
	if (strcmp(actionPara, "DeleteModel") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "MODELID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				modelID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "modelID==%d\n", modelID);
		actionDeleteModel(responseMsg, modelID);
		return;
	}
//	if (strcmp(actionPara, "KBKR") == 0) {
//		actionGetKRKB(responseMsg);
//		return;
//	}

}

//void actionGetKRKB(char* responseMsg) {
//	KRKB krkb;
//	getKRKB(&krkb);
//	sprintf(responseMsg, "<root><krkb><kr>%d</kr><kb>%d</kb></krkb></root>",
//			krkb.Kr, krkb.Kb);
//
//}
void actionDeleteModel(char* responseMsg, int modelID) {

	deleteModel(modelID);
	makeReturnMsg(responseMsg, 1, "delete schedule successes.");
}

void actionSaveModel(char* responseMsg, char paras[][128], int iCount) {
	MODE model;
	//int kr = 0, kb = 0;
	bzero(&model, sizeof(MODE));
	for (i = 0; i < iCount; i++) {
		fprintf(stderr, "%d: %s \n", i, paras[i]);
		if (strstr(paras[i], "MODELID") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.id = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "MODELNAME") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			strcpy(model.name, actionPara);
			strReplace(model.name, '^', ' ');
			continue;
		}
		if (strstr(paras[i], "PPF1") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.ppf1 = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "PPF2") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.ppf2 = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "PPF3") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.ppf3 = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "PPF4") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.ppf4 = atof(actionPara);
			continue;
		}

		if (strstr(paras[i], "MODE") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.mode = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "DIRECTION") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.direction = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "SPEED") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.speed = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "LENGTH") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			model.distance = atof(actionPara);
			continue;
		}
	}

	//fprintf(stderr,"RED==%d,blue=%d \n", kr, kb);

	int modelID = -1;
	if (model.id == -1) {
		modelID = insertModel(&model);
		model.id = modelID;
	} else {
		updateModel(&model);
	}
//	if (kr != 0 || kb != 0) {
//		KRKB krkb;
//		krkb.Kr = kr;
//		krkb.Kb = kb;
//		updateKRKB(&krkb);
//	}

	makeReturnMsg(responseMsg, model.id, "save model successes.");
	return;
}

void actionModelInfo(char* responseMsg, int modelID) {
	strcpy(responseMsg, "<root>");

	MODE model;
	getDBModelInfo(&model, modelID);

	makeModelInfoResponse(responseMsg, &model, 1);

	strcat(responseMsg, "</root>");
}
void actionModelInfoList(char* responseMsg) {
	strcpy(responseMsg, "<root>");
	int modelCount = getResultCount("MODE", NULL );
	if (modelCount > 0) {
		MODE models[modelCount];
		getAllModel(models);
		makeModelInfoResponse(responseMsg, models, modelCount);
	} else {
		strcat(responseMsg, "No model");
	}

	strcat(responseMsg, "</root>");
	//fprintf(stderr,"%s",responseMsg);
}

void actionAllModelList(char* responseMsg) {
	char tmp[256];
	strcpy(responseMsg, "<root>");
	int modelCount = getResultCount("MODE", NULL );
	if (modelCount > 0) {
		MODE models[modelCount];
		getAllModel(models);
		for (i = 0; i < modelCount; i++) {
			sprintf(tmp,
					"<model><id>%d</id><name>%s</name><type>%d</type></model>",
					models[i].id, models[i].name, models[i].type);
			strcat(responseMsg, tmp);

		}
	} else {
		strcat(responseMsg, "No model");
	}

	strcat(responseMsg, "</root>");

}
void makeModelInfoResponse(char* response, MODE* pModels, int modelCount) {
	char tmp[512], modeName[32];

	int i = 0;

	for (i = 0; i < modelCount; i++) {
		getModeName(modeName, pModels->mode);
//		fprintf(stderr, "%.2f,%.2f,%.2f,%.2f,", pModels->speed,
//				pModels->distance, pModels->ppf3, pModels->ppf4);
		sprintf(tmp,
				"<model><id>%d</id><name>%s</name><type>%d</type><ppf1>%.2f</ppf1><ppf2>%.2f</ppf2><ppf3>%.2f</ppf3><ppf4>%.2f</ppf4><mode>%d</mode><modeName>%s</modeName><direction>%d</direction><speed>%.2f</speed><distance>%.2f</distance></model>",
				pModels->id, pModels->name, pModels->type, pModels->ppf1,
				pModels->ppf2, pModels->ppf3, pModels->ppf4, pModels->mode,
				modeName, pModels->direction, pModels->speed,
				pModels->distance);
		//fprintf(stderr, "%s", tmp);
		strcat(response, tmp);
		if (modelCount > 1) {
			pModels++;
		}
	}
}

void getModeName(char *Name, int modeID) {
	switch (modeID) {
	case 0:
		strcpy(Name, "Stop");
		break;
	case 1:
		strcpy(Name, "Back And Forward");
		break;
	case 2:
		strcpy(Name, "Goto To End");
		break;
	case 3:
		strcpy(Name, "Move");
		break;
	}

}

void strReplace(char* src, char dist, char rep) {
	char *p;
	p = src;
	while (*p != '\0') {
		if(*p==dist){
			*p = rep;
		}
		p++;
	}

}

