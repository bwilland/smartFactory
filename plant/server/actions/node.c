#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "node.h"
#include "autosearch.h"
#include "../shared/serialport.h"
#include "../shared/sharemem.h"
#include "../shared/lightingpack.h"
#include "model.h"

int i;
char* token;
char* actionPara;

void nodeManage(char* request, char* responseMsg) {
	char* p_Paras;
	int nodeID = 0, isReset = 0;
	char nodeIDs[512];
	p_Paras = request;
	token = strsep(&p_Paras, "?");
	char paras[15][1024];
	int i = 0, iCount = 0;

	while (p_Paras != NULL )
	//for (i = 0;i < 5;i++)        
	{
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

	if (strcmp(actionPara, "AutoSearch") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "ISRESET") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				isReset = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "isReset==%d\n", isReset);
		autoSearch(responseMsg, isReset);
		return;
	}
	//get all node action
	if (strcmp(actionPara, "AllNodeList") == 0) {

		//fprintf(stderr,"pageNo==%d \n",pageNo);
		actionAllNodeList(responseMsg, 0);
		return;
	}

	if (strcmp(actionPara, "AllLEDNodeList") == 0) {

		//fprintf(stderr,"pageNo==%d \n",pageNo);
		actionAllLEDNodeList(responseMsg);
		return;
	}

	//get node info
	if (strcmp(actionPara, "NodeInfo") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "NODEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				nodeID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "nodeID==%d\n", nodeID);
		actionNodeInfo(responseMsg, nodeID);
		return;
	}

	//SetLEDCurrent
	if (strcmp(actionPara, "SetLEDCurrent") == 0) {
		float current1 = 0.0f, current2 = 0.0f, current3 = 0.0f,
				current4 = 0.0f;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "NODEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				nodeID = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "CURRENT1") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				current1 = atof(actionPara);
				continue;
			}
			if (strstr(paras[i], "CURRENT2") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				current2 = atof(actionPara);
				continue;
			}
			if (strstr(paras[i], "CURRENT3") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				current3 = atof(actionPara);
				continue;
			}
			if (strstr(paras[i], "CURRENT4") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				current4 = atof(actionPara);
				continue;
			}
		}
		fprintf(stderr, "nodeID==%d\n", nodeID);
		actionSetLEDCurrent(responseMsg, nodeID, current1, current2, current3,
				current4);
		return;
	}
	//get node status
	if (strcmp(actionPara, "GetNodeStatus") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "NODEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				nodeID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "nodeID==%d\n", nodeID);
		actionGetNodeStatus(responseMsg, nodeID);
		return;
	}
	//save node info
	if (strcmp(actionPara, "SaveNode") == 0) {
		actionSaveNode(responseMsg, paras, iCount);
		return;
	}
	if (strcmp(actionPara, "DeleteNode") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "NODEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");

				strcpy(nodeIDs, actionPara);
				break;
			}
		}
		fprintf(stderr, "nodeID==%s\n", nodeIDs);
		actionDeleteNode(responseMsg, nodeIDs);
		return;
	}

	if (strcmp(actionPara, "ResetNode") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "NODEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				nodeID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "nodeID==%d\n", nodeID);
		actionResetNode(responseMsg, nodeID);
		return;
	}

	if (strcmp(actionPara, "setNodeCaliberate") == 0) {
		CALIBERATE caliberate;
		bzero(&caliberate, sizeof(CALIBERATE));
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "NODEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(caliberate.nodeIDs, actionPara);
				continue;
			}
			if (strstr(paras[i], "CHANNEL") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				caliberate.channel = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "LEDS") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(caliberate.LEDS, actionPara);

				continue;
			}
			if (strstr(paras[i], "PWM") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				caliberate.pwm = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "CURRENT") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				caliberate.current = atof(actionPara);
				continue;
			}
			if (strstr(paras[i], "PPF") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				caliberate.ppf = atof(actionPara);
				continue;
			}

		}

		if (caliberate.LEDS[strlen(caliberate.LEDS) - 1] == ',') {
			caliberate.LEDS[strlen(caliberate.LEDS) - 1] = '\0';
		}
		if (caliberate.nodeIDs[strlen(caliberate.nodeIDs) - 1] == ',') {
			caliberate.nodeIDs[strlen(caliberate.nodeIDs) - 1] = '\0';
		}

		fprintf(stderr, "nodeID==%s,LEDS==%s\n", caliberate.nodeIDs,
				caliberate.LEDS);
		actionSetNodeCaliberate(responseMsg, &caliberate);
		return;
	}
	if (strcmp(actionPara, "TurnOffLed") == 0) {
		char nodeIDs[256];
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "NODEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(nodeIDs, actionPara);
				break;
			}
		}
		if (nodeIDs[strlen(nodeIDs) - 1] == ',') {
			nodeIDs[strlen(nodeIDs) - 1] = '\0';
		}
		actionTurnOffLed(responseMsg, nodeIDs);
	}

	if (strcmp(actionPara, "GetNodeInfo") == 0) {
		int nodeId = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "NODEID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				nodeId = atoi(actionPara);
				break;
			}
		}
		actionGetNodeInfo(responseMsg, nodeId);
	}

}

void actionGetNodeInfo(char* responseMsg, int nodeId) {
	NODE node;
	getDBNodeInfo(&node, nodeId);
	BYTE command[128], receiver[BUFFERSIZE];
	COMMANDINFO commandInfo;
	bzero(&commandInfo, sizeof(COMMANDINFO));
	strcpy(commandInfo.mac, node.mac);
	strcpy(responseMsg, "<root>");

	int iCount = genGetNodeInfo(&commandInfo, command);

	if ((iCount = serialData(command, iCount, 1, receiver)) > 0) {

		printHEX(receiver, iCount);
		if (receiver[0] == 0x7E) {
			//LED
			//BYTE *pos = &receiver[42];
			char type[10], hardVer[10], firmVer[10];

			if (receiver[5] == 0x01) {
				strcpy(type, "Motor");
			} else {
				strcpy(type, "LED");
			}
			sprintf(hardVer, "%d.%d.%d", receiver[9], receiver[10],
					receiver[11]);
			sprintf(firmVer, "%d.%d.%d", receiver[13], receiver[14],
					receiver[15]);
			char tmp[256];
			sprintf(tmp,
					"<node><type>%s</type><hardVer>%s</hardVer><firmVer>%s</firmVer></node>",
					type, hardVer, firmVer);

			strcat(responseMsg, tmp);
		}

	}

	else {
		fprintf(stderr, "read==00\n");
		//TODO: 0

	}
	strcat(responseMsg, "</root>");
}

void actionTurnOffLed(char* responseMsg, char* nodeIDs) {

	char p_Paras[256];
	char* token, *p;
	strcpy(p_Paras, nodeIDs);
	p = p_Paras;

	BYTE ledCaliberate[64];
	bzero(ledCaliberate, 64);

	while (p != NULL ) {
		token = strsep(&p, ",");
		if (token != NULL ) {
			int nodeID = atoi(token);
			fprintf(stderr, "nodeID==%d\n", nodeID);
			turnOffLed(nodeID);
		} else {
			break;
		}
	}

	makeReturnMsg(responseMsg, 1, "turn node Finished.");
}
void turnOffLed(int nodeId) {
	NODE node;
	bzero(&node, sizeof(NODE));
	getDBNodeInfo(&node, nodeId);

	BYTE command[128], receiver[BUFFERSIZE];
	COMMANDINFO commandInfo;
	bzero(&commandInfo, sizeof(COMMANDINFO));

	strcpy(commandInfo.mac, node.mac);

	commandInfo.onoffSt = 0;
	commandInfo.PPF1 = 0;
	commandInfo.PPF2 = 0;
	commandInfo.PPF3 = 0;
	commandInfo.PPF4 = 0;

	int iCount = genSetLEDCurrent(&commandInfo, command);
	serialData(command, iCount, 1, receiver);

}

void setNodeCaliberate(CALIBERATE *caliberate) {
	NODE node;
	bzero(&node, sizeof(NODE));
	getDBNodeInfo(&node, caliberate->nodeId);

	BYTE command[128], receiver[BUFFERSIZE];
	COMMANDINFO commandInfo;
	bzero(&commandInfo, sizeof(COMMANDINFO));

	strcpy(commandInfo.mac, node.mac);
	commandInfo.channel = caliberate->channel;

	commandInfo.pwm = (unsigned int)(caliberate->pwm * 0xFFFF / 100);

	commandInfo.current = caliberate->current;
	commandInfo.PPF1 = caliberate->ppf;

	strcpy(commandInfo.groupIDs, caliberate->LEDS);

	int iCount = genSetLEDCalibrate(&commandInfo, command);
	serialData(command, iCount, 1, receiver);
}
void actionSetNodeCaliberate(char* responseMsg, CALIBERATE *caliberate) {

	char p_Paras[256];
	char* token, *p;
	strcpy(p_Paras, caliberate->nodeIDs);
	p = p_Paras;

	BYTE ledCaliberate[64];
	bzero(ledCaliberate, 64);

	while (p != NULL ) {
		token = strsep(&p, ",");
		if (token != NULL ) {
			int nodeID = atoi(token);
			fprintf(stderr, "nodeID==%d\n", nodeID);
			caliberate->nodeId = nodeID;
			setNodeCaliberate(caliberate);
		} else {
			break;
		}
	}

//	if ((iCount = serialData(command, iCount, 1, receiver)) > 0) {
	makeReturnMsg(responseMsg, 1, "Reset node Finished.");
//	} else {
//		makeReturnMsg(responseMsg, 0, "Reset node failed.");
//	}
	fprintf(stderr, "%s\n", responseMsg);

}
void actionResetNode(char* responseMsg, int nodeId) {

	NODE node;
	getDBNodeInfo(&node, nodeId);

	BYTE command[128];
	COMMANDINFO commandInfo;
	bzero(&commandInfo, sizeof(COMMANDINFO));
	strcpy(commandInfo.mac, node.mac);

	int iCount = genReset(&commandInfo, command);
	sendData(command, iCount, 1);
	makeReturnMsg(responseMsg, 1, "Reset node successes.");

	//fprintf(stderr, "%s\n", responseMsg);
}

void actionGetNodeStatus(char* responseMsg, int nodeId) {
//strcpy(responseMsg, "<root>");
	NODE node;
	getDBNodeInfo(&node, nodeId);
	getNodeStatus(responseMsg, &node);
//makeNodeInfoResponse(responseMsg, &node, 1);

//strcat(responseMsg, "</root>");
}
void actionSetLEDCurrent(char* responseMsg, int nodeId, float current1,
		float current2, float current3, float current4) {
	NODE node;
	getDBNodeInfo(&node, nodeId);

	node.current1 = current1;
	node.current1 = current2;
	node.current1 = current3;
	node.current1 = current4;

	if (setLEDCurrent(&node) == 1) {
		makeReturnMsg(responseMsg, 1, "Set LED Current successes.");
		return;
	}

	makeReturnMsg(responseMsg, 0, "Set LED Current failed.");
}

int setLEDCurrent(NODE *node) {

	//getDBNodeInfo(node, node->id);

	BYTE command[128], receiver[BUFFERSIZE];
	COMMANDINFO commandInfo;
	bzero(&commandInfo, sizeof(COMMANDINFO));
	strcpy(commandInfo.mac, node->mac);
	//commandInfo.current = current;
	commandInfo.PPF1 = node->current1;
	commandInfo.PPF2 = node->current2;
	commandInfo.PPF3 = node->current3;
	commandInfo.PPF4 = node->current4;

	int iCount = 0;
	if (node->type == 0) {
		//LED
		iCount = genSetLEDCurrent(&commandInfo, command);
		if ((iCount = serialData(command, iCount, 1, receiver)) > 0) {
			updateNodeCurrent(node->id, node->current1, node->current2,
					node->current3, node->current4);

			return 1;
		}
	}
	return 0;
}
void actionAllLEDNodeList(char* responseMsg) {
	strcpy(responseMsg, "<root>");
	char clause[64];
	sprintf(clause, "TYPE=0");
	int nodeCount = getResultCount("NODES", clause);

	NODE nodes[nodeCount];
	getAllNodeByType(nodes, 0);
	makeNodeInfoResponse(responseMsg, nodes, nodeCount);

	strcat(responseMsg, "</root>");
}

void actionDeleteNode(char* responseMsg, char* nodeIDs) {
	char *p = nodeIDs;
	p += strlen(nodeIDs) - 1;

	if (*p == ',') {
		*p = '\0'; //去掉最后一个逗号
	}

	while (nodeIDs != NULL )
//for (i = 0;i < 5;i++)
	{

		token = strsep(&nodeIDs, ",");
		if (token != NULL ) {
			//strcpy(paras[iCount], token);
			//fprintf(stderr,"token==%s nodeIDs==%s\n", token,nodeIDs);
			deleteNode(token);
		} else {
			break;
		}
	}
	/*	fprintf(stderr,"nodeID==%s\n", nodeIDs);
	 deleteNode(nodeIDs);*/
	updateAreaNodeCount();
	makeReturnMsg(responseMsg, 1, "delete node successes.");
}

void getNodeStatus(char* responseMsg, NODE* node) {
	strcpy(responseMsg, "<root>");
	BYTE command[128], receiver[BUFFERSIZE];
	COMMANDINFO commandInfo;
	bzero(&commandInfo, sizeof(COMMANDINFO));
	strcpy(commandInfo.mac, node->mac);
	int iCount = 0;
	if (node->type == 0) {
		//LED
		iCount = genGetLEDInfo(&commandInfo, command);
	} else {
		//MOTOR
		iCount = genMotorStatus(&commandInfo, command);
	}

	if ((iCount = serialData(command, iCount, 1, receiver)) > 0) {

		printHEX(receiver, iCount);
		if (receiver[0] == 0x7E) {

			if (node->type == 0) {

				//LED
				BYTE *pos = &receiver[9];
				LEDSTATUS ledStaus;
				BYTE floatByte[4];
				int i = 0, j = 0;
				for (i = 0; i < 4; i++) {
					ledStaus.ledNumber = (int) *pos;
					pos++;
					ledStaus.ledType = (int) *pos;
					pos += 3;
					for (j = 0; j < 4; j++) {
						floatByte[j] = (int) *pos;
						pos++;
					}
					float *p_float = (float*) floatByte;
					ledStaus.ppf = *p_float;

					for (j = 0; j < 4; j++) {
						floatByte[j] = (int) *pos;
						pos++;
					}
					p_float = (float*) floatByte;
					ledStaus.caliberated = *p_float;

					for (j = 0; j < 4; j++) {
						floatByte[j] = (int) *pos;
						pos++;
					}
					p_float = (float*) floatByte;
					ledStaus.current = *p_float;

					for (j = 0; j < 4; j++) {
						floatByte[j] = (int) *pos;
						pos++;
					}
					p_float = (float*) floatByte;
					ledStaus.miniCurrent = *p_float;

					for (j = 0; j < 4; j++) {
						floatByte[j] = (int) *pos;
						pos++;
					}
					p_float = (float*) floatByte;
					ledStaus.fullCurrent = *p_float;
					char tmp[256];
					sprintf(tmp,
							"<node><number>%d</number><type>%d</type><ppf>%.02f</ppf><caliberated>%.02f</caliberated><current>%.02f</current><miniCurrent>%.02f</miniCurrent><fullCurrent>%.02f</fullCurrent></node>",
							ledStaus.ledNumber, ledStaus.ledType, ledStaus.ppf,
							ledStaus.caliberated, ledStaus.current,
							ledStaus.miniCurrent, ledStaus.fullCurrent);
					strcat(responseMsg, tmp);
				}

			} else {
				BYTE *pos = &receiver[5];
				MOTORSTATUS motorStatus;

				motorStatus.mode = (int) *pos;
				pos++;
				motorStatus.direction = (int) *pos;
				pos++;
				motorStatus.switchFlag = (int) *pos;
				pos++;
				motorStatus.error = (int) *pos;
				pos++;
				BYTE floatByte[4];
				int j = 0;
				for (j = 0; j < 4; j++) {
					floatByte[j] = (int) *pos;
					pos++;
				}
				float *p_float = (float*) floatByte;
				motorStatus.distance = *p_float;
				for (j = 0; j < 4; j++) {
					floatByte[j] = (int) *pos;
					pos++;
				}
				p_float = (float*) floatByte;
				motorStatus.currentDistance = *p_float;
				for (j = 0; j < 4; j++) {
					floatByte[j] = (int) *pos;
					pos++;
				}
				p_float = (float*) floatByte;
				motorStatus.speed = *p_float;
				for (j = 0; j < 4; j++) {
					floatByte[j] = (int) *pos;
					pos++;
				}
				p_float = (float*) floatByte;
				motorStatus.pulses = *p_float;
				for (j = 0; j < 4; j++) {
					floatByte[j] = (int) *pos;
					pos++;
				}
				p_float = (float*) floatByte;
				motorStatus.maxSpeed = *p_float;
				for (j = 0; j < 4; j++) {
					floatByte[j] = (int) *pos;
					pos++;
				}
				p_float = (float*) floatByte;
				motorStatus.maxLength = *p_float;
				char tmp[256];
				char modeName[64];
				getModeName(modeName, motorStatus.mode);
				sprintf(tmp,
						"<node><mode>%s</mode><direction>%d</direction><switchFlag>%d</switchFlag><error>%d</error><distance>%f</distance><currentDistance>%f</currentDistance><speed>%f</speed><pulses>%f</pulses><maxSpeed>%f</maxSpeed><maxLength>%f</maxLength></node>",
						modeName, motorStatus.direction, motorStatus.switchFlag,
						motorStatus.error, motorStatus.distance,
						motorStatus.currentDistance, motorStatus.speed,
						motorStatus.pulses, motorStatus.maxSpeed,
						motorStatus.maxLength);
				strcat(responseMsg, tmp);
			}

		}
	} else {
		fprintf(stderr, "read==00\n");
		//TODO: 0

	}
	strcat(responseMsg, "</root>");
}

int checkNode(char* mac) {
	BYTE command[128], receiver[BUFFERSIZE];
	COMMANDINFO commandInfo;
	bzero(&commandInfo, sizeof(COMMANDINFO));
	strcpy(commandInfo.mac, mac);
	int iCount = genGetNodeInfo(&commandInfo, command);
	if ((iCount = serialData(command, iCount, 1, receiver)) > 0) {
		fprintf(stderr, "read==%d\n", iCount);
		return 1;
	} else {
		fprintf(stderr, "read==00\n");
		//TODO: 0
		return 1;
	}
}

int setMotorParam(NODE *node) {
	BYTE command[128], receiver[BUFFERSIZE];
	COMMANDINFO commandInfo;
	bzero(&commandInfo, sizeof(COMMANDINFO));

	strcpy(commandInfo.mac, node->mac);
	commandInfo.pulse = node->pulses;
	commandInfo.maxSpeed = node->speed;
	commandInfo.maxLength = node->distance;
//fprintf(stderr, "commandInfo.pulse==%f\n", commandInfo.pulse);

	int iCount = genSetMotorParameters(&commandInfo, command);

	if ((iCount = serialData(command, iCount, 1, receiver)) > 0) {
		fprintf(stderr, "read==%d\n", iCount);
		return 1;
	} else {
		fprintf(stderr, "read==00\n");
		//TODO: 0
		return 1;
	}
}

void actionSaveNode(char* responseMsg, char paras[][1024], int iCount) {
	NODE node;
	bzero(&node, sizeof(NODE));
	for (i = 0; i < iCount; i++) {
		if (strstr(paras[i], "NODEID") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.id = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "NODEMAC") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			strcpy(node.mac, actionPara);
			continue;
		}
		if (strstr(paras[i], "AREA") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.areaId = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "NODETYPE") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.type = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "SPEED") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.speed = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "DISTANCE") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.distance = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "PULSES") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.pulses = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "MEMO") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			strcpy(node.memo, actionPara);
			continue;
		}
		if (strstr(paras[i], "CURRENT1") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.current1 = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "CURRENT2") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.current2 = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "CURRENT3") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.current3 = atof(actionPara);
			continue;
		}
		if (strstr(paras[i], "CURRENT4") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			node.current4 = atof(actionPara);
			continue;
		}

	}

	strReplace(node.memo, '^', ' ');
	fprintf(stderr, "NODEID==%d,memo=%s,nodeType=%d ,,pulses==%f\n", node.id,
			node.memo, node.type, node.pulses);
//TODO
//node.isValid = checkNode(node.mac);
	node.isValid = 1;
	if (node.isValid == 1) {
		if (node.id == -1) {
			char tmp[32];
			sprintf(tmp, "MAC='%s'", node.mac);
			if (getResultCount("NODES", tmp) == 0) {
				insertNode(&node);
			} else {
				makeReturnMsg(responseMsg, -1, "save node failed.");
				return;
			}
		} else {
			updateNode(&node);
		}
		updateAreaNodeCount();
		makeReturnMsg(responseMsg, node.id, "save node successes.");
		if (node.type == 1) {
			setMotorParam(&node);
		} else {
			setLEDCurrent(&node);
		}
		return;
	} else {
		makeReturnMsg(responseMsg, -1, "save node failed.");
	}

//makeReturnMsg(responseMsg, 1, "save node successes.");
	return;
}

void actionNodeInfo(char* responseMsg, int nodeID) {

	strcpy(responseMsg, "<root>");
	NODE node;
	getDBNodeInfo(&node, nodeID);
	makeNodeInfoResponse(responseMsg, &node, 1);

	strcat(responseMsg, "</root>");

}

void actionAllNodeList(char* responseMsg, int pageNo) {

	strcpy(responseMsg, "<root>");
	int nodeCount = getResultCount("NODES", NULL );

	NODE nodes[nodeCount];
	getAllNode(nodes);
	makeNodeInfoResponse(responseMsg, nodes, nodeCount);

	strcat(responseMsg, "</root>");

}

void makePageInfoResponse(char* response, int maxPage, int currentPage) {
	char tmp[256];
	sprintf(tmp, "<page><maxpage>%d</maxpage><pageno>%d</pageno></page>",
			maxPage, currentPage);
	strcat(response, tmp);
}
void makeNodeInfoResponse(char* response, NODE* pnodes, int nodeCount) {
	char tmp[512];
	int i = 0;
	for (i = 0; i < nodeCount; i++) {
		sprintf(tmp,
				"<node><id>%d</id><mac>%s</mac><domain>%d</domain><memo>%s</memo><type>%d</type><speed>%.2f</speed><distance>%.2f</distance><pulses>%.2f</pulses><curr1>%.2f</curr1><curr2>%.2f</curr2><curr3>%.2f</curr3><curr4>%.2f</curr4><isValid>%d</isValid></node>",
				pnodes->id, pnodes->mac, pnodes->areaId, pnodes->memo,
				pnodes->type, pnodes->speed, pnodes->distance, pnodes->pulses,
				pnodes->current1, pnodes->current2, pnodes->current3,
				pnodes->current4, pnodes->isValid);
		strcat(response, tmp);
		if (nodeCount > 1) {
			pnodes++;
		}
	}
}

NODE* getNodeInfoFromMem(NODE* p_node, int nodeID, int nodeCount) {
	NODE* p_tmp = p_node;
	int i = 0;
	for (i = 0; i < nodeCount; i++) {
		if (p_tmp->id == nodeID) {
			return p_tmp;
		}
		p_tmp++;

	}
	return NULL ;
}
void makeReturnMsg(char* responseMsg, int returnCd, char* message) {
	sprintf(responseMsg,
			"<root><return><code>%d</code><message>%s</message></return></root>",
			returnCd, message);
	return;
}
