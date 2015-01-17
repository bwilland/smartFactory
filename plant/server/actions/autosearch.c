#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include "autosearch.h"
#include "../shared/lightingpack.h"
#include "../shared/serialport.h"
#include "../shared/sharemem.h"
#include "node.h"
#include <pthread.h>
/**
 自动搜索设备
 */

void autoSearch(char* responseMsg, int isReset) {
	int i = 0, macCount = 0;
	int iCount = 0;
	char tmp[64];
	fprintf(stderr,"AutoSearch started... \n");
	BYTE buffer[BUFFERSIZE];
	memset(buffer,0,BUFFERSIZE);
	char command[32];
	if (isReset) {
		strcpy(command, "AT+RESET");
		iCount = serialWriter((BYTE*) command, 8, buffer, FALSE);
		sleep(7); //等待6.5秒

		strcpy(command, "AT+MTO");
		iCount = serialWriter((BYTE*) command, 6, buffer, FALSE);
		sleep(10);
	}
	strcpy(command, "AT+NODETABLE=FF");
	iCount = serialWriter((BYTE*) command, 15, buffer, TRUE);

	fprintf(stderr,"=====READ FROM COO: %d BYTES \n", iCount);

	//buffer[iCount] = '\0';
	fprintf(stderr,"%s \n", buffer);
	printHEX(buffer, iCount);

	if (iCount == 0) {
		fprintf(stderr,"Not Responded. \n");
		fprintf(stderr,"AuotoSearch stopped!\n");

		makeReturnMsg(responseMsg, 0, "autoSearch failed.");
		return;
	}

	if (isReset) {
		deleteAllNode(); //删除所有节点
	} else {
		invalidAllNodes(); //初始化设备状态
	}
	char mac[9];
	macCount = getLastNodeId() + 1;
	for (i = 0; i < iCount; i++) {
		//fprintf(stderr,"%c", buffer[i]);
		if (buffer[i] == 0x09) { // tab分割符号
			memcpy(mac, &(buffer[i + 1]), 8);
			mac[8] = '\0';
			fprintf(stderr,"%s \n", mac);

			//
			if (isReset) {
				//if (getResultCount("NODES", tmp) == 0) {
				//保存数据到数据库
				NODE node;
				bzero(&node,sizeof(NODE));
				strcpy(node.mac, mac);
				sprintf(node.memo, "Node%d", macCount);
				macCount++;

				node.areaId = 0;
				node.isValid = 1;
				int nodeID = insertNode(&node);
				if (nodeID == -1) {
					fprintf(stderr,"Save node to database error.");
				}
			} else {
				sprintf(tmp, "MAC='%s'", mac);

				if (getResultCount("NODES", tmp) == 0) {
					//保存数据到数据库
					NODE node;
					bzero(&node,sizeof(NODE));
					strcpy(node.mac, mac);
					sprintf(node.memo, "Node%d", macCount);
					macCount++;

					node.areaId = 0;
					node.isValid = 1;
					int nodeID = insertNode(&node);
					if (nodeID == -1) {
						fprintf(stderr,"Save node to database error.");
					}
				} else {
					validNode(mac);
				}
			}
			i += 17;

		}
	}

	makeReturnMsg(responseMsg, 1, "autoSearch success.");

	fprintf(stderr,"AuotoSearch stopped!\n");
	return;

}

