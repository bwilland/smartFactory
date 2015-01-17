#include "lightingpack.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/**
 * 指令打包类
 * @author Bill
 *
 */

/**
 * 制作通用帧内容
 * @return通用帧长度
 */
int genDeviceControlFrame(SERVERCOMMAND* commandInfo, BYTE* bFrame) {
	int i = 0;
	bzero(bFrame, 64);

	bFrame[0] = 0x2A;
	bFrame[1] = 0x27;
	bFrame[2] = 0x41;
	bFrame[3] = 0x88;

	for (i = 0; i < 14; i++) {
		bFrame[i + 4] = 0x00;
	}

	//Address
	BYTE bMac[5];
	atoMac((BYTE*) commandInfo->MAC, bMac);
	for (i = 0; i < 4; i++) {
		bFrame[i + 18] = bMac[3 - i];
	}

	for (i = 0; i < 6; i++) {
		bFrame[i + 22] = 0x00;
	}

	//帧 类 型：0x2500
	bFrame[28] = 0x25;
	bFrame[29] = 0x00;

	//源地址 2 固定为0000
	bFrame[30] = 0x00;
	bFrame[31] = 0x00;

	bFrame[32] = 0xC1;
	bFrame[33] = 0x2B;


	bFrame[34] = 0x00;

	bFrame[35] = 0x00;

	bFrame[36] = 0x04;

	//can be changed -> flag
	bFrame[37] = 0x01;
	//onoff
	bFrame[38] = commandInfo->onOff;
	//dimmer
	bFrame[39] = commandInfo->dimmer & 0x00FF;
	bFrame[40] = (commandInfo->dimmer & 0xFF00) >> 8;

	int iSum = sum(bFrame, 2, 40);
	bFrame[41] = (iSum & 0x00FF);
	bFrame[42] = 0x23;
	printHEX(bFrame, 43);

	return 43; //总长度为40BYTE.
}



int genSingleGroupControlFrame(SERVERCOMMAND* sCommand, BYTE* bFrame) {
	int i = 0;
	bzero(bFrame, 64);

	bFrame[0] = 0x2A;
	bFrame[1] = 0x27;
	bFrame[2] = 0x41;
	bFrame[3] = 0x88;

	for (i = 0; i < 14; i++) {
		bFrame[i + 4] = 0x00;
	}

	//Address broadcast
	bFrame[18] = 0xFF;
	bFrame[19] = 0xFF;
	bFrame[20] = 0xFF;
	bFrame[21] = 0xFF;

	for (i = 0; i < 6; i++) {
		bFrame[i + 22] = 0x00;
	}

	//帧 类 型：0x2501
	bFrame[28] = 0x25;
	bFrame[29] = 0x01;

	//groupIDs
	int bGroup = 1;
	bGroup = bGroup << (sCommand->groupID - 1);

	//Desc
	bFrame[31] = (bGroup & 0xFF00) >> 8;
	bFrame[30] = bGroup & 0x00FF;

	bFrame[32] = 0xC1;
	bFrame[33] = 0x2B;

	bFrame[34] = 0x00;
	bFrame[35] = 0x00;

	bFrame[36] = 0x04;

	//can be changed -> flag
	bFrame[37] = 0x01;
	//onoff
	bFrame[38] = sCommand->onOff;
	//dimmer
	bFrame[39] = sCommand->dimmer & 0x00FF;
	bFrame[40] = (sCommand->dimmer & 0xFF00) >> 8;

	int iSum = sum(bFrame, 2, 40);
	bFrame[41] = (iSum & 0x00FF);
	bFrame[42] = 0x23;
	printHEX(bFrame, 43);

	return 43; //总长度为40BYTE.
}

int genGroupsControlFrame(SERVERCOMMAND* sCommand, BYTE* bFrame) {
	int i = 0;
	bzero(bFrame, 64);

	bFrame[0] = 0x2A;
	bFrame[1] = 0x27;
	bFrame[2] = 0x41;
	bFrame[3] = 0x88;

	for (i = 0; i < 14; i++) {
		bFrame[i + 4] = 0x00;
	}

	//Address broadcast
	bFrame[18] = 0xFF;
	bFrame[19] = 0xFF;
	bFrame[20] = 0xFF;
	bFrame[21] = 0xFF;

	for (i = 0; i < 6; i++) {
		bFrame[i + 22] = 0x00;
	}

	//帧 类 型：0x2501
	bFrame[28] = 0x25;
	bFrame[29] = 0x01;

	//groupIDs
	int bGroup = 0;
	//bGroup = bGroup << (sCommand->groupID - 1);

	char p_Paras[256];
	char* token, *p;
	strcpy(p_Paras, sCommand->groupList);
	p = p_Paras;

	while (p != NULL ) {

		token = strsep(&p, ",");
		if (token != NULL ) {
			int groupID = atoi(token);
			bGroup += 1 << (groupID - 1);

		} else {
			break;
		}
	}


	//Desc
	bFrame[31] = (bGroup & 0xFF00) >> 8;
	bFrame[30] = bGroup & 0x00FF;

	bFrame[32] = 0xC1;
	bFrame[33] = 0x2B;

	bFrame[34] = 0x00;
	bFrame[35] = 0x00;

	bFrame[36] = 0x04;

	//can be changed -> flag
	bFrame[37] = 0x01;
	//onoff
	bFrame[38] = sCommand->onOff;
	//dimmer


	bFrame[39] = sCommand->dimmer & 0x00FF;
	bFrame[40] = (sCommand->dimmer & 0xFF00) >> 8;

	int iSum = sum(bFrame, 2, 40);
	bFrame[41] = (iSum & 0x00FF);
	bFrame[42] = 0x23;
	printHEX(bFrame, 43);

	return 43; //总长度为40BYTE.
}

int genGroupSetFrame(GroupSettingCommand* sCommand, BYTE* bFrame) {
	int i = 0;
	bzero(bFrame, 64);

	bFrame[0] = 0x2A;
	bFrame[1] = 0x27;
	bFrame[2] = 0x41;
	bFrame[3] = 0x88;

	for (i = 0; i < 14; i++) {
		bFrame[i + 4] = 0x00;
	}

	//Address
	BYTE bMac[5];
	atoMac((BYTE*) sCommand->MAC, bMac);
	for (i = 0; i < 4; i++) {
		bFrame[i + 18] = bMac[3 - i];
	}

	for (i = 0; i < 6; i++) {
		bFrame[i + 22] = 0x00;
	}

	//帧 类 型：0x2501
	bFrame[28] = 0x25;
	bFrame[29] = 0x00;

	//groupIDs
	//int bGroup = 1;
	//bGroup = bGroup < (sCommand->groupID - 1);
	bFrame[30] = 0x00;
	bFrame[31] = 0x00;

	bFrame[32] = 0xE9;
	bFrame[33] = 0x03;

	bFrame[34] = 0x06;
	bFrame[35] = 0x00;

	bFrame[36] = 0x04;

	//group
	long bGroup = 1;
	//printf( "bGroup=%d \n",bGroup);

	//printf( "sCommand->groupID-1=%d \n",sCommand->groupID - 1);

	bGroup = bGroup << (sCommand->groupID - 1);
	//printf( "group=%d \n",bGroup);

	//Desc
	bFrame[40] = 0x00;

	bFrame[39] = 0x00;

	bFrame[38] = (bGroup & 0x0000FF00) >> 8;

	bFrame[37] = (bGroup & 0x000000FF);

	int iSum = sum(bFrame, 2, 40);
	bFrame[41] = (iSum & 0x00FF);
	bFrame[42] = 0x23;
	printHEX(bFrame, 43);

	return 43; //总长度为40BYTE.
}

//sensor info
int genSensorQueryFrame(SERVERCOMMAND* commandInfo, BYTE* bFrame) {
	int i = 0;
	bzero(bFrame, 64);

	bFrame[0] = 0x2A;
	bFrame[1] = 0x22;
	bFrame[2] = 0x41;
	bFrame[3] = 0x88;

	for (i = 0; i < 14; i++) {
		bFrame[i + 4] = 0x00;
	}

	//Address
	BYTE bMac[5];
	atoMac((BYTE*) commandInfo->MAC, bMac);
	for (i = 0; i < 4; i++) {
		bFrame[i + 18] = bMac[3 - i];
	}

	for (i = 0; i < 6; i++) {
		bFrame[i + 22] = 0x00;
	}

	//帧 类 型：0x2000
	bFrame[28] = 0x20;
	bFrame[29] = 0x00;

	//源地址 2 固定为0000
	bFrame[30] = 0x00;
	bFrame[31] = 0x00;

	//光照检测索引
	bFrame[32] = 0x94;
	bFrame[33] = 0x2A;

	//sub index
	bFrame[34] = 0x00;

	bFrame[35] = 0x00;

	int iSum = sum(bFrame, 2, 35);
	bFrame[36] = (iSum & 0x00FF);
	bFrame[37] = 0x23;
	printHEX(bFrame, 38);

	return 38; //总长度为40BYTE.
}

int genGetDeviceInfoFrame(SERVERCOMMAND* commandInfo, BYTE* bFrame) {
	int i = 0;
	bzero(bFrame, 64);

	bFrame[0] = 0x2A;
	bFrame[1] = 0x22;
	bFrame[2] = 0x41;
	bFrame[3] = 0x88;

	for (i = 0; i < 14; i++) {
		bFrame[i + 4] = 0x00;
	}

	//Address
	BYTE bMac[5];
	atoMac((BYTE*) commandInfo->MAC, bMac);
	for (i = 0; i < 4; i++) {
		bFrame[i + 18] = bMac[3 - i];
	}

	for (i = 0; i < 6; i++) {
		bFrame[i + 22] = 0x00;
	}

	//帧 类 型：0x2000
	bFrame[28] = 0x20;
	bFrame[29] = 0x00;

	//源地址 2 固定为0000
	bFrame[30] = 0x00;
	bFrame[31] = 0x00;

	bFrame[32] = 0xC1;
	bFrame[33] = 0x2B;


	bFrame[34] = 0x00;

	bFrame[35] = 0x00;

	bFrame[36] = 0x04;

	//can be changed -> flag
	bFrame[37] = 0x01;
	//onoff
	bFrame[38] = commandInfo->onOff;
	//dimmer
	bFrame[39] = commandInfo->dimmer & 0x00FF;
	bFrame[40] = (commandInfo->dimmer & 0xFF00) >> 8;

	int iSum = sum(bFrame, 2, 40);
	bFrame[41] = (iSum & 0x00FF);
	bFrame[42] = 0x23;
	printHEX(bFrame, 43);

	return 43; //总长度为40BYTE.
}

//***********************************************************

/**
 * 校验和
 * @param iFrom 开始位置
 * @param iEnd 结束位置
 * @return
 */
int sum(BYTE* bFrame, int iFrom, int iEnd) {
	int iSum = 0;
	int i = 0;
	bFrame += iFrom;
	for (i = iFrom; i <= iEnd; i++) {
		iSum += *bFrame;
		bFrame++;
	}
	return iSum;
}

//字符转MAC模式
BOOL atoMac(BYTE *aMac, BYTE *bMac) {
	int i = 0;
	for (i = 0; i < 4; i++) {
		bMac[i] = (BYTE) ((atohex(aMac[i * 2]) << 4)
				+ (atohex(aMac[i * 2 + 1]) & 0x0F));
		//fprintf(stderr,"%02X",bMac[i]);
	}
	//fprintf(stderr,"==bMac\n");
	bMac[5] = '\0';
	return TRUE;
}

void printHEX(BYTE* comm, int len) {
	int i;
	//fprintf(stderr,"COMMAND::");
	for (i = 0; i < len; i++) {
		fprintf(stderr, "%02X ", comm[i]);
	}
	fprintf(stderr, "\n");
}

//字符转16进制数字
int atohex(BYTE a) {
	if (a < 0x3a) {
		//0~9
		return (int) (a - 48);
	} else if (a < 0x47) {
		//A~F
		return (int) (a - 55);
	} else {
		return (int) (a - 87);
	}
}
