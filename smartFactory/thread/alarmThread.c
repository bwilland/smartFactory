/*
 * alarmThread.c
 *
 *  Created on: Jan 10, 2015
 *      Author: root
 */
#include "alarmThread.h"
#include "../shared/lightingpack.h"
#include <unistd.h>
#include "../shared/serialport.h"

void startAlarmServer() {

	//pthread_create(&t_Schedule, NULL, schedule, NULL); //计划任务线程
	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		sleep(3);
		alarmServer();

	}

	fprintf(stderr,
			"=============start alarmServer server Ready ===============\n");

}

void* alarmServer() {
	BYTE readBuff[BUFFERSIZE];
	//SHAREMEM* shr_mem;
	int readCount = 0;
	while (TRUE) {
		fprintf(stderr, "Read From Serial..\n");
		readCount = readSerialData(readBuff);
		if (readCount > 0) {
			printHEX(readBuff, readCount);

		}

		sleep(2);
	}
	return NULL ;
}
