#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "area.h"
#include "node.h"
#include "../shared/lightingpack.h"
#include "../shared/serialport.h"
#include "../shared/sharemem.h"
#include "model.h"

void areaManage(char* request, char* responseMsg) {
	char* p_Paras;
	char* token;
	int areaID = 0;
	char mac[9], sortedAreaIDs[256];
	p_Paras = request;
//fprintf(stderr,"%s\n",request);
	token = strsep(&p_Paras, "?");
	char paras[10][1024];
	int i = 0, iCount = 0;
	while (p_Paras != NULL ) {
		i++;
		token = strsep(&p_Paras, "&");
		if (token != NULL ) {
			//fprintf(stderr,"token==%s \n",token);
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

	//get area list
	if (strcmp(actionPara, "AreaList") == 0) {
		actionAreaList(responseMsg);
		return;
	}

	if (strcmp(actionPara, "SaveArea") == 0) {
		actionSaveArea(paras, responseMsg, iCount);
		return;
	}

	if (strcmp(actionPara, "AddAreaNode") == 0) {
		actionAddAreaNode(paras, responseMsg, iCount);
		return;
	}
	if (strcmp(actionPara, "AreaInfo") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				areaID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "areaID==%d\n", areaID);
		actionAreaInfo(responseMsg, areaID);
		return;
	}
	//get area info
	if (strcmp(actionPara, "AreaNodeAdd") == 0) {
		int nodeType = 0;
		//fprintf(stderr, "iCount==%d\n", iCount);
		for (i = 0; i < iCount; i++) {
			//fprintf(stderr, "paras[i]==%s\n", paras[i]);
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				areaID = atoi(actionPara);
				//break;
				continue;
			}
			if (strstr(paras[i], "TYPE") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				nodeType = atoi(actionPara);
				//break;
				continue;
			}
		}
		fprintf(stderr, "areaID==%d,,nodeType==%d\n", areaID, nodeType);
		actionAreaNode4Setting(responseMsg, areaID, nodeType);
		return;
	}

	//get nodes in area
	if (strcmp(actionPara, "AreaNode") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				areaID = atoi(actionPara);
				break;
			}
		}
		//fprintf(stderr,"areaID==%d\n",areaID);
		actionAreaNode(responseMsg, areaID);
		return;
	}

	if (strcmp(actionPara, "DeleteArea") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				areaID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "areaID==%d\n", areaID);
		actionDeleteArea(responseMsg, areaID);
		return;
	}
	if (strcmp(actionPara, "AreaQuery") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "MAC") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(mac, actionPara);
				break;
			}
		}
		fprintf(stderr, "mac==%s\n", mac);
		actionAreaQuery(responseMsg, mac);
		return;
	}
	if (strcmp(actionPara, "AreaSort") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAIDS") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(sortedAreaIDs, actionPara);
				break;
			}
		}
		//fprintf(stderr, "sortedAreaIDs==%s\n", sortedAreaIDs);
		actionAreaSaveSort(responseMsg, sortedAreaIDs);
		return;
	}
}

void actionAreaSaveSort(char* responseMsg, char* sortedAreaIDs) {
	char *token, tmp[32];
	int i = 1;
	while (sortedAreaIDs != NULL ) {

		token = strsep(&sortedAreaIDs, ",");
		if (token != NULL ) {
			//strcpy(paras[iCount], token);
			//fprintf(stderr,"token==%s ORDERID==%d\n", token,i);
			deleteNode(token);
			sprintf(tmp, "ROWID=%s AND ORDERID=%d", token, i);
			if (getResultCount("AREA", tmp) == 0) {
				updateAreaOrder(atoi(token), i);
			}
		} else {
			break;
		}
		i++;
	}
	makeReturnMsg(responseMsg, 1, "save area sort successes.");
}
void actionDeleteArea(char* responseMsg, int areaID) {
	//更新数据库
	deleteAreaNode(areaID);
	deleteArea(areaID);

	//更新内存表
	SHAREMEM* share_mem = get_shm_Memory(TRUE);
	AREAINFO* p_area = share_mem->p_Area;
	int i = 0;
	AREAINFO* removedArea = NULL;
	for (i = 0; i < share_mem->areaPlaceholder; i++) {
		if (p_area->id == areaID) {
			removedArea = p_area;
			//not break;
		}
		p_area++;
	}
	p_area--; //返回最后一个
	if (removedArea != p_area) {
		//把最后一个移动上来
		memcpy(removedArea, p_area, sizeof(AREAINFO));

	}
	share_mem->areaPlaceholder--;

	release_shm_Memory(share_mem, TRUE);
	makeReturnMsg(responseMsg, 1, "delete area successes.");
}

void actionAreaList(char* responseMsg) {

	//get all Areas
	strcpy(responseMsg, "<root>");
	int areaCount = getResultCount("AREA", NULL );

	if (areaCount > 0) {
		SHAREMEM* share_mem = get_shm_Memory(FALSE);
		AREAINFO* p_area = share_mem->p_Area;
		AREA areas[areaCount];

		getAreaList(areas);

		char tmp[256];
		int i = 0, j = 0;

		for (i = 0; i < areaCount; i++) {
			BOOL isLocked = FALSE;
			for (j = 0; j < share_mem->areaPlaceholder; j++) {
				if (p_area->id == areas[i].id) {
					isLocked = p_area->isLocked;
					break;
				}
				p_area++;
			}

			sprintf(tmp,
					"<domain><id>%d</id><name>%s</name><memo>%s</memo><nodecount>%d</nodecount><isLocked>%d</isLocked></domain>",
					areas[i].id, areas[i].name, areas[i].memo,
					areas[i].nodeCount, isLocked);
			strcat(responseMsg, tmp);
		}
		release_shm_Memory(share_mem, FALSE);
	} else {
		strcat(responseMsg, "No AREA");
	}
	strcat(responseMsg, "</root>");

	//fprintf(stderr,"arealist:\n %s",responseMsg);
}

void actionAreaNode(char* responseMsg, int areaID) {
	fprintf(stderr, "AreaNode Action... \n");

	strcpy(responseMsg, "<root>");
	char tmp[64];
	sprintf(tmp, "AREAID=%d AND ISVALID=1", areaID);
	int nodeCount = getResultCount("NODES", tmp);
	if (nodeCount > 0) {
		NODE nodes[nodeCount];
		getNodeByAreaId(nodes, areaID);
		makeNodeInfoResponse(responseMsg, nodes, nodeCount);
	}

	strcat(responseMsg, "</root>");

	fprintf(stderr, "AreaNode END!\n");
	return;
}

void actionAreaNode4Setting(char* responseMsg, int areaID, int nodeType) {
	fprintf(stderr, "actionAreaNode4Setting Action... \n");

	strcpy(responseMsg, "<root>");
	char tmp[64];
	sprintf(tmp, "(AREA=0 OR AREA=%d) and TYPE=%d", areaID, nodeType);
	int nodeCount = getResultCount("NODES", tmp);
	fprintf(stderr, "nodeCount==%d\n", nodeCount);
	if (nodeCount > 0) {
		NODE nodes[nodeCount];
		bzero(nodes, sizeof(NODE) * nodeCount);
		getNodes4Area(nodes, areaID, nodeType);
		makeNodeInfoResponse(responseMsg, nodes, nodeCount);
	}

	strcat(responseMsg, "</root>");

	fprintf(stderr, "actionAreaNode4Setting END!\n");
	return;
}

void actionAreaQuery(char* responseMsg, char *mac) {

	int areaId = getAreaIDbyMac(mac);
	if (areaId > 0) {
		actionAreaInfo(responseMsg, areaId);
	} else {
		strcpy(responseMsg, "<root></root>");
	}
}

void actionAreaInfo(char* responseMsg, int areaID) {
	AREA areainfo;
	strcpy(responseMsg, "<root>");
	if (getAreaInfo(&areainfo, areaID)) {
		//OK
		char tmp[256];
		sprintf(tmp,
				"<domain><id>%d</id><name>%s</name><memo>%s</memo><kr>%d</kr><kb>%d</kb></domain>",
				areainfo.id, areainfo.name, areainfo.memo, areainfo.Kr,
				areainfo.Kb);

		strcat(responseMsg, tmp);

	}
	strcat(responseMsg, "</root>");
}

void actionSaveArea(char paras[][1024], char* responseMsg, int iCount) {
	char* actionPara = NULL;
	int areaID = 0;
	char* areaName = NULL;
	char* memo = NULL;
	//int kr = 0, kb = 0;
	//char* token = NULL;
	int i;
	for (i = 0; i < iCount; i++) {
		if (strstr(paras[i], "AREAID") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			areaID = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "AREANAME") != NULL ) {
			areaName = paras[i];
			strsep(&areaName, "=");
			continue;
		}
		if (strstr(paras[i], "MEMO") != NULL ) {
			memo = paras[i];
			strsep(&memo, "=");
			continue;
		}
//		if (strstr(paras[i], "PHOTOBLUE") != NULL ) {
//			actionPara = paras[i];
//			strsep(&actionPara, "=");
//			kb = atoi(actionPara);
//			continue;
//		}
//		if (strstr(paras[i], "PHOTORED") != NULL ) {
//			actionPara = paras[i];
//			strsep(&actionPara, "=");
//			kr = atoi(actionPara);
//			fprintf(stderr, "KR=%d", kr);
//			continue;
//		}

	}
	strReplace(areaName, '^', ' ');
	strReplace(memo, '^', ' ');

	SHAREMEM* shr_mem = get_shm_Memory(TRUE);
	AREAINFO *pArea = shr_mem->p_Area;

	if (areaID == -1) { //New Area
		areaID = insertNewArea(areaName, memo, 0, 0);

		// refresh memory
		AREAINFO area;
		bzero(&area, sizeof(AREAINFO));
		area.id = areaID;
		strcpy(area.name, areaName);
		strcpy(area.memo, memo);
		//	area.Kr = kr;
		//area.Kb = kb;

		pArea += shr_mem->areaPlaceholder;
		memcpy(pArea, &area, sizeof(AREAINFO));
		shr_mem->areaPlaceholder++;

	} else {
		fprintf(stderr, "Update area %d\n", areaID);
		updateArea(areaID, areaName, memo, 0, 0);
		for (i = 0; i < shr_mem->areaPlaceholder; i++) {
			if (pArea->id == areaID) {
				strcpy(pArea->name, areaName);
				strcpy(pArea->memo, memo);
				//		pArea->Kr = kr;
				//		pArea->Kb = kb;
				break;
			}
			pArea++;
		}
	}
	release_shm_Memory(shr_mem, TRUE);
	makeReturnMsg(responseMsg, areaID, "Add area successes.");
}

void actionAddAreaNode(char paras[][1024], char* responseMsg, int iCount) {
	int areaID = 0, nodeType = 0;
	char* nodeIDs = NULL;
	char *strAreaID;
	int i;
	for (i = 0; i < iCount; i++) {
		//fprintf(stderr, "%s\n", paras[i]);
		if (strstr(paras[i], "AREAID") != NULL ) {
			strAreaID = paras[i];
			strsep(&strAreaID, "=");
			areaID = atoi(strAreaID);
			continue;
		}
		if (strstr(paras[i], "NODE") != NULL ) {
			nodeIDs = paras[i];
			strsep(&nodeIDs, "=");
			continue;
		}
		if (strstr(paras[i], "TYPE") != NULL ) {

			strAreaID = paras[i];
			strsep(&strAreaID, "=");
			//fprintf(stderr,"strAreaID===%s\n",strAreaID);
			nodeType = atoi(strAreaID);
			continue;

		}
	}
	if (nodeIDs[strlen(nodeIDs) - 1] == ';') {
		nodeIDs[strlen(nodeIDs) - 1] = '\0';
	}
	fprintf(stderr, "areaID==%d,,nodeType==%d\n", areaID, nodeType);
	addAreaNodes(responseMsg, areaID, nodeIDs, nodeType);
}

void addAreaNodes(char* responseMsg, int areaID, char* nodeIDs, int nodeType) {
	fprintf(stderr, "addAreaNodes Action... \n");
	char* token;

	deleteAreaNodeWithType(areaID, nodeType);

	while (nodeIDs != NULL )       // divide the protocol head in blank */
	{
		token = strsep(&nodeIDs, ";");
		updateAreaNode(areaID, token);
	}
	updateAreaNodeCount();

	if (nodeType == 0) {
		//set GroupMask if LED
		setGroupMask(areaID, nodeType);
	}
	makeReturnMsg(responseMsg, areaID, "Add node successes.");

	fprintf(stderr, "addAreaNodes Action...END \n");

}

void setGroupMask(int areaID, int nodeType) {
	//set GroupMask
	static int CMDLENCOUNT = 49;
	COMMANDINFO cInfo;
	bzero(&cInfo, sizeof(COMMANDINFO));
	char tmp[64];

	int i = 0;
	sprintf(tmp, "AREA=%d AND ISVALID=1 and TYPE=%d", areaID, nodeType);
	fprintf(stderr, "tmp==%s\n", tmp);

	int nodeCount = getResultCount("NODES", tmp);

	if (nodeCount > 0) {
		NODE nodes[nodeCount];
		BYTE command[128];
		BYTE cmdBuff[CTRL_CMD_LEN * nodeCount];
		BYTE *p_cmd = cmdBuff;
		sprintf(cInfo.groupIDs, "%d;", areaID);

		getNodeByAreaIdType(nodes, areaID, nodeType);
		for (i = 0; i < nodeCount; i++) {
			strcpy(cInfo.mac, nodes[i].mac);

			int iCount = genSetLEDGroupsMask(&cInfo, command);
			fprintf(stderr, "iCount==%d  \n", iCount);
			memcpy(p_cmd, command, CMDLENCOUNT);
			p_cmd += CMDLENCOUNT;
		}

		//int iSend = 0;
		//for (iSend = 0; iSend < resendTimes; iSend++) {
		//printHEX(cmdBuff, CTRL_CMD_LEN * nodeCount);
		sendData(cmdBuff, CMDLENCOUNT, nodeCount);
		//}

	}
}

