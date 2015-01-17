/*
 * xmlParse.c
 *
 *  Created on: Oct 27, 2014
 *      Author: root
 */
#include "xmlParse.h"

int parseXMLType(xmlDocPtr pdoc) {
	xmlNodePtr root = xmlDocGetRootElement(pdoc);

	if (xmlStrcmp(root->name, (const xmlChar *) "ROOT")) {
		fprintf(stderr, "document of the wrong type, root node != ROOT");
		return 0;
	}

	xmlNodePtr cur = root->xmlChildrenNode;
	xmlChar *key = NULL;
	while (cur != NULL ) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "Type"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			if (strcmp((const char *) key, "TCP_OK") == 0) {
				xmlFree(key);
				return TCP_OK;
			} else if (strcmp((const char *) key, "NodeCtrl") == 0) {
				xmlFree(key);
				return CTRL_COMMAND;
			} else if (strcmp((const char *) key, "GroupCtrl") == 0) {
				xmlFree(key);
				return CTRL_COMMAND;
			} else if (strcmp((const char *) key, "ASK_REGISTRATION") == 0) {
				xmlFree(key);
				return ASK_REGISTION;
			} else if (strcmp((const char *) key, "GroupSetting") == 0) {
				xmlFree(key);
				return SETTING_COMMAND;
			} else if (strcmp((const char *) key, "Synchronization") == 0) {
				xmlFree(key);
				return SYNCHRONIZATION;
			} else if (strcmp((const char *) key, "DeviceSearch") == 0) {
				xmlFree(key);
				return SYNCHRONIZATION_AUTOSEARCH;
			} else if (strcmp((const char *) key, "Programe") == 0) {
				xmlFree(key);
				return PROGRAM_SETTING;
			}else if (strcmp((const char *) key, "Registion") == 0) {
				xmlFree(key);
				return REGISTION;
			}

		}

		cur = cur->next;
	}

	//xmlFreeDoc(pdoc);
	return 1;
}

int parseRegistionXML(xmlDocPtr pdoc, REGISTIONINFO* regInfo) {

	xmlNodePtr root = xmlDocGetRootElement(pdoc);
	//得到当前节点的第一个子节点，即第一个ITEM
	xmlNodePtr cur = root->xmlChildrenNode;
	xmlChar *key = NULL;
	while (cur != NULL ) {
		//fprintf(stderr,"cur->name==%s \n",cur->name);
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "Type"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(regInfo->type, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "GateUID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(regInfo->appUID, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "FrameType"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			regInfo->frameType = atoi((const char*) key);
			fprintf(stderr, "regInfo->frameType==%d \n", regInfo->frameType);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "OrgID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			regInfo->orgID = atoi((const char*) key);
			xmlFree(key);
		}

		cur = cur->next;
	}
	//xmlFreeDoc(pdoc);

	return 1;
}

int parseSettingXML(xmlDocPtr pdoc, GroupSettingCommand* settingInfo) {

	xmlNodePtr root = xmlDocGetRootElement(pdoc);
	//得到当前节点的第一个子节点，即第一个ITEM
	xmlNodePtr cur = root->xmlChildrenNode;
	xmlChar *key = NULL;
	while (cur != NULL ) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "Type"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(settingInfo->type, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "FrameType"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			if (strcmp((const char *) key, "GroupSetting") == 0) {
				settingInfo->frameType = GROUPSETTING;
			}
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "GroupID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			settingInfo->groupID = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "Devices"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(settingInfo->devices, (const char*) key);
			xmlFree(key);
		}

		cur = cur->next;
	}

	return 1;
}

int parseProgramXML(xmlDocPtr pdoc, PROGRAM* program) {
	fprintf(stderr, "parseProgramXML \n");
	xmlNodePtr root = xmlDocGetRootElement(pdoc);
	//得到当前节点的第一个子节点，即第一个ITEM
	xmlNodePtr cur = root->xmlChildrenNode;
	xmlChar *key = NULL;
	while (cur != NULL ) {
		//fprintf(stderr,"cur->name==%s \n",cur->name);

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "ProgrameID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->id = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "FrameType"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->frameType = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "Name"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(program->name, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "GroupID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(program->groupList, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "Week"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(program->week, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "StartTime"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(program->startTime, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "EndTime"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(program->endTime, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "Dimmer"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->dimmer = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "SensorLevel"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->sensorLevel = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "RoomSensor"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->roomSensor = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "DaylightSensor"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->DaylightSenesor = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "Motion"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->motion = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "MotionDelayTime"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->motionDelayTime = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "MotionDimmer"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->MotionDimmer = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "MotionRoomSensor"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->MotionRoomSensor = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "MotionSecureAlarm"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			program->MotionSecureAlarm = atoi((const char*) key);
			xmlFree(key);
		}

		cur = cur->next;
	}
	//fprintf(stderr,"parseProgramXML 3 \n");
	return 1;
}

int parseSynchronizationXML(xmlDocPtr pdoc, int *frameType) {

	xmlNodePtr root = xmlDocGetRootElement(pdoc);
	//得到当前节点的第一个子节点，即第一个ITEM
	xmlNodePtr cur = root->xmlChildrenNode;
	xmlChar *key = NULL;
	while (cur != NULL ) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "FrameType"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			if (strcmp((const char *) key, "DeviceSearch") == 0) {
				*frameType = SYNCHRONIZATION_AUTOSEARCH;
			}
			xmlFree(key);
		}
		cur = cur->next;
	}

	return 1;
}

int parseCommadXML(xmlDocPtr pdoc, SERVERCOMMAND* cmdInfo) {

	xmlNodePtr root = xmlDocGetRootElement(pdoc);
	//得到当前节点的第一个子节点，即第一个ITEM
	xmlNodePtr cur = root->xmlChildrenNode;
	xmlChar *key = NULL;
	while (cur != NULL ) {
//fprintf(stderr,"cur->name==%s \n",cur->name);
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "Type"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(cmdInfo->type, (const char*) key);
			xmlFree(key);

		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "MID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(cmdInfo->MAC, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "GroupID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			cmdInfo->groupID = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "DeviceID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			cmdInfo->deviceID = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "GateUID"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			strcpy(cmdInfo->appUID, (const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "FrameType"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			cmdInfo->frameType = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "OnOff"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			cmdInfo->onOff = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "Brightness"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			cmdInfo->dimmer = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "RoomSensor"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			cmdInfo->roomSensor = atoi((const char*) key);
			xmlFree(key);
		} else if ((!xmlStrcmp(cur->name, (const xmlChar *) "DaylightSensor"))) {
			key = xmlNodeListGetString(pdoc, cur->xmlChildrenNode, 1);
			cmdInfo->dayLightSensor = atoi((const char*) key);
			xmlFree(key);
		}
		cur = cur->next;
	}
	//xmlFreeDoc(pdoc);

	return 1;
}
