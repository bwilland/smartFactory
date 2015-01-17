/*
 * smart.c
 *
 *  Created on: 2014-10-26
 *      Author: root
 */
#include "./shared/serialport.h"
#include "./thread/scheduleThread.h"
#include "./shared/sharemem.h"
#include "./shared/socket.h"
#include "./thread/cmdTransThread.h"
#include "./thread/heartBeatThread.h"
#include "./shared/lightingpack.h"
#include "./actions/autosearch.h"
#include "./thread/alarmThread.h"
#include <unistd.h>

int main(int argc, char **argv) {

	startSerialServer();
	createShareMem();


	startSocketServer();
	startTransServer();

	sleep(2);
	startProgramServer();
	startHeartBeatServer();

	sleep(1);
	startAlarmServer();

//*************************************************

//	char resp[1024];
//
//	//autoSearch(resp,0);
//
//	SERVERCOMMAND cmd;
//	strcpy(cmd.MAC, "002F5ABB");
//	cmd.groupID = 1;
//	cmd.dimmer = 800;
//	cmd.onOff = 1;
//
//	BYTE command[64];
//	int iCount = genSensorQueryFrame(&cmd, command);
//	//int iCount = genDeviceControlFrame(&cmd,command);
//
//	//int iCount = genGroupSetFrame(&cmd,command);
//	genSingleGroupControlFrame(&cmd,command);
//	BYTE test[BUFFERSIZE];
//	//serialData(command, strlen((const char *) command), 1,test);
//	iCount = serialSingleData((BYTE*) command, iCount, test);
//	printf("iCount==%d \n", iCount);

	//genGroupControlFrame(&cmd,command);


	return 0;
}
