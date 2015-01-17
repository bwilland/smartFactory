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

char DB[] = "/plant/plant.db";

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

/*
 int count_callback(void *arg, int nr, char **values, char **names) {

 *(int*) arg = atoi(values[0]);
 return 0;
 }
 */

//=========Area=========================
int getAllArea(AREAINFO* pArea) {

	sprintf(sql,
			"SELECT ROWID,NAME,MEMO,KR,KB FROM AREA ORDER BY ORDERID,ROWID");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i;
		for (i = 1; i < nrow + 1; i++) {
			pArea->id = atoi(azResult[i * ncolumn]);

			strcpy(pArea->name, azResult[i * ncolumn + 1]);
			strcpy(pArea->memo, azResult[i * ncolumn + 2]);
			pArea->Kr = atoi(azResult[i * ncolumn + 3]);
			pArea->Kb = atoi(azResult[i * ncolumn + 4]);

			fprintf(stderr, "pArea->id==%d \n", pArea->id);
			fprintf(stderr, "pArea->name==%s \n", pArea->name);
			pArea++;

		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int getAllAreaIDs(char* areaIds) {

	sprintf(sql, "SELECT ROWID FROM AREA ORDER BY ORDERID,ROWID;");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i;
		for (i = 1; i < nrow + 1; i++) {
			strcat(areaIds, azResult[i * ncolumn]);
			strcat(areaIds, ",");
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int getAreaList(AREA* pArea) {

	sprintf(sql,
			"SELECT ROWID,NAME,MEMO,NODECOUNT FROM AREA ORDER BY ORDERID,ROWID");
//	fprintf(stderr,"sql==%s \n",sql);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i;
		for (i = 1; i < nrow + 1; i++) {
			pArea->id = atoi(azResult[i * ncolumn]);
			strcpy(pArea->name, azResult[i * ncolumn + 1]);
			strcpy(pArea->memo, azResult[i * ncolumn + 2]);
			pArea->nodeCount = atoi(azResult[i * ncolumn + 3]);
			pArea++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库 

	return ret;
}

int getAreaNames(char* areaName, char *areaIds) {

	sprintf(sql,
			"SELECT NAME FROM AREA WHERE ROWID IN (%s) ORDER BY ORDERID,ROWID;",
			areaIds);
//	fprintf(stderr,"sql==%s \n",sql);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i;
		for (i = 1; i < nrow + 1; i++) {
			strcat(areaName, azResult[i * ncolumn]);
			strcat(areaName, "; ");
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int getNodeByAreaId(NODE* pNode, int areaID) {
	int i = 0;
	sprintf(sql,
			"SELECT ROWID,MAC,MEMO,AREA,TYPE FROM NODES WHERE AREA=%d AND ISVALID=1",
			areaID);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			pNode->id = atoi(azResult[i * ncolumn]);
			strcpy(pNode->mac, azResult[i * ncolumn + 1]);
			strcpy(pNode->memo, azResult[i * ncolumn + 2]);
			pNode->areaId = atoi(azResult[i * ncolumn + 3]);
			pNode->type = atoi(azResult[i * ncolumn + 4]);
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

int updateNodeCurrent(int nodeID, float current1, float current2,
		float current3, float current4) {
	memset(sql, 0, SQLEN);
	sprintf(sql,
			"UPDATE NODES SET CURRENT1 = %f,CURRENT2 = %f,CURRENT3 = %f,CURRENT4 = %f WHERE ROWID=%d",
			current1, current2, current3, current4, nodeID);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int getNodeByAreaIdType(NODE* pNode, int areaID, int type) {
	int i = 0;
	sprintf(sql,
			"SELECT ROWID,MAC,MEMO,AREA FROM NODES WHERE AREA=%d AND ISVALID=1 AND TYPE=%d;",
			areaID, type);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			pNode->id = atoi(azResult[i * ncolumn]);
			strcpy(pNode->mac, azResult[i * ncolumn + 1]);
			strcpy(pNode->memo, azResult[i * ncolumn + 2]);
			pNode->areaId = atoi(azResult[i * ncolumn + 3]);
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

int getNodes4Area(NODE* pNodes, int areaID, int nodeType) {
	int i = 0;
	sprintf(sql,
			"SELECT ROWID,MAC,MEMO,AREA,ISVALID FROM NODES WHERE (AREA = 0 OR AREA =%d) and TYPE=%d;",
			areaID, nodeType);

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			pNodes->id = atoi(azResult[i * ncolumn]);
			strcpy(pNodes->mac, azResult[i * ncolumn + 1]);
			strcpy(pNodes->memo, azResult[i * ncolumn + 2]);
			pNodes->areaId = atoi(azResult[i * ncolumn + 3]);
			pNodes->isValid = atoi(azResult[i * ncolumn + 4]);
			pNodes++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库 

	return TRUE;
}

int updateAreaNode(int areaID, char* nodeID) {

	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE NODES SET AREA = %d WHERE ROWID=%s", areaID, nodeID);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库 
	return ret;
}

int getAreaIDbyMac(char* mac) {
	sprintf(sql, "SELECT AREA FROM NODES WHERE MAC='%s'", mac);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			return atoi(azResult[1]);
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return 0;
}
BOOL getAreaInfo(AREA* pArea, int areaid) {

	//areaid = 1;
	//fprintf(stderr,"sql areaID==%d \n",areaid);
	BOOL bRet = TRUE;
	sprintf(sql, "SELECT ROWID,NAME,MEMO,KR,KB FROM AREA WHERE ROWID=%d",
			areaid);
//	fprintf(stderr,"sql==%s \n",sql);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			pArea->id = atoi(azResult[ncolumn]);
			strcpy(pArea->name, azResult[ncolumn + 1]);
			strcpy(pArea->memo, azResult[ncolumn + 2]);
			pArea->Kr = atoi(azResult[ncolumn + 3]);
			pArea->Kb = atoi(azResult[ncolumn + 4]);
		} else {
			bRet = FALSE;
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
		bRet = FALSE;
	}
//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库 

	return bRet;
}

int updateArea(int areaID, char* areaName, char* memo, int kr, int kb) {

	memset(sql, 0, SQLEN);
	sprintf(sql,
			"UPDATE AREA SET NAME='%s',MEMO='%s',KR=%d,KB=%d WHERE ROWID=%d;",
			areaName, memo, kr, kb, areaID);
	//fprintf(stderr,"sql==%s \n", sql);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	return ret;
}

int updateAreaOrder(int areaID, int orderId) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE AREA SET ORDERID=%d WHERE ROWID=%d;", orderId, areaID);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	return ret;
}

int insertNewArea(char* areaName, char* memo, int kr, int kb) {
	memset(sql, 0, SQLEN);
	sprintf(sql,
			"INSERT INTO AREA(NAME,MEMO,KR,KB,ORDERID,NODECOUNT) VALUES('%s','%s',%d,%d,0,0);",
			areaName, memo, kr, kb);
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

int deleteArea(int areaID) {
	memset(sql, 0, SQLEN);
	openDB();
	sprintf(sql, "DELETE FROM AREA  WHERE ROWID=%d;", areaID);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int deleteAreaNodeWithType(int areaID, int nodeType) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE NODES SET AREA = 0  WHERE AREA=%d and TYPE=%d;",
			areaID, nodeType);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int deleteAreaNode(int areaID) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE NODES SET AREA = 0  WHERE AREA=%d ;", areaID);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int updateAreaNodeCount() {
	int i = 0;
	sprintf(sql, "SELECT AREA,COUNT(*) FROM NODES WHERE TYPE=0 GROUP BY AREA;");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		sprintf(sql, "UPDATE AREA SET NODECOUNT=0;");
		ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

		for (i = 1; i < nrow + 1; i++) {
			sprintf(sql, "UPDATE AREA SET NODECOUNT=%s WHERE ROWID=%s;",
					azResult[i * ncolumn + 1], azResult[i * ncolumn]);
			ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
		}
	} else {
		nrow = 0;
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}

//=========Node=======================

int getAllNode(NODE* pNodes) {
	int i = 0;

	strcpy(sql,
			"SELECT N.ROWID,N.MAC,N.MEMO,N.AREA,A.NAME,N.TYPE,N.MAXSPEED,N.MAXDISTANCE,N.PULSES,N.ISVALID FROM NODES N LEFT JOIN AREA A ON N.AREA=A.ROWID;");

	//fprintf(stderr,"sql==%s \n",sql);

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			pNodes->id = atoi(azResult[i * ncolumn]);
			strcpy(pNodes->mac, azResult[i * ncolumn + 1]);
			strcpy(pNodes->memo, azResult[i * ncolumn + 2]);
			pNodes->areaId = atoi(azResult[i * ncolumn + 3]);
			if (pNodes->areaId == 0) {
				strcpy(pNodes->areaName, "*");
			} else {
				strcpy(pNodes->areaName, azResult[i * ncolumn + 4]);
			}

			pNodes->type = atoi(azResult[i * ncolumn + 5]);
			pNodes->speed = atof(azResult[i * ncolumn + 6]);
			pNodes->distance = atof(azResult[i * ncolumn + 7]);
			pNodes->pulses = atof(azResult[ncolumn + 8]);
			pNodes->isValid = atoi(azResult[ncolumn + 9]);

			pNodes++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库 

	return TRUE;
}

int getAllNodeByType(NODE* pNodes, int type) {
	int i = 0;

	sprintf(sql,
			"SELECT N.ROWID,N.MAC,N.MEMO,N.AREA,A.NAME,N.TYPE,N.MAXSPEED,N.MAXDISTANCE,N.PULSES,N.ISVALID FROM NODES N LEFT JOIN AREA A ON N.AREA=A.ROWID WHERE N.TYPE=%d;",
			type);

	//fprintf(stderr,"sql==%s \n",sql);

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			pNodes->id = atoi(azResult[i * ncolumn]);
			strcpy(pNodes->mac, azResult[i * ncolumn + 1]);
			strcpy(pNodes->memo, azResult[i * ncolumn + 2]);
			pNodes->areaId = atoi(azResult[i * ncolumn + 3]);
			if (pNodes->areaId == 0) {
				strcpy(pNodes->areaName, "*");
			} else {
				strcpy(pNodes->areaName, azResult[i * ncolumn + 4]);
			}

			pNodes->type = atoi(azResult[i * ncolumn + 5]);
			pNodes->speed = atof(azResult[i * ncolumn + 6]);
			pNodes->distance = atof(azResult[i * ncolumn + 7]);
			pNodes->pulses = atof(azResult[ncolumn + 8]);
			pNodes->isValid = atoi(azResult[ncolumn + 9]);

			pNodes++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return TRUE;
}

int getAllNodeWithPage(NODE* pNodes, int startNo) {
	int i = 0;

	sprintf(sql,
			"SELECT N.ROWID,N.MAC,N.MEMO,N.AREA,A.NAME,N.TYPE,N.MAXSPEED,N.MAXDISTANCE,N.PULSES,N.ISVALID FROM NODES N LEFT JOIN AREA A ON N.AREA=A.ROWID ORDER BY N.ROWID LIMIT 10 OFFSET %d;",
			startNo);

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			pNodes->id = atoi(azResult[i * ncolumn]);
			strcpy(pNodes->mac, azResult[i * ncolumn + 1]);
			strcpy(pNodes->memo, azResult[i * ncolumn + 2]);
			pNodes->areaId = atoi(azResult[i * ncolumn + 3]);
			if (pNodes->areaId == 0) {
				strcpy(pNodes->areaName, "*");
			} else {
				strcpy(pNodes->areaName, azResult[i * ncolumn + 4]);
			}
			pNodes->type = atoi(azResult[i * ncolumn + 5]);
			pNodes->speed = atof(azResult[i * ncolumn + 6]);
			pNodes->distance = atof(azResult[ncolumn + 7]);
			pNodes->pulses = atof(azResult[ncolumn + 8]);
			pNodes->isValid = atoi(azResult[ncolumn + 9]);

			pNodes++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return TRUE;
}

int getDBNodeInfo(NODE* pNodes, int nodeID) {

	sprintf(sql,
			"SELECT ROWID,MAC,MEMO,AREA,TYPE,MAXSPEED,MAXDISTANCE,PULSES,CURRENT1,CURRENT2,CURRENT3,CURRENT4,ISVALID FROM NODES WHERE ROWID=%d",
			nodeID);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			pNodes->id = atoi(azResult[ncolumn]);
			strcpy(pNodes->mac, azResult[ncolumn + 1]);
			strcpy(pNodes->memo, azResult[ncolumn + 2]);
			pNodes->areaId = atoi(azResult[ncolumn + 3]);
			pNodes->type = atoi(azResult[ncolumn + 4]);
			pNodes->speed = atof(azResult[ncolumn + 5]);
			pNodes->distance = atof(azResult[ncolumn + 6]);
			pNodes->pulses = atof(azResult[ncolumn + 7]);

			pNodes->current1 = atof(azResult[ncolumn + 8]);
			pNodes->current2 = atof(azResult[ncolumn + 9]);
			pNodes->current3 = atof(azResult[ncolumn + 10]);
			pNodes->current4 = atof(azResult[ncolumn + 11]);

			pNodes->isValid = atoi(azResult[ncolumn + 12]);
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

int updateNode(NODE* pNodes) {
	memset(sql, 0, SQLEN);
	//char tmp[50];
	sprintf(sql,
			"UPDATE NODES SET MAC = '%s',MEMO = '%s',Type = %d,MAXSPEED=%f,MAXDISTANCE=%f,PULSES=%f,CURRENT1=%f,CURRENT2=%f,CURRENT3=%f,CURRENT4=%f WHERE ROWID=%d;",
			pNodes->mac, pNodes->memo, pNodes->type, pNodes->speed,
			pNodes->distance, pNodes->pulses, pNodes->current1,
			pNodes->current2, pNodes->current3, pNodes->current4, pNodes->id);
	//fprintf(stderr, "sql==%s \n", sql);

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
			"INSERT INTO NODES(MAC,MEMO,AREA,TYPE,MAXSPEED,MAXDISTANCE,PULSES,CURRENT1,CURRENT2,CURRENT3,CURRENT4,ISVALID) VALUES('%s','%s',%d,%d,%f,%f,%f,%f,%f,%f,%f,%d);",
			pNodes->mac, pNodes->memo, pNodes->areaId, pNodes->type,
			pNodes->speed, pNodes->distance, pNodes->pulses, pNodes->current1,
			pNodes->current2, pNodes->current3, pNodes->current4,
			pNodes->isValid);

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

//===========================MODE====================================
int getAllModel(MODE* p_mode) {
	int i = 0;

	strcpy(sql,
			"SELECT ROWID,NAME,TYPE ,PPF1 ,PPF2 ,PPF3 ,PPF4 ,MOTORMODE ,DIRECTION ,SPEED ,DISTANCE  FROM MODE");

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			p_mode->id = atoi(azResult[i * ncolumn]);
			strcpy(p_mode->name, azResult[i * ncolumn + 1]);
			p_mode->type = atoi(azResult[i * ncolumn + 2]);
			p_mode->ppf1 = atof(azResult[i * ncolumn + 3]);
			p_mode->ppf2 = atof(azResult[i * ncolumn + 4]);
			p_mode->ppf3 = atof(azResult[i * ncolumn + 5]);
			p_mode->ppf4 = atof(azResult[i * ncolumn + 6]);
			p_mode->mode = atoi(azResult[i * ncolumn + 7]);
			p_mode->direction = atoi(azResult[i * ncolumn + 8]);
			p_mode->speed = atof(azResult[i * ncolumn + 9]);
			p_mode->distance = atof(azResult[i * ncolumn + 10]);

			p_mode++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return TRUE;
}
int getAllReproduction(MODE* p_mode) {
	int i = 0;

	strcpy(sql,
			"SELECT ROWID,NAME,TYPE ,PPF1 ,PPF2 ,PPF3 ,PPF4 ,MOTORMODE ,DIRECTION ,SPEED ,DISTANCE  FROM STATICMODE");

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) { //将查询结果写入到结构中
			p_mode->id = atoi(azResult[i * ncolumn]);
			strcpy(p_mode->name, azResult[i * ncolumn + 1]);
			p_mode->type = atoi(azResult[i * ncolumn + 2]);
			p_mode->ppf1 = atof(azResult[i * ncolumn + 3]);
			p_mode->ppf2 = atof(azResult[i * ncolumn + 4]);
			p_mode->ppf3 = atof(azResult[i * ncolumn + 5]);
			p_mode->ppf4 = atof(azResult[i * ncolumn + 6]);
			p_mode->mode = atoi(azResult[i * ncolumn + 7]);
			p_mode->direction = atoi(azResult[i * ncolumn + 8]);
			p_mode->speed = atof(azResult[i * ncolumn + 9]);
			p_mode->distance = atof(azResult[i * ncolumn + 10]);

			p_mode++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	//	fprintf(stderr,"error== %s \n",zErrMsg);
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return TRUE;
}
int deleteAllReproductionTask() {

	sprintf(sql, "DELETE FROM TASK WHERE SCHEDULEID=%d", REPRODUCTIONID);

	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	//	fprintf(stderr,"error== %s \n",zErrMsg);

	closeDB(); //关闭数据库

	return TRUE;
}

int getTimeSpan() {

	strcpy(sql, "SELECT SPAN FROM TIMESPAN");

	openDB();
	int timeSpan = 1;
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			timeSpan = atoi(azResult[ncolumn]);

		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return timeSpan;
}

int updateTimeSpan(int timeSpan) {
	memset(sql, 0, SQLEN);
	//char tmp[50];
	sprintf(sql, "UPDATE TIMESPAN SET SPAN = %d", timeSpan);
	openDB();

	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int getDBModelInfo(MODE* p_mode, int modeID) {

	sprintf(sql,
			"SELECT ROWID,NAME,TYPE ,PPF1 ,PPF2 ,PPF3 ,PPF4 ,MOTORMODE ,DIRECTION ,SPEED ,DISTANCE  FROM MODE WHERE ROWID=%d",
			modeID);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		ret = nrow;
		if (nrow == 1) { //将查询结果写入到结构中
			p_mode->id = atoi(azResult[ncolumn]);
			strcpy(p_mode->name, azResult[ncolumn + 1]);
			p_mode->type = atoi(azResult[ncolumn + 2]);
			p_mode->ppf1 = atof(azResult[ncolumn + 3]);

			p_mode->ppf2 = atof(azResult[ncolumn + 4]);
			p_mode->ppf3 = atof(azResult[ncolumn + 5]);
			p_mode->ppf4 = atof(azResult[ncolumn + 6]);

			p_mode->mode = atoi(azResult[ncolumn + 7]);
			p_mode->direction = atoi(azResult[ncolumn + 8]);
			p_mode->speed = atof(azResult[ncolumn + 9]);

			p_mode->distance = atof(azResult[ncolumn + 10]);

		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}

int getStaticModelInfo(MODE* p_mode, int modeID) {

	sprintf(sql,
			"SELECT ROWID,NAME ,PPF1 ,PPF2 ,PPF3 ,PPF4,MOTORMODE ,DIRECTION ,SPEED ,DISTANCE   FROM STATICMODE WHERE RowID=%d",
			modeID);

	fprintf(stderr, "sql==%s\n", sql);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		ret = nrow;
		if (nrow == 1) { //将查询结果写入到结构中
			p_mode->id = atoi(azResult[ncolumn]);
			strcpy(p_mode->name, azResult[ncolumn + 1]);
			p_mode->ppf1 = atof(azResult[ncolumn + 2]);
			p_mode->ppf2 = atof(azResult[ncolumn + 3]);
			p_mode->ppf3 = atof(azResult[ncolumn + 4]);
			p_mode->ppf4 = atof(azResult[ncolumn + 5]);
			p_mode->mode = atoi(azResult[ncolumn + 6]);
			p_mode->direction = atoi(azResult[ncolumn + 7]);
			p_mode->speed = atof(azResult[ncolumn + 8]);
			p_mode->distance = atof(azResult[ncolumn + 9]);
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}

int updateModel(MODE* pModels) {
	memset(sql, 0, SQLEN);
	//char tmp[50];
	//"SELECT ROWID,NAME,TYPE ,PPF1 ,PPF2 ,PPF3 ,PPF4 ,MOTORMODE ,DIRECTION ,SPEED ,DISTANCE  FROM MODE");
	sprintf(sql,
			"UPDATE MODE SET NAME ='%s',TYPE =%d ,PPF1=%f ,PPF2=%f ,PPF3=%f ,PPF4=%f ,MOTORMODE=%d,DIRECTION=%d,SPEED=%f,DISTANCE=%f WHERE ROWID=%d;",
			pModels->name, pModels->type, pModels->ppf1, pModels->ppf2,
			pModels->ppf3, pModels->ppf4, pModels->mode, pModels->direction,
			pModels->speed, pModels->distance, pModels->id);

	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}
int updateReproduction(MODE* pModels) {
	memset(sql, 0, SQLEN);
	//char tmp[50];
	sprintf(sql,
			"UPDATE STATICMODE SET NAME ='%s',TYPE =%d ,PPF1=%f ,PPF2=%f ,PPF3=%f ,PPF4=%f ,MOTORMODE=%d,DIRECTION=%d,SPEED=%f,DISTANCE=%f WHERE ROWID=%d;",
			pModels->name, pModels->type, pModels->ppf1, pModels->ppf2,
			pModels->ppf3, pModels->ppf4, pModels->mode, pModels->direction,
			pModels->speed, pModels->distance, pModels->id);

	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}
int insertReproduction(MODE* pModels) {

	memset(sql, 0, SQLEN);
	sprintf(sql,
			"INSERT INTO STATICMODE(NAME,TYPE ,PPF1 ,PPF2 ,PPF3 ,PPF4 ,MOTORMODE ,DIRECTION ,SPEED ,DISTANCE) VALUES('%s',%d,%f,%f,%f,%f,%d,%d,%f,%f);",
			pModels->name, pModels->type, pModels->ppf1, pModels->ppf2,
			pModels->ppf3, pModels->ppf4, pModels->mode, pModels->direction,
			pModels->speed, pModels->distance);
	//fprintf(stderr, "SQL==%s", sql);
	openDB();
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
int insertModel(MODE* pModels) {

	memset(sql, 0, SQLEN);
	sprintf(sql,
			"INSERT INTO MODE(NAME,TYPE ,PPF1 ,PPF2 ,PPF3 ,PPF4 ,MOTORMODE ,DIRECTION ,SPEED ,DISTANCE ) VALUES('%s',%d,%f,%f,%f,%f,%d,%d,%f,%f);",
			pModels->name, 1, pModels->ppf1, pModels->ppf2, pModels->ppf3,
			pModels->ppf4, pModels->mode, pModels->direction, pModels->speed,
			pModels->distance);
	//fprintf(stderr, "SQL==%s", sql);
	openDB();
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
int deleteModel(int modeID) {
	openDB();
	memset(sql, 0, SQLEN);

	sprintf(sql, "DELETE FROM MODE  WHERE ROWID=%d;", modeID);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}
int deleteReproduction(int modeID) {
	openDB();
	memset(sql, 0, SQLEN);

	sprintf(sql, "DELETE FROM STATICMODE  WHERE ROWID=%d;", modeID);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

//===========================TASK====================================

int getScheduleListbyPage(SCHEDULEINFO* p_schedule, char* status, int startNo) {
	sprintf(sql,
			"SELECT ROWID,STARTTIME,ENDTIME,DESCRIPTION,STATUS,MODIFYTIME,AREALIST,MAXSQUENCE,CURRENTSQU,CURRSQULEFTTIME FROM SCHEDULE WHERE STATUS IN(%s) ORDER BY ROWID DESC LIMIT 10 OFFSET %d;",
			status, startNo);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i = 1;
		for (i = 1; i < nrow + 1; i++) {
			p_schedule->id = atoi(azResult[i * ncolumn]);
			p_schedule->startTime = atoi(azResult[i * ncolumn + 1]);
			p_schedule->endTime = atoi(azResult[i * ncolumn + 2]);
			if (azResult[i * ncolumn + 3] != NULL ) {
				strcpy(p_schedule->description, azResult[i * ncolumn + 3]);
			} else {
				strcpy(p_schedule->description, "");
			}
			p_schedule->status = atoi(azResult[i * ncolumn + 4]);
			p_schedule->modifyTime = atoi(azResult[i * ncolumn + 5]);
			strcpy(p_schedule->areaList, azResult[i * ncolumn + 6]);
			p_schedule->maxSquence = atoi(azResult[i * ncolumn + 7]);
			p_schedule->currentSquence = atoi(azResult[i * ncolumn + 8]);
			p_schedule->currSquLeftTime = atoi(azResult[i * ncolumn + 9]);

			p_schedule++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}
int getScheduleList(SCHEDULEINFO* p_schedule, char* status) {
	sprintf(sql,
			"SELECT ROWID,STARTTIME,ENDTIME,DESCRIPTION,STATUS,MODIFYTIME,AREALIST,MAXSQUENCE,CURRENTSQU,CURRSQULEFTTIME FROM SCHEDULE WHERE STATUS IN(%s) ORDER BY ROWID DESC;",
			status);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i = 1;
		for (i = 1; i < nrow + 1; i++) {
			p_schedule->id = atoi(azResult[i * ncolumn]);
			p_schedule->startTime = atoi(azResult[i * ncolumn + 1]);
			p_schedule->endTime = atoi(azResult[i * ncolumn + 2]);
			if (azResult[i * ncolumn + 3] != NULL ) {
				strcpy(p_schedule->description, azResult[i * ncolumn + 3]);
			} else {
				strcpy(p_schedule->description, "");
			}
			p_schedule->status = atoi(azResult[i * ncolumn + 4]);
			p_schedule->modifyTime = atoi(azResult[i * ncolumn + 5]);
			strcpy(p_schedule->areaList, azResult[i * ncolumn + 6]);
			p_schedule->maxSquence = atoi(azResult[i * ncolumn + 7]);
			p_schedule->currentSquence = atoi(azResult[i * ncolumn + 8]);
			p_schedule->currSquLeftTime = atoi(azResult[i * ncolumn + 9]);

			p_schedule++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}
int getSchedule(SCHEDULEINFO* p_schedule, int scheduleId) {
	sprintf(sql,
			"SELECT ROWID,STARTTIME,ENDTIME,DESCRIPTION,STATUS,MODIFYTIME,AREALIST,MAXSQUENCE FROM SCHEDULE WHERE ROWID=%d;",
			scheduleId);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		p_schedule->id = atoi(azResult[ncolumn]);
		//fprintf(stderr,"p_schedule->id ==%s\n",azResult[ncolumn]);
		p_schedule->startTime = atol(azResult[ncolumn + 1]);
		p_schedule->endTime = atol(azResult[ncolumn + 2]);

		//fprintf(stderr,"azResult[ncolumn + 3]==%s\n",azResult[ncolumn + 3]);

		if (azResult[ncolumn + 3] != NULL ) {
			strcpy(p_schedule->description, azResult[ncolumn + 3]);
		}
		p_schedule->status = atoi(azResult[ncolumn + 4]);
		p_schedule->modifyTime = atoi(azResult[ncolumn + 5]);
		strcpy(p_schedule->areaList, azResult[ncolumn + 6]);
		p_schedule->maxSquence = atoi(azResult[ncolumn + 7]);

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}
int insertSchedule(SCHEDULEINFO* p_schedule) {
	memset(sql, 0, SQLEN);
	sprintf(sql,
			"INSERT INTO SCHEDULE(STARTTIME,ENDTIME,DESCRIPTION,STATUS,MODIFYTIME,AREALIST,MAXSQUENCE,CURRENTSQU,CURRSQULEFTTIME) VALUES(%ld,%ld,'%s',%d,%d,'%s',%d,0,0);",
			p_schedule->startTime, p_schedule->endTime, p_schedule->description,
			p_schedule->status, p_schedule->modifyTime, p_schedule->areaList,
			p_schedule->maxSquence);

	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);

	}
	int rowID = sqlite3_last_insert_rowid(db);
	closeDB(); //关闭数据库
	return rowID;
}

int updateSchedule(SCHEDULEINFO* p_schedule) {
	memset(sql, 0, SQLEN);
	sprintf(sql,
			"UPDATE SCHEDULE SET STARTTIME=%ld,ENDTIME=%ld,DESCRIPTION='%s',STATUS=%d,MODIFYTIME=%d,AREALIST='%s',MAXSQUENCE=%d WHERE ROWID=%d;",
			p_schedule->startTime, p_schedule->endTime, p_schedule->description,
			p_schedule->status, p_schedule->modifyTime, p_schedule->areaList,
			p_schedule->maxSquence, p_schedule->id);
	openDB();

	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int updateScheduleStatus(int status, int scheduleId) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE SCHEDULE SET STATUS=%d WHERE ROWID=%d;", status,
			scheduleId);
	//fprintf(stderr,"OKsql==%s \n", sql);
	openDB();

	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int removeDBSchedule(int scheduleID) {
	memset(sql, 0, SQLEN);
	openDB();
	sprintf(sql, "DELETE FROM TASk WHERE SCHEDULEID=%d", scheduleID);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

	sprintf(sql, "DELETE FROM SCHEDULE WHERE ROWID=%d", scheduleID);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int getTaskList(TASK* pTask, int scheduleId) {
	sprintf(sql,
			"SELECT T.SCHEDULEID,T.SEQUENECE,T.MODELID,T.DURING,M.NAME FROM TASK T LEFT JOIN MODE M ON T.MODELID=M.ROWID WHERE T.SCHEDULEID=%d ORDER BY T.SEQUENECE;",
			scheduleId);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i;
		for (i = 1; i < nrow + 1; i++) {
			pTask->scheduleId = atoi(azResult[i * ncolumn]);
			pTask->sequence = atoi(azResult[i * ncolumn + 1]);
			pTask->modeId = atoi(azResult[i * ncolumn + 2]);
			pTask->during = atof(azResult[i * ncolumn + 3]);
			strcpy(pTask->modeName, azResult[i * ncolumn + 4]);
			pTask++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int getTaskInfo(TASK* pTask, int scheduleId, int squenece) {
	sprintf(sql,
			"SELECT T.SCHEDULEID,T.SEQUENECE,T.MODELID,T.DURING FROM TASK T WHERE T.SCHEDULEID=%d AND SEQUENECE=%d;",
			scheduleId, squenece);
	//fprintf(stderr,"OKSQL==%s\n", sql);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			pTask->scheduleId = atoi(azResult[ncolumn]);
			pTask->sequence = atoi(azResult[ncolumn + 1]);
			pTask->modeId = atoi(azResult[ncolumn + 2]);
			pTask->during = atof(azResult[ncolumn + 3]);
			strcpy(pTask->modeName, "");
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return nrow;
}

int insertTask(TASK* task, int scheduleID) {
	memset(sql, 0, SQLEN);
	sprintf(sql,
			"INSERT INTO TASK(SCHEDULEID,SEQUENECE,MODELID,DURING) VALUES(%d,%d,%d,%.2f);",
			scheduleID, task->sequence, task->modeId, task->during);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	fprintf(stderr, "insert OK! \n");
	closeDB(); //关闭数据库
	return ret;
}

int updateReproductionTask(float during) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "UPDATE TASK SET DURING=%.2f WHERE SCHEDULEID=%d;", during,
			REPRODUCTIONID);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}
int deleteTask(int scheduleID) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "DELETE FROM TASK WHERE SCHEDULEID=%d;", scheduleID);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

//------------------------USER----------------------------------
int loginDB(char* userName, char* password, int *role) {
	BOOL bRet = TRUE;
	sprintf(sql, "SELECT PASSWD,ROLE FROM USERS WHERE USERNAME='%s';",
			userName);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			strcpy(password, azResult[ncolumn]);
			*role = atoi(azResult[ncolumn + 1]);
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
		bRet = FALSE;
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return bRet;
}

int getAllUsers(USER* users) {
	BOOL bRet = TRUE;
	sprintf(sql, "SELECT ROWID,USERNAME,PASSWD,ROLE FROM USERS;");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i = 0;
		for (i = 1; i < nrow + 1; i++) {
			users->userId = atol(azResult[i * ncolumn]);
			strcpy(users->userName, azResult[i * ncolumn + 1]);
			strcpy(users->userPwd, azResult[i * ncolumn + 2]);
			users->role = atoi(azResult[i * ncolumn + 3]);

			users++;
		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
		bRet = FALSE;
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return bRet;
}

int getUserInfo(USER* users, int userID) {
	BOOL bRet = TRUE;
	sprintf(sql, "SELECT ROWID,USERNAME,PASSWD,ROLE FROM USERS WHERE ROWID=%d;",
			userID);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			users->userId = atol(azResult[ncolumn]);
			strcpy(users->userName, azResult[ncolumn + 1]);
			strcpy(users->userPwd, azResult[ncolumn + 2]);
			users->role = atoi(azResult[ncolumn + 3]);

		}
	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
		bRet = FALSE;
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return bRet;
}

int saveUser(char* userName, char* password, int role) {

	sprintf(sql, "INSERT INTO USERS values('%s','%s',%d);", userName, password,
			role);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int updateUser(char* userName, char* password, int role) {

	sprintf(sql,
			"UPDATE USERS SET PASSWORD = '%s',role = %d where userName='%s';)",
			password, role, userName);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int deleteUser(int userID) {

	sprintf(sql, "DELETE FROM USERS WHERE ROWID=%d;", userID);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int updateDB(char* userName, char* password) {
	sprintf(sql, "UPDATE USERS SET PASSWD='%s' WHERE USERNAME='%s'", password,
			userName);
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
			"SELECT ROWID FROM COMMLOG WHERE AREAID =%d ORDER BY ROWID DESC LIMIT 1;",
			log->areaId);
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		if (nrow == 1) {
			lastRowid = atoi(azResult[ncolumn]);
		}
	}
	if (lastRowid != 0) {
		sprintf(sql, "UPDATE COMMLOG SET ENDTIME =%ld WHERE ROWID=%d;",
				log->dateTime, lastRowid);
		ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "sql==%s \n", sql);
			fprintf(stderr, "error: %s \n", zErrMsg);
		}
	}
	sprintf(sql,
			"INSERT INTO COMMLOG(DATETIME ,ENDTIME,MODELID ,MODELNAME , AREAID ,AREANAME ,PPF1 ,PPF2 ,PPF3 ,PPF4,OPTIONTYPE,TIMESTOPED) VALUES(%ld,%d,%d,'%s',%d,'%s',%f,%f,%f,%f,%d,%d);",
			log->dateTime, 0, log->modeId, log->modeName, log->areaId,
			log->areaName, log->PPF1, log->PPF2, log->PPF3, log->PPF4,
			log->optionType, 0);

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

int getCommLog(COMMLOG *log, int startTime, int endTime, int areaId,
		int startNo) {
	int i = 0;
	if (areaId != 0) {
		sprintf(sql,
				"SELECT DATETIME ,MODELID ,MODELNAME , AREAID ,AREANAME ,PPF1 ,PPF2 ,PPF3 ,PPF4,OPTIONTYPE,TIMESTOPED,ROWID FROM COMMLOG  WHERE (DATETIME BETWEEN %d AND %d) AND AREAID =%d ORDER BY DATETIME DESC LIMIT 10 OFFSET %d;",
				startTime, endTime, areaId, startNo);
	} else {
		sprintf(sql,
				"SELECT DATETIME ,MODELID ,MODELNAME , AREAID ,AREANAME ,PPF1 ,PPF2 ,PPF3 ,PPF4,OPTIONTYPE,TIMESTOPED,ROWID FROM COMMLOG  WHERE (DATETIME BETWEEN %d AND %d) ORDER BY DATETIME DESC LIMIT 10 OFFSET %d;",
				startTime, endTime, startNo);
	}
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) {
			log->dateTime = atol(azResult[i * ncolumn]);
			log->modeId = atoi(azResult[i * ncolumn + 1]);
			strcpy(log->modeName, azResult[i * ncolumn + 2]);
			log->areaId = atoi(azResult[i * ncolumn + 3]);
			strcpy(log->areaName, azResult[i * ncolumn + 4]);
			log->PPF1 = atof(azResult[i * ncolumn + 5]);
			log->PPF2 = atof(azResult[i * ncolumn + 6]);
			log->PPF3 = atof(azResult[i * ncolumn + 7]);
			log->PPF4 = atof(azResult[i * ncolumn + 8]);
			log->optionType = atoi(azResult[i * ncolumn + 9]);
			log->timeStoped = atoi(azResult[i * ncolumn + 10]);

			log->id = atoi(azResult[i * ncolumn + 11]);

			log++;
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

int getCommLogDetail(COMMLOG *log, int logId) {

	sprintf(sql,
			"SELECT DATETIME ,ENDTIME ,MODELNAME  ,AREANAME ,PPF1 ,PPF2 ,PPF3 ,PPF4,OPTIONTYPE,TIMESTOPED FROM COMMLOG  WHERE ROWID = %d;",
			logId);

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		log->dateTime = atol(azResult[ncolumn]);
		log->endTime = atol(azResult[ncolumn + 1]);
		strcpy(log->modeName, azResult[ncolumn + 2]);
		strcpy(log->areaName, azResult[ncolumn + 3]);
		log->PPF1 = atoi(azResult[ncolumn + 4]);
		log->PPF2 = atoi(azResult[ncolumn + 5]);
		log->PPF3 = atoi(azResult[ncolumn + 6]);
		log->PPF4 = atoi(azResult[ncolumn + 7]);
		log->optionType = atoi(azResult[ncolumn + 8]);
		log->timeStoped = atoi(azResult[ncolumn + 9]);

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}
//*********************USERLOG********************
void writeUserLog(USERLOG *log) {
	openDB();

	sprintf(sql, "INSERT INTO USERLOG VALUES('%s','%s','%ld','%d','%s');",
			log->userName, log->IP, log->logTime, log->logType, log->logResult);
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库

	return;
}

int getUserLog(USERLOG *log, int startTime, int endTime, char* userName,
		int startNo) {
	int i = 0;
	char temp[64];
	sprintf(sql,
			"SELECT USERNAME,IP ,LOGTIME ,LOGTYPE,LOGRESULT FROM USERLOG  WHERE (LOGTIME BETWEEN %d AND %d)",
			startTime, endTime);

	if (strlen(userName) > 0) {
		sprintf(temp, " AND userName ='%s' ", userName);
		strcat(sql, temp);
	}

	sprintf(temp, " ORDER BY LOGTIME DESC LIMIT 10 OFFSET %d;", startNo);
	strcat(sql, temp);

	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		for (i = 1; i < nrow + 1; i++) {
			strcpy(log->userName, azResult[i * ncolumn]);
			strcpy(log->IP, azResult[i * ncolumn + 1]);

			log->logTime = atoi(azResult[i * ncolumn + 2]);
			log->logType = atoi(azResult[i * ncolumn + 3]);
			strcpy(log->logResult, azResult[i * ncolumn + 4]);

			log++;
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库

	return ret;
}

//**************************************
int updateServerName(char* serverName) {
	sprintf(sql, "UPDATE SERVERNAME SET SERVERNAME='%s';", serverName);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int getServerName(char* serverName) {
	sprintf(sql, "SELECT SERVERNAME FROM SERVERNAME LIMIT 1;");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		strcpy(serverName, azResult[ncolumn]);

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}

int updateSendSetting(int reSendTimes, int iBroadcast, int sendConfirm) {
	sprintf(sql,
			"UPDATE SENDSETTING SET SENDTIMES=%d,BROADCAST=%d,SENDCONFIRM=%d;",
			reSendTimes, iBroadcast, sendConfirm);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int getSendSetting(int* reSendTimes, int *iBroadcast, int* sendConfirm) {
	sprintf(sql,
			"SELECT SENDTIMES, BROADCAST,SENDCONFIRM FROM SENDSETTING LIMIT 1;");
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		*reSendTimes = atoi(azResult[ncolumn]);
		*iBroadcast = atoi(azResult[ncolumn + 1]);
		*sendConfirm = atoi(azResult[ncolumn + 2]);
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

int modifyWaitSchedule4Stop(int timeStoped) {
	sprintf(sql,
			"UPDATE SCHEDULE SET STARTTIME=STARTTIME+%d,ENDTIME=ENDTIME+%d,DESCRIPTION='*断电恢复*'||DESCRIPTION WHERE STATUS=0;",
			timeStoped, timeStoped);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;

}
int modifyRunningSchedule4Stop(int timeStoped) {
	sprintf(sql,
			"UPDATE SCHEDULE SET ENDTIME=ENDTIME+%d,DESCRIPTION='*断电恢复*'||DESCRIPTION WHERE STATUS=1;",
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
int modifySchedule4Exception() {
	sprintf(sql, "UPDATE SCHEDULE SET STATUS=0 WHERE STATUS<3;");
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;

}

int setCurrentSquence(int squence, int squenceLeftTime, int scheduleId) {
	sprintf(sql,
			"UPDATE SCHEDULE SET CURRENTSQU=%d,CURRSQULEFTTIME=%d WHERE ROWID=%d;",
			squence, squenceLeftTime, scheduleId);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int setCurrentSquenceLeftTime(int squenceLeftTime, int scheduleId) {
	sprintf(sql, "UPDATE SCHEDULE SET CURRSQULEFTTIME=%d WHERE ROWID=%d;",
			squenceLeftTime, scheduleId);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}

	closeDB(); //关闭数据库
	return ret;
}

int getCurrentSquence(int* squence, int* squenceLeftTime, int scheduleId) {
	sprintf(sql,
			"SELECT CURRENTSQU,CURRSQULEFTTIME FROM SCHEDULE WHERE ROWID=%d;",
			scheduleId);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		*squence = atoi(azResult[ncolumn]);
		*squenceLeftTime = atoi(azResult[ncolumn + 1]);

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}

int insertBackDB(char* dbName, char* backDate) {
	memset(sql, 0, SQLEN);
	sprintf(sql, "INSERT INTO BACKDB(DATETIME,DBNAME) VALUES('%s','%s');",
			backDate, dbName);
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	fprintf(stderr, "insert OK! \n");
	closeDB(); //关闭数据库
	return ret;
}

int getBackDBList(BACKDB* dbInfo,char *order) {
	sprintf(sql, "SELECT DATETIME,DBNAME FROM BACKDB ORDER BY ROWID %s;",order);
	openDB();
	ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (ret == SQLITE_OK) {
		int i = 0;
		for (i = 1; i < nrow + 1; i++) {
			strcpy(dbInfo->dbDate, azResult[i * ncolumn]);
			strcpy(dbInfo->dbName, azResult[i * ncolumn + 1]);
			dbInfo++;
		}

	} else {
		fprintf(stderr, "sql==%s \n", sql);
		fprintf(stderr, "error: %s \n", zErrMsg);
	}
	sqlite3_free_table(azResult);
	closeDB(); //关闭数据库
	return ret;
}

int setBackDBList(BACKDB* dbInfo, int recordCount) {
	sprintf(sql, "DELETE FROM BACKDB;");
	openDB();
	ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

	int i = 0;
	for (i = 0; i < recordCount; i++) {
		sprintf(sql, "INSERT INTO BACKDB(DATETIME,DBNAME) VALUES('%s','%s');",
				dbInfo->dbDate, dbInfo->dbName);

		ret = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);

		dbInfo++;
	}
	closeDB(); //关闭数据库
	return ret;
}

