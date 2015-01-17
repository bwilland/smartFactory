#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "../shared/sharemem.h"
#include "../thread/heartBeatThread.h"
#include "../shared/serialport.h"
#include "../shared/lightingpack.h"

int i;
char* token;
char* actionPara;

void actionAllNodeList(xmlDocPtr pdoc) {
	int nodeCount = getResultCount("NODES", "ISVALID=1");
	if (nodeCount > 0) {
		NODE nodes[nodeCount];
		getAllNodeList(nodes);

		char nodesInfo[16 * nodeCount], temp[16];
		bzero(nodesInfo, 16 * nodeCount);

		int i = 0;

		for (i = 0; i < nodeCount; i++) {
			sprintf(temp, "%d:%s:%d:%d,", nodes[i].id, nodes[i].mac,
					nodes[i].nodeType, nodes[i].model);
			strcat(nodesInfo, temp);
		}

		//delete last ,
		nodesInfo[strlen(nodesInfo) - 1] = '\0';

		fprintf(stderr, "%s\n", nodesInfo);

		xmlNodePtr root = xmlDocGetRootElement(pdoc);

		xmlNewTextChild(root, NULL, (const xmlChar *) "Devices",
				(const xmlChar *) nodesInfo);
	}
}

void controlCMD(xmlDocPtr pdoc, SERVERCOMMAND *cmdInfo) {
	int iLength = 0, result = 0;
	BYTE command[64];
	if (strcmp(cmdInfo->type, "GroupCtrl") == 0) {
		//group control
		fprintf(stderr,
				"cmdInfo->groupID==%d  cmdInfo->roomSensor==%d,cmdInfo->dayLightSensor==%d,,cmdInfo->dimmer==%d\n",
				cmdInfo->groupID, cmdInfo->roomSensor, cmdInfo->dayLightSensor,
				cmdInfo->dimmer);

		if (cmdInfo->roomSensor == 1 || cmdInfo->dayLightSensor == 1) {
			cmdInfo->isSensored = 1;
			if (cmdInfo->roomSensor == 1) {
				cmdInfo->sensorType = 1;
			} else {
				cmdInfo->sensorType = 2;
			}

		} else {
			cmdInfo->isSensored = 0;
		}
		if (!cmdInfo->isSensored) {
			iLength = genSingleGroupControlFrame(cmdInfo, command);
			fprintf(stderr, "genSingleGroupControlFrame %d!\n", iLength);

			result = sendSingleData(command, iLength);
		}

		//-------------------------------------
		COMMLOG log;
		time_t timeNow;
		time(&timeNow);
		log.dimmer = cmdInfo->dimmer;
		log.groupId = cmdInfo->groupID;
		log.optionType = cmdInfo->onOff;
		log.endTime = timeNow;
		log.startTime = timeNow;
		//strcpy(log.groupName ,"");//not used

		writeDBLog(&log);

		SHAREMEM* shr_mem = get_shm_Memory(TRUE);
		GROUPAUTODIMMER* p_groupDimmer = shr_mem->p_groupDimmer;
		int i = 0;
		for (i = 0; i < shr_mem->groupPlaceholder; i++) {

			if (p_groupDimmer->groupID == cmdInfo->groupID) {
				//manu controled, remove sensor setting.
				p_groupDimmer->isSensored = cmdInfo->isSensored;
				p_groupDimmer->sensorType = cmdInfo->sensorType;
				p_groupDimmer->currentDimmer = cmdInfo->dimmer;
				break;
			}
			p_groupDimmer++;
		}
		release_shm_Memory(shr_mem, TRUE);

		if (cmdInfo->isSensored) {
			checkDimmer();
		}

		//send current group info
		sendGroupDimmer();
		//}
	} else {
		iLength = genDeviceControlFrame(cmdInfo, command);
		//genQueryControlFrame(NULL,commond);
		//readLength = serialData(command, iLength, 1, recieveBuff);
		result = sendSingleData(command, iLength);

		//TODO:readLength == 0
		if (result == 1) {
			//Error: Time over
			addErrorMsg(pdoc, 400);
		}
	}
}
/**
 * Add an error message into xml doc
 */
void addErrorMsg(xmlDocPtr pdoc, int messageID) {
	xmlNodePtr root = xmlDocGetRootElement(pdoc);
	char code[10];
	sprintf(code, "%d", messageID);
	xmlNewTextChild(root, NULL, (const xmlChar *) "ErrorCode",
			(const xmlChar *) code);
}
