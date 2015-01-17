#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include "../sqlite3.h"
#include "dbadapter.h"
#include "lightingpack.h"
#include "sharemem.h"
#define SQLEN 512

char sql[SQLEN];
sqlite3 *db;
char *zErrMsg = 0;
int nrow = 0, ncolumn = 0;
char **azResult;
int ret;

char DB[] = "/smartFactory/factory.db";

//char DB[] = "/smart/plant.db";

void openDB() {
	//SHAREMEM* shr_men = get_shm_Memory();
	//while(shr_men->dbLocked){
	//	fprintf(stderr,"locked \n");
	//	usleep(100000);
	//}
	//shr_men->dbLocked = TRUE;
	//fprintf(stderr,"locking \n");
	sqlite3_open(DB, &db);
	//release_shm_Memory(shr_men);
}

void closeDB() {
	//SHAREMEM* shr_men = get_shm_Memory();
	sqlite3_close(db);
	//shr_men->dbLocked = FALSE;
	//fprintf(stderr,"unlocked \n");
	//release_shm_Memory(shr_men);
}

/**
 查询表中记录条数 count(*)
 */
int getResultCount(char* table, char* clause) {
	int i_count = 0;
	strcpy(sql, "SELECT COUNT(*) FROM ");
	strcat(sql, table);
	if (clause != NULL ) {
		strcat(sql, " WHERE ");
		strcat(sql, clause);
	}
	strcat(sql, ";");

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		i_count = atoi(azResult[1]);
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return i_count;
}

//=========Group=========================
int getGroupInfo(GROUP *group) {

	sprintf(sql,
			"SELECT GROUPID ,NAME,ROOMSENSORMAC ,DAYNIGHTSENSORMAC FROM GROUPS WHERE GROUPID=%d",
			group->id);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i;
		for (i = 1; i < nrow + 1; i++) {
			//group->id = atoi(azResult[i * ncolumn]);
			strcpy(group->name, azResult[i * ncolumn + 1]);
			strcpy(group->roomSensorMAC, azResult[i * ncolumn + 2]);
			strcpy(group->dayNightSensorMAC, azResult[i * ncolumn + 3]);

		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int getAllGroup(GROUPAUTODIMMER* groups) {

	sprintf(sql,
			"SELECT  GROUPID ,NAME,ROOMSENSORMAC ,DAYNIGHTSENSORMAC FROM GROUPS ORDER BY GROUPID;");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i;
		for (i = 1; i < nrow + 1; i++) {
			groups->groupID = atoi(azResult[i * ncolumn]);
			strcpy(groups->name, azResult[i * ncolumn + 1]);
			strcpy(groups->roomSensorMAC, azResult[i * ncolumn + 2]);
			strcpy(groups->daynightSensorMAC, azResult[i * ncolumn + 3]);

			groups++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int getProgramList(PROGRAM* p_program, char* status) {
	sprintf(sql,
			"SELECT PROGRAMID, NAME,STARTTIME,ENDTIME,STATUS ,GROUPLIST ,DIMMER ,SENSORLEVEL,ISLOOP ,WEEK,ROOMSENSOR,DAYLIGHTSENESOR ,MOTION ,MOTIONDELAYTIME ,MOTIONDIMMER ,MOTIONSECUREALARM FROM PROGRAM WHERE STATUS IN(%s) ORDER BY PROGRAMID;",
			status);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i = 1;
		for (i = 1; i < nrow + 1; i++) {
			p_program->id = atoi(azResult[i * ncolumn]);
			strcpy(p_program->name, azResult[i * ncolumn + 1]);
			strcpy(p_program->startTime, azResult[i * ncolumn + 2]);
			strcpy(p_program->endTime, azResult[i * ncolumn + 3]);
			//p_program->status = atoi(azResult[i * ncolumn + 3]);

			p_program->status = TASKWAIT;

			strcpy(p_program->groupList, azResult[i * ncolumn + 5]);

			p_program->dimmer = atoi(azResult[i * ncolumn + 6]);
			p_program->sensorLevel = atoi(azResult[i * ncolumn + 7]);
			p_program->isLoop = atoi(azResult[i * ncolumn + 8]);

			strcpy(p_program->week, azResult[i * ncolumn + 9]);

			p_program->roomSensor = atoi(azResult[i * ncolumn + 10]);
			p_program->DaylightSenesor = atoi(azResult[i * ncolumn + 11]);
			p_program->motion = atoi(azResult[i * ncolumn + 12]);
			p_program->motionDelayTime = atoi(azResult[i * ncolumn + 13]);
			p_program->MotionDimmer = atoi(azResult[i * ncolumn + 14]);
			p_program->MotionSecureAlarm = atoi(azResult[i * ncolumn + 15]);

			p_program++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}

int saveProgram(PROGRAM* program) {

	char sql[512];
	openDB();
	sprintf(sql, "SELECT COUNT(*) as iCount FROM PROGRAM WHERE PROGRAMID=%d",
			program->id);
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
//	fprintf(stderr, "sql==%s \n", sql);
	if (ret == SQLITE_OK) {
		int iCount = atoi(azResult[1]);
		//fprintf(stderr, "iCount==%d \n", iCount);
		if (iCount == 0) {
			//new program
			sprintf(sql,
					"INSERT INTO PROGRAM(PROGRAMID,NAME,STARTTIME,ENDTIME,STATUS,GROUPLIST,DIMMER,SENSORLEVEL,ISLOOP,WEEK,ROOMSENSOR,DAYLIGHTSENESOR,MOTION,MOTIONDELAYTIME,MOTIONDIMMER,MOTIONROOMSENSOR,MOTIONSECUREALARM) values(%d,'%s','%s','%s',%d,'%s',%d,%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d)",
					program->id, program->name, program->startTime,
					program->endTime, program->status, program->groupList,
					program->dimmer, program->sensorLevel, program->isLoop,
					program->week, program->roomSensor,
					program->DaylightSenesor, program->motion,
					program->motionDelayTime, program->MotionDimmer,
					program->MotionRoomSensor, program->MotionSecureAlarm);
		} else {
			sprintf(sql,
					" UPDATE PROGRAM SET NAME='%s', STARTTIME='%s',ENDTIME='%s',STATUS=%d,GROUPLIST='%s',DIMMER=%d,SENSORLEVEL=%d,ISLOOP=%d,WEEK='%s',ROOMSENSOR=%d,DAYLIGHTSENESOR=%d,MOTION=%d,MOTIONDELAYTIME=%d,MOTIONDIMMER=%d,MOTIONROOMSENSOR=%d,MOTIONSECUREALARM=%d WHERE PROGRAMID=%d",
					program->name, program->startTime, program->endTime,
					program->status, program->groupList, program->dimmer,
					program->sensorLevel, program->isLoop, program->week,
					program->roomSensor, program->DaylightSenesor,
					program->motion, program->motionDelayTime,
					program->MotionDimmer, program->MotionRoomSensor,
					program->MotionSecureAlarm, program->id);
		}

		ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
		//fprintf(stderr, "sql==%s \n", sql);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "sql==%s \n", sql);
			fprintf(stderr, "error: %s \n", zErrMsg);
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;

}

int deleteProgram(PROGRAM* program) {

	char sql[512];
	openDB();
	sprintf(sql, "delete FROM PROGRAM WHERE PROGRAMID=%d", program->id);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	closeDB(); //关闭数据库
	return ret;

}

int getSensorLevel(int sensorType, int sensorDimmer, int* iDimmer) {

	sprintf(sql,
			"SELECT DIMMER FROM SENSORLEVEL WHERE MINDIMMER <= %d AND MAXDIMMER >=%d AND SENSORTYPE=%d;",
			sensorDimmer, sensorDimmer, sensorType);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		*iDimmer = atoi(azResult[ncolumn]);

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}

//**************************************************************************
int getNodeBygroupId(NODE* pNode, int groupId) {
	int i = 0;
	sprintf(sql,
			"SELECT ROWID,MAC,MEMO,AREA FROM NODES WHERE AREA=%d AND ISVALID=1",
			groupId);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			pNode->id = atoi(azResult[i * ncolumn]);
			strcpy(pNode->mac, azResult[i * ncolumn + 1]);
			strcpy(pNode->memo, azResult[i * ncolumn + 2]);
			pNode->groupId = atoi(azResult[i * ncolumn + 3]);
			pNode->isValid = TRUE;
			pNode++;
		}
	} else {
		nrow = 0;
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return nrow;
}

int getNodesMacs(char mac[][9], int offset) {
	int i = 0;
	nrow = 0;
	sprintf(sql,
			"SELECT MAC FROM NODES WHERE ISVALID=1 ORDER BY ROWID LIMIT 10 OFFSET %d;",
			offset);

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			strcpy(mac[i - 1], azResult[i * ncolumn]);
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return nrow;
}

int insertNewArea(char* groupName, char* memo, int kr, int kb) {
	memset(sql, 0, SQLEN);
	sprintf(sql,
			"INSERT INTO AREA(NAME,MEMO,KR,KB,ORDERID,NODECOUNT) VALUES('%s','%s',%d,%d,0,0);",
			groupName, memo, kr, kb);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	int rowID = sqlite3_last_insert_rowid(db);
	//fprintf(stderr,"rowID === %d\n",rowID);

	closeDB(); //关闭数据库

	return rowID;

}

int deleteArea(int groupId) {
	memset(sql, 0, SQLEN);
	openDB();
	sprintf(sql, "DELETE FROM AREA  WHERE ROWID=%d;", groupId);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int deleteAreaNode(int groupId) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE NODES SET AREA = 0  WHERE AREA=%d;", groupId);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

//=========Node=======================

int getDBNodeInfo(NODE* pNodes, int nodeID) {

	sprintf(sql, "SELECT ROWID,MAC,MEMO,AREA,ISVALID FROM NODES WHERE ROWID=%d",
			nodeID);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			pNodes->id = atoi(azResult[ncolumn]);
			strcpy(pNodes->mac, azResult[ncolumn + 1]);
			strcpy(pNodes->memo, azResult[ncolumn + 2]);
			pNodes->groupId = atoi(azResult[ncolumn + 3]);
			pNodes->isValid = atoi(azResult[ncolumn + 4]);
		} else {
			ret = -1; //no result
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	//fprintf(stderr,"ret== %d \n", ret);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int getAllNodeList(NODE* pNodes) {
	int i = 0;
	strcpy(sql,
			"SELECT ROWID,MAC TEXT,NODETYPE,MODEL,GROUPID,ISVALID FROM NODES WHERE ISVALID=1;");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) {
			pNodes->id = atoi(azResult[ncolumn]);
			strcpy(pNodes->mac, azResult[ncolumn + 1]);
			pNodes->nodeType = atoi(azResult[ncolumn + 2]);
			pNodes->model = atoi(azResult[ncolumn + 3]);
			pNodes->groupId = atoi(azResult[ncolumn + 4]);
			pNodes->isValid = atoi(azResult[ncolumn + 5]);
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	//fprintf(stderr,"ret== %d \n", ret);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int updateNode(NODE* pNodes) {
	memset(sql, 0, SQLEN);
	//char tmp[50];
	sprintf(sql,
			"UPDATE NODES SET MAC = '%s',MEMO = '%s',AREA=%d WHERE ROWID=%d;",
			pNodes->mac, pNodes->memo, pNodes->groupId, pNodes->id);
	//fprintf(stderr,"sql==%s \n", sql);

	openDB();
//	fprintf(stderr,"sql==%s \n",sql);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}
int getLastNodeId() {

	sprintf(sql, "SELECT ROWID FROM NODES ORDER BY ROWID DESC LIMIT 1;");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			ret = atoi(azResult[ncolumn]);

		} else {
			ret = 0;
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
		ret = 0;
	}
	//fprintf(stderr,"ret== %d \n", ret);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int insertNode(NODE* pNodes) {

	memset(sql, 0, SQLEN);
	sprintf(sql,
			"INSERT INTO NODES(MAC,NODETYPE,MODEL,GROUPID,ISVALID) VALUES('%s',0,0,0,1);",
			pNodes->mac);

	openDB();
	//fprintf(stderr,"sql==%s \n", sql);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	int rowID = sqlite3_last_insert_rowid(db);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
		rowID = -1;
	}
	closeDB(); //关闭数据库
	return rowID;
}

int deleteAllNode() {
	openDB();
	memset(sql, 0, SQLEN);

	sprintf(sql, "DELETE FROM NODES;");
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int deleteNode(char* nodeIDs) {
	openDB();
	memset(sql, 0, SQLEN);

	sprintf(sql, "DELETE FROM NODES  WHERE ROWID = %s;", nodeIDs);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int invalidAllNodes() {
	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE NODES SET ISVALID = 0;");

	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int validNode(char* mac) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE NODES SET ISVALID = 1 WHERE MAC='%s';", mac);

	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

//------------------------LOG---------------------------------------
void writeDBLog(COMMLOG *log) {
	openDB();
	int lastRowid = 0;
	sprintf(sql,
			"SELECT ROWID FROM COMMLOG WHERE GROUPID =%d ORDER BY ROWID DESC LIMIT 1;",
			log->groupId);
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			lastRowid = atoi(azResult[ncolumn]);
		}
	}
	if (lastRowid != 0) {
		sprintf(sql, "UPDATE COMMLOG SET ENDTIME =%ld WHERE ROWID=%d;",
				log->startTime, lastRowid);
		ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "sql==%s \n", sql);
			fprintf(stderr, "error: %s \n", zErrMsg);
		}
	}
	sprintf(sql,
			"INSERT INTO COMMLOG(STARTTIME ,ENDTIME,GROUPID ,DIMMER) VALUES(%ld,%d,%d,%.04f);",
			log->startTime, 0, log->groupId, ((float) log->dimmer / 1000));

	//	fprintf(stderr,"sql==%s \n",sql);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return;
}

int getEnergeCost(ENERGECOST* cost, int startTime, int endTime) {
	int i = 0;
	sprintf(sql,
			"SELECT GROUPID, SUM((ENDTIME - STARTTIME)*DIMMER/10) AS COST  FROM COMMLOG WHERE ENDTIME BETWEEN %d AND %d GROUP BY GROUPID",
			startTime, endTime);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) {
			cost->groupID = atoi(azResult[i * ncolumn]);
			cost->costValue = atol(azResult[i * ncolumn + 1]);

			cost++;
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}


int modifyLogs(int timeStoped) {
	sprintf(sql, "UPDATE COMMLOG SET TIMESTOPED=%d WHERE ENDTIME=0;",
			timeStoped);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

