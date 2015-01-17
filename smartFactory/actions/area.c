#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "group.h"
#include "node.h"
#include "../shared/serialport.h"
#include "../shared/sharemem.h"

void groupManage(char* request, char* responseMsg) {
	char* p_Paras;
	char* token;
	int groupID = 0;
	char mac[9], sortedGroupIDs[256];
	p_Paras = request;
//fprintf(stderr,"%s\n",request);
	token = strsep(&p_Paras, "?");
	char paras[10][1024];
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

	//get group list
	if (strcmp(actionPara, "GroupList") == 0) {
		actionGroupList(responseMsg);
		return;
	}

	if (strcmp(actionPara, "SaveGroup") == 0) {
		actionSaveGroup(paras, responseMsg, iCount);
		return;
	}

	if (strcmp(actionPara, "AddGroupNode") == 0) {
		actionAddGroupNode(paras, responseMsg, iCount);
		return;
	}
	if (strcmp(actionPara, "GroupInfo") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				groupID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "groupID==%d\n", groupID);
		actionGroupInfo(responseMsg, groupID);
		return;
	}
	//get group info
	if (strcmp(actionPara, "GroupNodeAdd") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				groupID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "groupID==%d\n", groupID);
		actionGroupNode4Setting(responseMsg, groupID);
		return;
	}

	//get nodes in group
	if (strcmp(actionPara, "GroupNode") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				groupID = atoi(actionPara);
				break;
			}
		}
		//fprintf(stderr,"groupID==%d\n",groupID);
		actionGroupNode(responseMsg, groupID);
		return;
	}

	if (strcmp(actionPara, "DeleteGroup") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				groupID = atoi(actionPara);
				break;
			}
		}
		fprintf(stderr, "groupID==%d\n", groupID);
		actionDeleteGroup(responseMsg, groupID);
		return;
	}
	if (strcmp(actionPara, "GroupQuery") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "MAC") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(mac, actionPara);
				break;
			}
		}
		fprintf(stderr, "mac==%s\n", mac);
		actionGroupQuery(responseMsg, mac);
		return;
	}
	if (strcmp(actionPara, "GroupSort") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "AREAIDS") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(sortedGroupIDs, actionPara);
				break;
			}
		}
		//fprintf(stderr, "sortedGroupIDs==%s\n", sortedGroupIDs);
		actionGroupSaveSort(responseMsg, sortedGroupIDs);
		return;
	}
}

void actionGroupSaveSort(char* responseMsg, char* sortedGroupIDs) {
	char *token, tmp[32];
	int i = 1;
	while (sortedGroupIDs != NULL ) {

		token = strsep(&sortedGroupIDs, ",");
		if (token != NULL ) {
			//strcpy(paras[iCount], token);
			//fprintf(stderr,"token==%s ORDERID==%d\n", token,i);
			deleteNode(token);
			sprintf(tmp, "ROWID=%s AND ORDERID=%d", token, i);
			if (getResultCount("AREA", tmp) == 0) {
				updateGroupOrder(atoi(token), i);
			}
		} else {
			break;
		}
		i++;
	}
	makeReturnMsg(responseMsg, 1, "save group sort successes.");
}
void actionDeleteGroup(char* responseMsg, int groupID) {
	//更新数据库
	deleteGroupNode(groupID);
	deleteGroup(groupID);

	//更新内存表
	SHAREMEM* share_mem = get_shm_Memory(TRUE);
	/*AREAINFO* p_group = share_mem->p_Group;
	int i = 0;
	AREAINFO* removedGroup = NULL;
	for (i = 0; i < share_mem->groupPlaceholder; i++) {
		if (p_group->id == groupID) {
			removedGroup = p_group;
			//not break;
		}
		p_group++;
	}
	p_group--; //返回最后一个
	if (removedGroup != p_group) {
		//把最后一个移动上来
		memcpy(removedGroup, p_group, sizeof(AREAINFO));

	}
	share_mem->groupPlaceholder--;
*/
	release_shm_Memory(share_mem, TRUE);
	makeReturnMsg(responseMsg, 1, "delete group successes.");
}

void actionGroupList(char* responseMsg) {

	//get all Groups
	strcpy(responseMsg, "<root>");
	int groupCount = getResultCount("AREA", NULL );

	if (groupCount > 0) {
		SHAREMEM* share_mem = get_shm_Memory(FALSE);
		/*
		AREAINFO* p_group = share_mem->p_Group;
		AREA groups[groupCount];

		getGroupList(groups);

		char tmp[256];
		int i = 0, j = 0;

		for (i = 0; i < groupCount; i++) {
			BOOL isLocked = FALSE;
			for (j = 0; j < share_mem->groupPlaceholder; j++) {
				if (p_group->id == groups[i].id) {
					isLocked = p_group->isLocked;
					break;
				}
				p_group++;
			}

			sprintf(tmp,
					"<domain><id>%d</id><name>%s</name><memo>%s</memo><nodecount>%d</nodecount><isLocked>%d</isLocked></domain>",
					groups[i].id, groups[i].name, groups[i].memo,
					groups[i].nodeCount, isLocked);
			strcat(responseMsg, tmp);
		}
		release_shm_Memory(share_mem, FALSE);
	} else {
		strcat(responseMsg, "No AREA");
	}
	strcat(responseMsg, "</root>");
*/
	//fprintf(stderr,"grouplist:\n %s",responseMsg);
}

void actionGroupNode(char* responseMsg, int groupID) {
	fprintf(stderr, "GroupNode Action... \n");

	strcpy(responseMsg, "<root>");
	char tmp[64];
	sprintf(tmp, "AREAID=%d AND ISVALID=1", groupID);
	int nodeCount = getResultCount("NODES", tmp);
	if (nodeCount > 0) {
		NODE nodes[nodeCount];
		getNodeByGroupId(nodes, groupID);
		makeNodeInfoResponse(responseMsg, nodes, nodeCount);
	}

	strcat(responseMsg, "</root>");

	fprintf(stderr, "GroupNode END!\n");
	return;
}

void actionGroupNode4Setting(char* responseMsg, int groupID) {
	fprintf(stderr, "actionGroupNode4Setting Action... \n");

	strcpy(responseMsg, "<root>");
	char tmp[64];
	sprintf(tmp, "AREA=0 OR AREA=%d", groupID);
	int nodeCount = getResultCount("NODES", tmp);
	fprintf(stderr, "nodeCount==%d\n", nodeCount);
	if (nodeCount > 0) {
		NODE nodes[nodeCount];
		bzero(nodes, sizeof(NODE) * nodeCount);
		getNodes4Group(nodes, groupID);
		makeNodeInfoResponse(responseMsg, nodes, nodeCount);
	}

	strcat(responseMsg, "</root>");

	fprintf(stderr, "actionGroupNode4Setting END!\n");
	return;
}

void actionGroupQuery(char* responseMsg, char *mac) {

	int groupId = getGroupIDbyMac(mac);
	if (groupId > 0) {
		actionGroupInfo(responseMsg, groupId);
	} else {
		strcpy(responseMsg, "<root></root>");
	}
}



void actionSaveGroup(char paras[][1024], char* responseMsg, int iCount) {
	char* actionPara = NULL;
	int groupID = 0;
	char* groupName = NULL;
	char* memo = NULL;
	int kr = 0, kb = 0;
	//char* token = NULL;
	int i;
	for (i = 0; i < iCount; i++) {
		if (strstr(paras[i], "AREAID") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			groupID = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "AREANAME") != NULL ) {
			groupName = paras[i];
			strsep(&groupName, "=");
			continue;
		}
		if (strstr(paras[i], "MEMO") != NULL ) {
			memo = paras[i];
			strsep(&memo, "=");
			continue;
		}
		if (strstr(paras[i], "PHOTOBLUE") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			kb = atoi(actionPara);
			continue;
		}
		if (strstr(paras[i], "PHOTORED") != NULL ) {
			actionPara = paras[i];
			strsep(&actionPara, "=");
			kr = atoi(actionPara);
			fprintf(stderr, "KR=%d", kr);
			continue;
		}

	}

	SHAREMEM* shr_mem = get_shm_Memory(TRUE);
	/*AREAINFO *pGroup = shr_mem->p_Group;

	if (groupID == -1) { //New Group
		groupID = insertNewGroup(groupName, memo, kr, kb);
		// refresh memory
		AREAINFO group;
		bzero(&group, sizeof(AREAINFO));
		group.id = groupID;
		strcpy(group.name, groupName);
		strcpy(group.memo, memo);
		group.Kr = kr;
		group.Kb = kb;

		pGroup += shr_mem->groupPlaceholder;
		memcpy(pGroup, &group, sizeof(AREAINFO));
		shr_mem->groupPlaceholder++;

	} else {
		fprintf(stderr, "Update group %d\n", groupID);
		updateGroup(groupID, groupName, memo, kr, kb);
		for (i = 0; i < shr_mem->groupPlaceholder; i++) {
			if (pGroup->id == groupID) {
				strcpy(pGroup->name, groupName);
				strcpy(pGroup->memo, memo);
				pGroup->Kr = kr;
				pGroup->Kb = kb;
				break;
			}
			pGroup++;
		}
	}
	*/
	release_shm_Memory(shr_mem, TRUE);
	makeReturnMsg(responseMsg, groupID, "Add group successes.");
}

void actionAddGroupNode(char paras[][1024], char* responseMsg, int iCount) {
	int groupID = 0;
	char* nodeIDs = NULL;
	char *strGroupID;
	int i;
	for (i = 0; i < iCount; i++) {
		if (strstr(paras[i], "AREAID") != NULL ) {
			strGroupID = paras[i];
			strsep(&strGroupID, "=");
			groupID = atoi(strGroupID);
			continue;
		}
		if (strstr(paras[i], "NODE") != NULL ) {
			nodeIDs = paras[i];
			strsep(&nodeIDs, "=");
			continue;
		}
	}
	if (nodeIDs[strlen(nodeIDs) - 1] == ';') {
		nodeIDs[strlen(nodeIDs) - 1] = '\0';
	}
	addGroupNodes(responseMsg, groupID, nodeIDs);
}

void addGroupNodes(char* responseMsg, int groupID, char* nodeIDs) {
	fprintf(stderr, "addGroupNodes Action... \n");
	char* token;

	deleteGroupNode(groupID);

	while (nodeIDs != NULL )       // divide the protocol head in blank */
	{
		token = strsep(&nodeIDs, ";");
		updateGroupNode(groupID, token);

	}
	updateGroupNodeCount();
	makeReturnMsg(responseMsg, groupID, "Add node successes.");

	fprintf(stderr, "addGroupNodes Action...END \n");

}

