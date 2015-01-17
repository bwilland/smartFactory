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
int genLEDControlFrame(COMMANDINFO* commandInfo, BYTE* bFrame) {
	//bzero(bFrame, 128);

	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x1A;
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E;
	bFrame[i++] = 0x00;
	bFrame[i++] = 0x02;
	bFrame[i++] = 0x00;
	bFrame[i++] = 0x14;

//	bFrame[i++] = 0xFF;
//	bFrame[i++] = 0xFF;
	//groupIDs
	int bGroup = 0;
	//bGroup = bGroup << (sCommand->groupID - 1);

	char p_Paras[256];
	char* token, *p;
	strcpy(p_Paras, commandInfo->groupIDs);
	p = p_Paras;

	while (p != NULL ) {
		token = strsep(&p, ",");
		if (token != NULL ) {
			int groupID = atoi(token);
			bGroup += (1 << (groupID - 1));
		} else {
			break;
		}
	}

	if (commandInfo->isBroadCast == 1) {
		bFrame[i++] = 0xFF;
		bFrame[i++] = 0xFF;
	} else {
		bFrame[i++] = (bGroup & 0xFF00) >> 8; // GroupMask
		bFrame[i++] = bGroup & 0x00FF;
	}

	bFrame[i++] = 0x00;
	bFrame[i++] = 0x00;

	int j = 0;
	float fPPF = (float) commandInfo->PPF1;
	BYTE* pchar = (BYTE*) &fPPF;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	fPPF = (float) commandInfo->PPF2;
	pchar = (BYTE*) &fPPF;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	fPPF = (float) commandInfo->PPF3;
	pchar = (BYTE*) &fPPF;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	fPPF = (float) commandInfo->PPF4;
	pchar = (BYTE*) &fPPF;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	bFrame[i++] = 0x7D;

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * make command before section: write data.
 * @return length
 */
int genHeader(COMMANDINFO* commandInfo, BYTE* bFrame) {

	//fprintf(stderr, "commandInfo->pulse1==%f\n", commandInfo->pulse);
	bzero(bFrame, 128);
	// 0C 10 01 0F 32 32 32 00 00 00 00 32 00 03 46
	// MAC 8
	//fprintf(stderr, "commandInfo->pulse2==%f\n", commandInfo->pulse);
	// Header
	bFrame[0] = 0x2A;

	// Length
	// 35 + uart data length
	// not including header, footer, check and length
	bFrame[1] = 0x23;

	// Frame control
	bFrame[2] = 0x41;
	bFrame[3] = 0x88;

	// Reserved 6b
	bFrame[4] = 0x00;
	bFrame[5] = 0x00;
	bFrame[6] = 0x00;
	bFrame[7] = 0x00;
	bFrame[8] = 0x00;
	bFrame[9] = 0x00;

	// FIXME Source Address(MAC)
	// CO mac address
	bFrame[10] = 0x00;
	bFrame[11] = 0x00;
	bFrame[12] = 0x00;
	bFrame[13] = 0x00;

	// Reserved 4b
	bFrame[14] = 0x00;
	bFrame[15] = 0x00;
	bFrame[16] = 0x00;
	bFrame[17] = 0x00;

	// Target Address(MAC)
	// Device address or FF FF FF FF for broadcast
	if (commandInfo->isBroadCast) {
		bFrame[18] = 0xFF;
		bFrame[19] = 0xFF;
		bFrame[20] = 0xFF;
		bFrame[21] = 0xFF;
	} else {
		BYTE bMac[5];
		atoMac((BYTE*) commandInfo->mac, bMac);
		int i = 0;
		for (i = 0; i < 4; i++) {
			bFrame[i + 18] = bMac[3 - i];
		}
	}

	// Reserved 6b
	bFrame[22] = 0x00;
	bFrame[23] = 0x00;
	bFrame[24] = 0x00;
	bFrame[25] = 0x00;
	bFrame[26] = 0x00;
	bFrame[27] = 0x00;

	// Cluster ID
	// Type=0x38 COO (0x18 for Router), ID=0x25 Write, 0x20 Read
	bFrame[28] = 0x25;
	bFrame[29] = 0x00;

	// Reserved 2b
	bFrame[30] = 0x00;
	bFrame[31] = 0x00;

	// Index
	// Uart port mappping address see section 3.7.1
	bFrame[32] = 0xA0;
	bFrame[33] = 0x0F;

	// Subindex
	bFrame[34] = 0x00;

	// Opt
	bFrame[35] = 0x00;

	// Data Length
	// Uart packet Length must be less than packet length - packet overhead
	bFrame[36] = 0xFF;

	return 37;
}

/**
 * make command after section: write data.
 * @return length
 */
int genFooter(COMMANDINFO* commandInfo, BYTE* bFrame, int i) {
	// Check
	// the lowest byte of sum Payload (not including Header, Footer, Check and Length)
	int iSum = sum(bFrame, 3, i);
	bFrame[i++] = (iSum & 0x00FF);

	// Footer
	bFrame[i++] = 0x23;

	//Totol length
	bFrame[1] = (BYTE) (i - 4);

	//-----------for test -----------------
	time_t timep;
	time(&timep);
	fprintf(stderr,"\n%s\n", ctime(&timep));
    //--------------------------------------


	printHEX(bFrame, i);

	// Return total length
	return i;
}

/**
 * 6.1. GetInfo Command
 * @return length of command
 */
int genGetNodeInfo(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x05; //TODO
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x01; // Read
	bFrame[i++] = 0x00; // Address: 0000
	bFrame[i++] = 0x00;

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 6.2. Reset Command
 * @return length of command
 */
int genReset(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x10; //TODO
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x00; // Write
	bFrame[i++] = 0x00; // Address: 00FF
	bFrame[i++] = 0xFF;
	bFrame[i++] = 0x04; // Length
	bFrame[i++] = 0x55; // Command: reset board
	bFrame[i++] = 0x00; // Reserved
	bFrame[i++] = 0x00;
	bFrame[i++] = 0x00;

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 7.2. SetLEDCurrent Command
 * return length of command
 */
int genSetLEDCurrent(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x16; //TODO
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x00; // Write
	bFrame[i++] = 0x02; // Address: 0210
	bFrame[i++] = 0x10;
	bFrame[i++] = 0x10; // Length
	int j = 0;
	float fCurrent = (float) commandInfo->PPF1;
	BYTE* pchar = (BYTE*) &fCurrent;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	fCurrent = (float) commandInfo->PPF2;
	pchar = (BYTE*) &fCurrent;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	fCurrent = (float) commandInfo->PPF3;
	pchar = (BYTE*) &fCurrent;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	fCurrent = (float) commandInfo->PPF4;
	pchar = (BYTE*) &fCurrent;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 7.3. GetLEDError Command
 * @return length of command
 */
int genGetLEDError(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x10; //TODO
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x01; // Read
	bFrame[i++] = 0x02; // Address: 0201
	bFrame[i++] = 0x01;

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 7.4 GetLEDInfo Command
 * @return length of command
 */
int genGetLEDInfo(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x05; //TODO
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x01; // Read
	bFrame[i++] = 0x02; // Address: 0202
	bFrame[i++] = 0x02;

//	bFrame[i++] = 0x34; //length
//	bFrame[i++] = 0x00;
//	bFrame[i++] = 0x00;
//
//	int j = 0;
//	for (j = 0; j < 48; j++) {
//		bFrame[i++] = 0x00;
//	}

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 7.5. SetLEDGroupMask
 * @return length of command
 */
int genSetLEDGroupsMask(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x0A; // Data length
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x00; // Write
	bFrame[i++] = 0x02; // Address: 029X
	bFrame[i++] = 0x40;
	bFrame[i++] = 0x04; // Length

	//groupIDs
	int bGroup = 0;
	//bGroup = bGroup << (sCommand->groupID - 1);

	char p_Paras[256];
	char* token, *p;
	strcpy(p_Paras, commandInfo->groupIDs);
	p = p_Paras;

	while (p != NULL ) {
		token = strsep(&p, ",");
		if (token != NULL ) {
			int groupID = atoi(token);
			bGroup += (1 << (groupID - 1));
		} else {
			break;
		}
	}

	bFrame[i++] = (bGroup & 0xFF00) >> 8; // GroupMask
	bFrame[i++] = bGroup & 0x00FF;

	bFrame[i++] = 0x00; // RES
	bFrame[i++] = 0x00;

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 7.6. LEDManualControl Command
 * @return length of command
 */
int genLEDManualControl(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x10; //TODO
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x00; // Write
	bFrame[i++] = 0x02; // Address: 029X
	bFrame[i++] = 0x90;
	bFrame[i++] = 0x08; // Length
	bFrame[i++] = 0x00; // LED Select
	bFrame[i++] = 0x00;
	bFrame[i++] = 0xFF; // LED PWM%
	bFrame[i++] = 0xFF;
	bFrame[i++] = 0x00; // LED Current(mA)
	bFrame[i++] = 0x00;
	bFrame[i++] = 0x48;
	bFrame[i++] = 0x43;

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 7.7. SetLEDCalibrate Command
 * @return length of command
 */
int genSetLEDCalibrate(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x4D; //TODO
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x00; // Write
	bFrame[i++] = 0x02; // Address: 02aX
	bFrame[i++] = (0xa0 | commandInfo->channel);

	bFrame[i++] = 0x48; // Length
	bFrame[i++] = 0x00; // Reserved
	bFrame[i++] = 0x00;

	bFrame[i++] = (commandInfo->pwm & 0xFF00) >> 8; // LED Calibration PWM%
	bFrame[i++] = (commandInfo->pwm & 0x00FF);

	// Measured PPF Array
	int j = 0;
	float value = (float) commandInfo->current;
	BYTE* pchar = (BYTE*) &value;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}
	value = (float) commandInfo->PPF1;
	BYTE ppf[4];
	pchar = (BYTE*) &value;
	for (j = 0; j < sizeof(float); j++) {
		ppf[j] = *pchar;
		pchar++;
	}

	char p_Paras[256];
	char* token, *p;
	strcpy(p_Paras, commandInfo->groupIDs);
	p = p_Paras;

	BYTE ledCaliberate[64];
	bzero(ledCaliberate, 64);

	while (p != NULL ) {
		token = strsep(&p, ",");
		if (token != NULL ) {
			int ledID = atoi(token);
			//fprintf(stderr, "ledID==%d\n", ledID);

			memcpy(&(ledCaliberate[ledID * 4]), ppf, 4);

		} else {
			break;
		}
	}

	memcpy(&(bFrame[i]), ledCaliberate, 64);
	i += 64;

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 8.1. MotorSet Command
 */
int genMotoSetControl(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x12;
	int j = 0;
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x00; // Write
	bFrame[i++] = 0x01; // Address: 0X01
	bFrame[i++] = 0x00;
	bFrame[i++] = 0x0C; // Length
	bFrame[i++] = (BYTE) commandInfo->mode; // Mode
	bFrame[i++] = (BYTE) commandInfo->direction;
	bFrame[i++] = 0x00; // res
	bFrame[i++] = 0x00;

	float value = (float) commandInfo->distance;
	BYTE* pchar = (BYTE*) &value;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}
	value = (float) commandInfo->speed;
	pchar = (BYTE*) &value;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 8.2. MotorStatus Command
 */
int genMotorStatus(COMMANDINFO* commandInfo, BYTE* bFrame) {
	genHeader(commandInfo, bFrame);
	bFrame[36] = 0x05;
	//int j = 0;
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x01; // Read
	bFrame[i++] = 0x01; // Address: 0X01
	bFrame[i++] = 0x01;

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}

/**
 * 8.3. SetMotorParameters  Command
 */
int genSetMotorParameters(COMMANDINFO* commandInfo, BYTE* bFrame) {
	//fprintf(stderr, "commandInfo->pulse==%f\n", commandInfo->pulse);
	genHeader(commandInfo, bFrame);
	//fprintf(stderr, "commandInfo->pulse==%f\n", commandInfo->pulse);
	bFrame[36] = 0x12;
	int j = 0;
	// Write Data
	int i = 37;
	bFrame[i++] = 0x7E; // Uart Header
	bFrame[i++] = 0x00; // Write
	bFrame[i++] = 0x01; // Address: 0180
	bFrame[i++] = 0x80;
	bFrame[i++] = 0x0C; //Length
	//fprintf(stderr, "commandInfo->pulse==%f\n", commandInfo->pulse);
	float value = (float) commandInfo->pulse;

	//fprintf(stderr, "value==%f,,commandInfo->pulse==%f\n", value,commandInfo->pulse);

	BYTE* pchar = (BYTE*) &value;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}
	value = commandInfo->maxSpeed;
	pchar = (BYTE*) &value;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}
	value = commandInfo->maxLength;
	pchar = (BYTE*) &value;
	for (j = 0; j < sizeof(float); j++) {
		bFrame[i++] = *pchar;
		pchar++;
	}

	bFrame[i++] = 0x7D; //Uart Footer

	// Return total length
	return genFooter(commandInfo, bFrame, i);
}
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
