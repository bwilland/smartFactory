/*
 * home.c
 *
 *  Created on: Aug 7, 2013
 *      Author: root
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "home.h"
#include "node.h"
#include "area.h"
#include "../shared/sharemem.h"

char* token;
char* actionPara;

void homeManage(char* request, char* responseMsg) {

	char* p_Paras;
	p_Paras = request;
	token = strsep(&p_Paras, "?");
	char paras[10][128];
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

	for (i = 0; i < iCount; i++) {
		if (strstr(paras[i], "METHOD") != NULL ) {
			actionPara = paras[i];
			token = strsep(&actionPara, "=");
			fprintf(stderr, "METHOD==%s\n", actionPara);
			break;
		}
	}
	if (strcmp(actionPara, "HomeAll") == 0) {
		int pageNo = 1;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "PAGENO") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				pageNo = atoi(actionPara);
				break;
			}
		}
		//fprintf(stderr, "pageNo==%d \n", pageNo);
		actionHome(responseMsg, pageNo);
		return;
	}
	if (strcmp(actionPara, "LogQuery") == 0) {

		int startTime = 0, endTime = 0, areaId = 0, pageNo = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "STARTTIME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				startTime = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "ENDTIME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				endTime = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "AREAID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				areaId = atoi(actionPara);
				continue;
			}
			if (strstr(paras[i], "PAGENO") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				pageNo = atoi(actionPara);
				continue;
			}
		}

		/*	fprintf(stderr,"startTime, endTime, areaId, pageNo::%d,%d,%d,%d\n", startTime,
		 endTime, areaId, pageNo);*/

		actionLogQuery(responseMsg, startTime, endTime, areaId, pageNo);
		return;
	}
	if (strcmp(actionPara, "LogLoginQuery") == 0) {

			int startTime = 0, endTime = 0, pageNo = 0;
			char userName[64];
			for (i = 0; i < iCount; i++) {
				if (strstr(paras[i], "STARTTIME") != NULL ) {
					actionPara = paras[i];
					token = strsep(&actionPara, "=");
					startTime = atoi(actionPara);
					continue;
				}
				if (strstr(paras[i], "ENDTIME") != NULL ) {
					actionPara = paras[i];
					token = strsep(&actionPara, "=");
					endTime = atoi(actionPara);
					continue;
				}
				if (strstr(paras[i], "USERID") != NULL ) {
					actionPara = paras[i];
					token = strsep(&actionPara, "=");
					strcpy(userName,actionPara);
					continue;
				}
				if (strstr(paras[i], "PAGENO") != NULL ) {
					actionPara = paras[i];
					token = strsep(&actionPara, "=");
					pageNo = atoi(actionPara);
					continue;
				}
			}

//				fprintf(stderr,"startTime, endTime, userName, pageNo::%d,%d,%s,%d\n", startTime,
//			 endTime, userName, pageNo);

			actionLogLoginQuery(responseMsg, startTime, endTime, userName, pageNo);
			return;
		}
	if (strcmp(actionPara, "LogDetail") == 0) {
		int logId = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "LOGID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				logId = atoi(actionPara);
				break;
			}
		}
		actionLogDetail(responseMsg, logId);
	}

	//------------login--------------------
	if (strcmp(actionPara, "Login") == 0) {

		char userName[20], password[20];
		char clientIp[32];
		int loginTime = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "USERNAME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(userName, actionPara);
				continue;
			}
			if (strstr(paras[i], "PASSWORD") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(password, actionPara);
				continue;
			}
			if (strstr(paras[i], "CLIENTIP") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(clientIp, actionPara);
				continue;
			}
			if (strstr(paras[i], "LOGINTIME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				loginTime = atoi(actionPara);
				continue;
			}
		}
		/*fprintf(stderr, "login== %s,%s,%d,%s\n", userName, password, loginTime,
		 clientIp);*/

		actionLogin(responseMsg, userName, password, loginTime, clientIp);
		return;
	}
	if (strcmp(actionPara, "ModiPwd") == 0) {

		char userName[20], password[20], newPwd[20];
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "USERNAME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(userName, actionPara);
				continue;
			}
			if (strstr(paras[i], "PASSWORD") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(password, actionPara);
				continue;
			}
			if (strstr(paras[i], "NEWPWD") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(newPwd, actionPara);
				continue;
			}
		}
		actionModiPwd(responseMsg, userName, password, newPwd);
		return;
	}

	if (strcmp(actionPara, "LogOut") == 0) {
		char clientIp[32];
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "CLIENTIP") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(clientIp, actionPara);
				break;
			}
		}
		actionLogOut(responseMsg, clientIp);
		return;
	}

	if (strcmp(actionPara, "SaveUser") == 0) {

		char userName[64], password[64];
		int role = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "USERNAME") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(userName, actionPara);
				continue;
			}
			if (strstr(paras[i], "USERPWD") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				strcpy(password, actionPara);
				continue;
			}

			if (strstr(paras[i], "ROLE") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				role = atoi(actionPara);
				continue;
			}
		}

		actionSaveUser(responseMsg, userName, password, role);
		return;
	}
	if (strcmp(actionPara, "AllUserList") == 0) {

		actionAllUser(responseMsg);
		return;
	}

	if (strcmp(actionPara, "UserInfo") == 0) {
		int userID = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "USERID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				userID = atoi(actionPara);
				break;
			}
		}
		actionUserInfo(responseMsg, userID);
		return;
	}

	if (strcmp(actionPara, "DeleteUser") == 0) {
		int userID = 0;
		for (i = 0; i < iCount; i++) {
			if (strstr(paras[i], "USERID") != NULL ) {
				actionPara = paras[i];
				token = strsep(&actionPara, "=");
				userID = atoi(actionPara);
				break;
			}
		}
		actionDeleteUser(responseMsg, userID);
		return;
	}

}
void actionDeleteUser(char* responseMsg, int userID) {
	deleteUser(userID);
	makeReturnMsg(responseMsg, 1, "Save user OK.");
}
void actionUserInfo(char* responseMsg, int userID) {
	strcpy(responseMsg, "<root>");
	USER user;
	getUserInfo(&user, userID);
	char tmp[256];
	sprintf(tmp,
			"<User><userId>%d</userId><userName>%s</userName><userPwd>%s</userPwd><role>%d</role></User>",
			user.userId, user.userName, user.userPwd, user.role);
	strcat(responseMsg, tmp);
	strcat(responseMsg, "</root>");
}
void actionAllUser(char* responseMsg) {
	strcpy(responseMsg, "<root>");
	int iCount = getResultCount("USERS", NULL );

	if (iCount > 0) {
		USER user[iCount];
		getAllUsers(user);
		USER *p_user = user;
		char tmp[256];
		int i = 0;
		for (i = 0; i < iCount; i++) {
			sprintf(tmp,
					"<User><userId>%d</userId><userName>%s</userName><userPwd>%s</userPwd><role>%d</role></User>",
					p_user->userId, p_user->userName, p_user->userPwd,
					p_user->role);
			strcat(responseMsg, tmp);
			p_user++;

		}
	}
	strcat(responseMsg, "</root>");
	//fprintf(stderr, "responseMsg==%s \n", responseMsg);
}
void actionSaveUser(char* responseMsg, char* userName, char* password, int role) {
	char clause[128];
	sprintf(clause, "USERNAME = '%s'", userName);
	int userCount = getResultCount("USERS", clause);
	if (userCount == 0) {
		saveUser(userName, password, role);
	} else {
		updateUser(userName, password, role);
	}
	makeReturnMsg(responseMsg, 1, "Save user OK.");
}

void actionHome(char* responseMsg, int pageNo) {
	char tmp[512];
	strcpy(responseMsg, "<root>");
	SHAREMEM* shr_mem = get_shm_Memory(FALSE);
	AREAINFO* p_area = shr_mem->p_Area;
	int i = 0, iCount = 6;
	int areaCount = shr_mem->areaPlaceholder;
	/*	fprintf(stderr, "shr_mem->areaPlaceholder==%d \n",
	 shr_mem->areaPlaceholder);*/
	if (areaCount > 0) {
		if (areaCount < (6 * pageNo)) { //一页不满
			iCount = areaCount - 6 * (pageNo - 1);
			makePageInfoResponse(responseMsg, pageNo, pageNo);
		} else {
			int maxPage = areaCount / 6;
			if (areaCount % 6 > 0) {
				maxPage++;
			}
			makePageInfoResponse(responseMsg, maxPage, pageNo);
		}
		p_area += 6 * (pageNo - 1);
		for (i = 0; i < iCount; i++) {
			sprintf(tmp,
					"<domain><id>%d</id><name>%s</name><optionType>%s</optionType><modelName>%s</modelName><onoffST>%d</onoffST><PPF1>%d</PPF1><PPF2>%d</PPF2><PPF3>%d</PPF3><PPF4>%d</PPF4></domain>",
					p_area->id, p_area->name,
					(p_area->optionType == 0 ? "Manu" : "Auto"),
					p_area->modeName, p_area->onoffST, p_area->ppf1,
					p_area->ppf2, p_area->ppf3, p_area->ppf4);

			strcat(responseMsg, tmp);
			p_area++;

		}
	} else {
		makePageInfoResponse(responseMsg, 1, 1);
	}
	strcat(responseMsg, "</root>");

	//fprintf(stderr,"responseMsg==%s \n", responseMsg);
	release_shm_Memory(shr_mem, FALSE);
}


void actionLogLoginQuery(char* responseMsg, int startTime, int endTime, char* userName,
		int pageNo) {

	strcpy(responseMsg, "<root>");
	char tmp[256];
	int i = 0;
	if (strlen(userName) != 0) {
		sprintf(tmp, "(LOGTIME BETWEEN %d AND %d) AND USERNAME ='%s'", startTime,
				endTime, userName);
	} else {
		sprintf(tmp, "(LOGTIME BETWEEN %d AND %d)", startTime, endTime);
	}
	int logCount = getResultCount("USERLOG", tmp);
	int iCount = 10;
	//fprintf(stderr,"logCount==%d \n", logCount);

	if (logCount > 0) {

		if (logCount < (10 * pageNo)) { //一页不满
			iCount = logCount - 10 * (pageNo - 1);
			makePageInfoResponse(responseMsg, pageNo, pageNo);
		} else {
			int maxPage = logCount / 10;
			if (logCount % 10 > 0) {
				maxPage++;
			}
			makePageInfoResponse(responseMsg, maxPage, pageNo);
		}
		USERLOG log[iCount];
		getUserLog(log, startTime, endTime, userName, (pageNo - 1) * 10);

		for (i = 0; i < iCount; i++) {
			sprintf(tmp,
					"<log><datetime>%ld</datetime><username>%s</username><ip>%s</ip><result>%s</result></log>",
					log[i].logTime, log[i].userName,
					log[i].IP, log[i].logResult);

			strcat(responseMsg, tmp);

		}

	} else {
		makePageInfoResponse(responseMsg, 1, 1);
	}
	strcat(responseMsg, "</root>");
	fprintf(stderr, "responseMsg==%s \n", responseMsg);
}

void actionLogQuery(char* responseMsg, int startTime, int endTime, int areaId,
		int pageNo) {

	strcpy(responseMsg, "<root>");
	char tmp[256];
	int i = 0;
	if (areaId != 0) {
		sprintf(tmp, "(DATETIME BETWEEN %d AND %d) AND AREAID =%d", startTime,
				endTime, areaId);
	} else {
		sprintf(tmp, "(DATETIME BETWEEN %d AND %d)", startTime, endTime);
	}
	int logCount = getResultCount("COMMLOG", tmp);
	int iCount = 10;
	//fprintf(stderr,"logCount==%d \n", logCount);

	if (logCount > 0) {

		if (logCount < (10 * pageNo)) { //一页不满
			iCount = logCount - 10 * (pageNo - 1);
			makePageInfoResponse(responseMsg, pageNo, pageNo);
		} else {
			int maxPage = logCount / 10;
			if (logCount % 10 > 0) {
				maxPage++;
			}
			makePageInfoResponse(responseMsg, maxPage, pageNo);
		}
		COMMLOG log[iCount];
		getCommLog(log, startTime, endTime, areaId, (pageNo - 1) * 10);

		for (i = 0; i < iCount; i++) {
			sprintf(tmp,
					"<log><id>%d</id><datetime>%ld</datetime><modelname>%s</modelname><areaname>%s</areaname><PPF1>%.2f</PPF1><PPF2>%.2f</PPF2><PPF3>%.2f</PPF3><PPF4>%.2f</PPF4><optiontype>%s</optiontype></log>",
					log[i].id, log[i].dateTime, log[i].modeName,
					log[i].areaName, log[i].PPF1, log[i].PPF2, log[i].PPF3,
					log[i].PPF4, (log[i].optionType == 0 ? "Manu" : "Auto"));

			strcat(responseMsg, tmp);

		}

	} else {
		makePageInfoResponse(responseMsg, 1, 1);
	}
	strcat(responseMsg, "</root>");
	//fprintf(stderr, "responseMsg==%s \n", responseMsg);
}
void actionLogDetail(char* responseMsg, int logId) {
	strcpy(responseMsg, "<root>");
	char tmp[512];
	int during = 0;
	COMMLOG commlog;
	getCommLogDetail(&commlog, logId);
	if (commlog.endTime > 0) {
		during = commlog.endTime - commlog.dateTime - commlog.timeStoped;
	} else {
		time_t timeNow;
		time(&timeNow);
		during = timeNow - commlog.dateTime - commlog.timeStoped;

	}
	if (during < 0) {
		during += SCHEDULEINTERVALS;
	}
	//fprintf(stderr,"during==%d \n", during);

	float ppf1 = (float) (during * commlog.PPF1) / 1000.00;
	float ppf2 = (float) (during * commlog.PPF2) / 1000.00;
	float ppf3 = (float) (during * commlog.PPF3) / 1000.00;
	float ppf4 = (float) (during * commlog.PPF4) / 1000.00;

	sprintf(tmp,
			"<log><datetime>%ld</datetime><modelname>%s</modelname><areaname>%s</areaname><appf1>%.2f</appf1><appf2>%.2f</appf2><appf3>%.2f</appf3><appf4>%.2f</appf4><PPF1>%.2f</PPF1><PPF2>%.2f</PPF2><PPF3>%.2f</PPF3><PPF4>%.2f</PPF4><optiontype>%s</optiontype></log>",
			commlog.dateTime, commlog.modeName, commlog.areaName, ppf1, ppf2,
			ppf3, ppf4, commlog.PPF1, commlog.PPF2, commlog.PPF3, commlog.PPF4,
			(commlog.optionType == 0 ? "Manu" : "Auto"));

	strcat(responseMsg, tmp);
	strcat(responseMsg, "</root>");
}

void actionLogin(char* responseMsg, char* userName, char* password,
		int loginTime, char* clientIp) {

	char pwdDB[20];
	bzero(pwdDB, 20);
	int role = 0;
	loginDB(userName, pwdDB, &role);

	time_t t_now;
	time(&t_now);

	USERLOG userLog;
	bzero(&userLog, sizeof(USERLOG));
	strcpy(userLog.userName, userName);
	userLog.logTime = t_now;
	strcpy(userLog.IP, clientIp);

	if (strcmp(password, pwdDB) == 0) {
		SHAREUSER* shr_user = get_shm_User();
		USERLOGIN *p_user = shr_user->p_User;
		int i = 0;
		time_t timeNow;
		time(&timeNow);
		//fprintf(stderr,"shr_user->userPlaceholder==%d\n",shr_user->userPlaceholder);
		for (i = 0; i < shr_user->userPlaceholder; i++) {
			//fprintf(stderr,"clientIp==%d\n", p_user->clientIp);
			if (p_user->isLogin) {
				if ((timeNow - p_user->lastLogin) > LOGINTIMEOVER) {
					//超时
					p_user->isLogin = FALSE;
					p_user->lastLogin = 0;
					p_user->role = -1;
					strcpy(p_user->clientIp, "");
					//shr_mem->userPlaceholder--;
				}
			}
			p_user++;
		}
//fprintf(stderr,"shr_user->userPlaceholder==%d\n",shr_user->userPlaceholder);
		p_user = shr_user->p_User;
		BOOL isFilled = FALSE;
		for (i = 0; i < shr_user->userPlaceholder; i++) {
			//printf("clientIp==%d\n", p_user->clientIp);
			if (strlen(p_user->clientIp) == 0) {
				strcpy(p_user->clientIp, clientIp);
				p_user->isLogin = TRUE;
				p_user->role = role;
				time_t timeNow;
				time(&timeNow);
				p_user->lastLogin = timeNow;
				isFilled = TRUE;
				break;
			}
			p_user++;
		}

		//fprintf(stderr,"shr_user->userPlaceholder==%d\n",shr_user->userPlaceholder);
		if (!isFilled) {
			strcpy(p_user->clientIp, clientIp);
			p_user->isLogin = TRUE;
			p_user->role = role;
			time_t timeNow;
			time(&timeNow);
			p_user->lastLogin = timeNow;
			shr_user->userPlaceholder++;

		}

		p_user = shr_user->p_User;
		for (i = 0; i < shr_user->userPlaceholder; i++) {
			//fprintf(stderr, "logined==  %s, %d, %d\n", p_user->clientIp,
			//p_user->isLogin, p_user->lastLogin);
			p_user++;
		}

		//检测时间差距

		if (loginTime - t_now > 3600) {
			makeReturnMsg(responseMsg, 2, "Login OK.but need check time.");
			strcpy(userLog.logResult, "Login OK.but time ERROR.");
		} else {
			makeReturnMsg(responseMsg, 1, "Login OK.");
			strcpy(userLog.logResult, "Login OK.");
		}
		userLog.isSucceed = 1;

		release_shm_User(shr_user);
	} else {
		makeReturnMsg(responseMsg, 0, "Login failed.");
		strcpy(userLog.logResult, "Login failed.");
		userLog.isSucceed = 0;
	}
	logActionLog(&userLog);

}

void logActionLog(USERLOG* userLog) {
	writeUserLog(userLog);
}
void actionLogOut(char* responseMsg, char* clientIp) {
	SHAREUSER* shr_user = get_shm_User();
	USERLOGIN *p_user = shr_user->p_User;
	int i = 0;
	for (i = 0; i < shr_user->userPlaceholder; i++) {
		if (strcmp(p_user->clientIp, clientIp) == 0) {
			p_user->isLogin = FALSE;
			p_user->lastLogin = 0;
			strcpy(p_user->clientIp, "");
			//shr_mem->userPlaceholder--;
			break;
		}
		p_user++;
	}
	makeReturnMsg(responseMsg, 1, "Logout OK.");

	release_shm_User(shr_user);
}

void actionModiPwd(char* responseMsg, char* userName, char* password,
		char* newPwd) {
	char pwdDB[20];
	bzero(pwdDB, 20);
	int role = 0;
	loginDB(userName, pwdDB, &role);
	if (strcmp(password, pwdDB) == 0) {
		updateDB(userName, newPwd);
		makeReturnMsg(responseMsg, 1, "modify OK.");
	} else {
		makeReturnMsg(responseMsg, 0, "Login failed.");
	}

}

