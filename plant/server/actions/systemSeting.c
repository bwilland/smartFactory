#include <unistd.h>
#include <wait.h>

#include "systemSeting.h"
#include "node.h"
#include "shared/sharemem.h"

char* token;
char* actionPara;

void systemManage(char* request, char* responseMsg) {

	char* p_Paras;
	p_Paras = request;
	token = strsep(&p_Paras, "?");
	char paras[10][128];
	int i = 0, iCount = 0;
	int resendTimes = 0, isBroasdcast = 0, isSendConfirm = 0;

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
	if (strcmp(actionPara, "SystemUpdate") == 0) {
		int isW3 = 0;
		char url[128];
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "ISW3UPDATE") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				isW3 = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "URL") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(url, actionPara);
				continue;
			}
		}
		actionSystemUpdate(responseMsg, isW3, url);
		fprintf(stderr, "%s\n", responseMsg);
		return;
	}
	if (strcmp(actionPara, "RebootServer") == 0) {
		int rt = system("sudo reboot");
		if (rt == -1) {
			makeReturnMsg(responseMsg, -1, "reboot system failed.");
		} else {
			makeReturnMsg(responseMsg, 1, "reboot system OK.");
		}
		return;
	}

	if (strcmp(actionPara, "BackupDB") == 0) {
		char cmd[64], serverDate[64], dbName[64];
		time_t now;    //实例化time_t结构
		struct tm *timenow;    //实例化tm结构指针
		time(&now);
		timenow = localtime(&now);
		sprintf(dbName, "plant.bak.%d%02d%02d%02d%02d%02d",
				timenow->tm_year + 1900, timenow->tm_mon + 1, timenow->tm_mday,
				timenow->tm_hour, timenow->tm_min, timenow->tm_sec);

		sprintf(cmd, "sudo cp plant.db ./dbBack/%s", dbName);

		int rt = system(cmd);
		if (rt == -1) {
			makeReturnMsg(responseMsg, -1, "back db failed.");
		} else {
			makeReturnMsg(responseMsg, 1, "back db OK.");
			sprintf(serverDate, "back %d-%02d-%02d %02d:%02d:%02d",
					timenow->tm_year + 1900, timenow->tm_mon + 1,
					timenow->tm_mday, timenow->tm_hour, timenow->tm_min,
					timenow->tm_sec);
			insertBackDB(dbName, serverDate);

		}
		return;
	}

	if (strcmp(actionPara, "GetRstoreDB") == 0) {
		strcpy(responseMsg, "<root>");
//		/char clause[64];
		//sprintf(clause, "TYPE=0");
		int nodeCount = getResultCount("BACKDB", NULL );

		BACKDB dbs[nodeCount];
		getBackDBList(dbs,"DESC");
		makeDBInfoResponse(responseMsg, dbs, nodeCount);

		strcat(responseMsg, "</root>");
		//fprintf(stderr,"%s",responseMsg);
	}

	if (strcmp(actionPara, "RestoreDB") == 0) {

		char cmd[64], dbName[64];
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "DBNAME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(dbName, actionPara);
				break;
			}
		}

		//back dbback recorder
		int recordCount = getResultCount("BACKDB", NULL );
		BACKDB dbs[recordCount];
		getBackDBList(dbs,"ASC");

		sprintf(cmd, "sudo cp ./dbBack/%s plant.db", dbName);
		fprintf(stderr, "cmd==%s\n", cmd);

		int rt = system(cmd);
		if (rt == -1) {
			makeReturnMsg(responseMsg, -1, "restore db failed.");
		} else {
			makeReturnMsg(responseMsg, 1, "restore db OK.");

		}

		//restore dbback recorder
		setBackDBList(dbs,recordCount);
		return;
	}

	if (strcmp(actionPara, "SaveSendSetting") == 0) {
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "TIMES") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				resendTimes = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "BROADCAST") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				isBroasdcast = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "SENDCONFIRM") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				isSendConfirm = atoi(actionPara);
				continue;
			}
		}
		//fprintf(stderr, "resendTimes==%d\n", resendTimes);
		actionSaveSendSetting(responseMsg, resendTimes, isBroasdcast,
				isSendConfirm);
		return;
	}
	if (strcmp(actionPara, "getSendSetting") == 0) {

		actionGetSendSetting(responseMsg);
		return;
	}
	if (strcmp(actionPara, "GetSysDate") == 0) {
		actionGetSystemDate(responseMsg);
		return;
	}
	if (strcmp(actionPara, "setSysDate") == 0) {
		int dt = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "DATETIME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				dt = atoi(actionPara);
				break;
			}
		}
		actionSetSystemDate(responseMsg, dt);
		return;
	}
	if (strcmp(actionPara, "GetSysDate") == 0) {
		actionGetSystemDate(responseMsg);
		return;
	}
	if (strcmp(actionPara, "setSysDate") == 0) {
		int dt = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "DATETIME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				dt = atoi(actionPara);
				break;
			}
		}
		actionSetSystemDate(responseMsg, dt);
		return;
	}

	if (strcmp(actionPara, "SaveServerName") == 0) {
		char serverName[64];
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "SERVERNAME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(serverName, actionPara);
				break;
			}
		}
		saveServerName(responseMsg, serverName);
	}
	if (strcmp(actionPara, "GetServerName") == 0) {
		actionGetServerName(responseMsg);

	}
}

void actionSystemUpdate(char* responseMsg, int isW3, char* url) {

	fprintf(stderr, "isW3==%d,url==%s\n", isW3, url);
	int returnCode = 0;

	if (isW3) {
		char cmd[218];
		sprintf(cmd, "/plant/w3update.sh %s", url);
		returnCode = system(cmd);
		if (access("/plant/update.tar.gz", F_OK) == -1) {
			makeReturnMsg(responseMsg, -2, "download update file failed.");
			return;
		}
	} else {
		if (access("/plant/update.tar.gz", F_OK) == -1) {
			makeReturnMsg(responseMsg, -3,
					"could not find update.tar.gz in /plant directory!.");
			return;
		}
	}

	returnCode = system("/plant/update.sh");

	fprintf(stderr, "returnCode==%d\n", returnCode);
	if (returnCode == -1) {
		perror("update");
		makeReturnMsg(responseMsg, -1, "update system failed.");
		return;
	}

	makeReturnMsg(responseMsg, 1, "update system OK.");

}

void actionGetSendSetting(char* responseMsg) {
	int resendTimes = 0, isBroasdcast = 0, isSendConfirm = 0;
	getSendSetting(&resendTimes, &isBroasdcast, &isSendConfirm);

	sprintf(responseMsg,
			"<root><SendSetting><times>%d</times><broadcast>%d</broadcast><sendConfirm>%d</sendConfirm></SendSetting></root>",
			resendTimes, isBroasdcast, isSendConfirm);
	//fprintf(stderr,"%s \n",responseMsg);
}

void actionSaveSendSetting(char* responseMsg, int resendTimes, int isBroasdcast,
		int isSendConfirm) {
	if (updateSendSetting(resendTimes, isBroasdcast, isSendConfirm) == 0) {
		SHAREMEM* share_mem = get_shm_Memory(FALSE);
		share_mem->cmdSendTimes = resendTimes;
		share_mem->isBroadcast = isBroasdcast;
		share_mem->needSendConfirm = isSendConfirm;
		release_shm_Memory(share_mem, FALSE);
		makeReturnMsg(responseMsg, 1, "save resendTimes successes.");
	} else {
		makeReturnMsg(responseMsg, -1, "save resendTimes failed.");
	}
}

void saveServerName(char* responseMsg, char* serverName) {
	updateServerName(serverName);
	makeReturnMsg(responseMsg, 1, "modify OK.");
}

void actionGetServerName(char* responseMsg) {
	char serverName[64];
	getServerName(serverName);
	strcpy(responseMsg, "<root><server>");
	strcat(responseMsg, "<serverName>");
	strcat(responseMsg, serverName);
	strcat(responseMsg, "</serverName>");
	strcat(responseMsg, "</server></root>");
}

void actionGetSystemDate(char* responseMsg) {

	char serverDate[32];
	time_t now;    //实例化time_t结构
	struct tm *timenow;    //实例化tm结构指针
	time(&now);
	timenow = localtime(&now);

	sprintf(serverDate, "%d,%d,%d,%d,%d,00", timenow->tm_year + 1900,
			timenow->tm_mon, timenow->tm_mday, timenow->tm_hour,
			timenow->tm_min);

	strcpy(responseMsg, "<root><server>");
	strcat(responseMsg, "<serverDate>");
	strcat(responseMsg, serverDate);
	strcat(responseMsg, "</serverDate>");
	strcat(responseMsg, "</server></root>");

}

void actionSetSystemDate(char* responseMsg, int dt) {

	int rt = 0;
	char serverDate[32];
	time_t newTime = dt;    //实例化time_t结构
	struct tm *timenow;    //实例化tm结构指针
	timenow = localtime(&newTime);

	sprintf(serverDate, "date -s %d/%d/%d", timenow->tm_year + 1900,
			timenow->tm_mon + 1, timenow->tm_mday);

	//fprintf(stderr, "%s \n", serverDate);

	rt = system(serverDate);
	if (rt == -1) {
		makeReturnMsg(responseMsg, -1, "setting time Error.");
		return;
	}
	sprintf(serverDate, "date -s %d:%d:01", timenow->tm_hour, timenow->tm_min);

	rt = system(serverDate);
	if (rt == -1) {
		makeReturnMsg(responseMsg, -1, "setting time Error.");
		return;
	}
	//保存到硬件时钟
	char tm[128];
	sprintf(tm, "/plant/linker_rtc %d:%d:%d:%d:%d:%d", timenow->tm_year + 1900,
			timenow->tm_mon + 1, timenow->tm_mday, timenow->tm_hour,
			timenow->tm_min, timenow->tm_sec);
	fprintf(stderr, "%s\n", tm);
	rt = system(tm);
	if (rt == -1) {
		makeReturnMsg(responseMsg, -1, "setting time Error.");
		return;
	}
	makeReturnMsg(responseMsg, 1, "setting time OK.");
}

void makeDBInfoResponse(char* response, BACKDB* dbInfo, int dbCount) {
	char tmp[256];
	int i = 0;
	for (i = 0; i < dbCount; i++) {
		sprintf(tmp, "<backDB><dbDate>%s</dbDate><dbName>%s</dbName></backDB>",
				dbInfo->dbDate, dbInfo->dbName);
		strcat(response, tmp);
		if (dbCount > 1) {
			dbInfo++;
		}
	}
}
