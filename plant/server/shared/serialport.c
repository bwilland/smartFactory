#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include "serialport.h"
#include "lightingpack.h"
#include "sharemem.h"

#define BAUD_RATE	115200
#define DATA_BITS	8
#define STOP_BITS	1
#define PARITY		'N'

#define LOOPCOUNT	20

int serialPort_fd;

//const char* SERIALPORT_USB = "/dev/ttyUSB0";
const char* SERIALPORT = "/dev/ttyUSB0";
int nRead=0, nWrite =0;

/********************************************************************************************
 * name:        sendData
 * func:          发送串口数据
 * para:          指令，指令长度，指令数量
 * ret:         --
 * modify:
 * comment:
 *********************************************************************************************/
int sendData(BYTE* commandBuffer, int size, int cCount) {
	int retval = 0, i = 0;
	fd_set wfds;
	struct timeval tv;
	tv.tv_sec = 2; //2s超时
	tv.tv_usec = 0;
	FD_ZERO(&wfds); // 清空串口接收端口集
	FD_SET(serialPort_fd, &wfds); // 设置串口接收端口集

	retval = select(serialPort_fd + 1, NULL, &wfds, NULL, &tv); // 检测串口是否可写
	if (retval > 0) {
		if (FD_ISSET(serialPort_fd,&wfds) > 0) {

			tcflush(serialPort_fd, TCIOFLUSH);

			BYTE *p_Command = commandBuffer;
			for (i = 0; i < cCount; i++) {
				//printHEX(p_Command, size);
				nWrite = write(serialPort_fd, p_Command, size);
				p_Command += size;
				usleep(800000); //间隔250ms
			}
			//fprintf(stderr,"%d bytes wrote OK!!! \n", writeCount);
		}
	} else {
		fprintf(stderr, "Time Over \n");
		return 0;
	}
	return 1;
}

/********************************************************************************************
 * name:        serialData
 * func:          发送/接受串口数据
 * para:          指令，指令长度，指令数量，接受buffer
 * ret:         反馈的数据位数
 * modify:
 * comment:
 *********************************************************************************************/
int serialData(BYTE* commandBuffer, int size, int cCount, BYTE* readBuffer) {
	int retval = 0, i = 0;
	fd_set wfds, rfds;
	struct timeval tv;
	tv.tv_sec = 2; //2s超时
	tv.tv_usec = 0;
	nRead = 0;
	FD_ZERO(&wfds); // 清空串口接收端口集
	FD_SET(serialPort_fd, &wfds); // 设置串口接收端口集

	retval = select(serialPort_fd + 1, NULL, &wfds, NULL, &tv); // 检测串口是否可写
	if (retval > 0) {
		if (FD_ISSET(serialPort_fd,&wfds) > 0) {

			tcflush(serialPort_fd, TCIOFLUSH);
			BYTE *p_Command = commandBuffer;
			for (i = 0; i < cCount; i++) {
				//printHEX(p_Command, size);
				nWrite =write(serialPort_fd, p_Command, size);
				p_Command += size;
				usleep(750000); //间隔750ms
			}
			fprintf(stderr,"%d bytes wrote OK!!! \n", size*cCount);

			//读数据
			FD_ZERO(&rfds);// 清空串口接收端口集
			FD_SET(serialPort_fd, &rfds); // 设置串口接收端口集
			retval = select(serialPort_fd + 1, &rfds, NULL, NULL, &tv); // 检测串口是否可读
			if (retval) {
				if (FD_ISSET(serialPort_fd,&rfds) > 0) {
					memset(readBuffer, 0, BUFFERSIZE);
					//usleep(200000);
					if ((nRead = read(serialPort_fd, readBuffer, BUFFERSIZE))
							> 0) {
						fprintf(stderr,"%d bytes read OK \n", nRead);
						//读到数据
						return nRead;
					} else {
						return 0;
					}

				}
			} else {
				fprintf(stderr, "Time Over \n");
				return 0;
			}
		}
	} else {
		fprintf(stderr, "Time Over \n");
		return 0;
	}

	return nRead;
}

int serialWriter(BYTE* commandBuffer, int size, BYTE* readBuffer, BOOL needWait) {
	int writeCount = 0;

	tcflush(serialPort_fd, TCIOFLUSH);
printHEX(commandBuffer, size);
	writeCount = write(serialPort_fd, commandBuffer, size);
	fprintf(stderr, "%d bytes wrote OK!! \n wait some seconds.... \n",
			writeCount);
	if (needWait) {
		sleep(2); //等待
	} else {
		usleep(1000000);
	}

	memset(readBuffer, 0, BUFFERSIZE);
	if ((nRead = read(serialPort_fd, readBuffer, BUFFERSIZE)) > 0) {
		//读到数据
		return nRead;
	} else {
		return 0;
	}

	return nRead;
}

void setParam(int serialPort_fd) {
	struct termios stOptions;

	bzero(&stOptions, sizeof(stOptions));
	tcgetattr(serialPort_fd, &stOptions);                           // 得到当前串口的参数
	//cfsetispeed(&stOptions, B115200);                          // 将输入波特率设为115200
	//cfsetospeed(&stOptions, B115200);                          // 将输出波特率设为115200

	cfsetispeed(&stOptions, B57600);                          // 将输入波特率设为115200
	cfsetospeed(&stOptions, B57600);                          // 将输出波特率设为115200

	stOptions.c_cflag |= (CLOCAL | CREAD);                   // 使能接收并使能本地状态
	stOptions.c_cflag &= ~PARENB;                         // 无校验 8位数据位1位停止位
	stOptions.c_cflag &= ~CSTOPB;
	stOptions.c_cflag &= ~CSIZE;
	stOptions.c_cflag |= CS8;
	stOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN | ECHOK
			| ECHOCTL | ECHOKE); // 原始数据输入
	stOptions.c_oflag &= ~(OPOST | ONLCR | OCRNL);
	stOptions.c_iflag |= (IGNBRK);
	stOptions.c_iflag &= ~(ICRNL | IXON | IXOFF | IXANY);

	stOptions.c_cc[VTIME] = 0;                          // 设置等待时间和最小接收字符数
	stOptions.c_cc[VMIN] = 0;

//tcflush(serialPort_fd, TCIFLUSH);
	tcflush(serialPort_fd, TCIOFLUSH);                          // 处理未接收的字符
	tcsetattr(serialPort_fd, TCSANOW, &stOptions);                      // 激活新配置
}

/**************主程序***********************************************************************************************************/
int startSerialServer() {

	fprintf(stderr, "open serial %s \n", SERIALPORT);
//打开、设置串口
	serialPort_fd = openSerial(SERIALPORT);
	if (serialPort_fd > 0) {
		setParam(serialPort_fd);
		usleep(50000);

		//初始化zigbee网络
		/*		BYTE command[]="AT+MTO";
		 BYTE readBuffer[BUFFERSIZE];
		 sendData(command,6,readBuffer);*/

	} else {
		/*serialPort_fd = openSerial(SERIALPORT_USB);
		 //fprintf(stderr,"open serial failed!\n");
		 //exit(FALSE);
		 if (serialPort_fd > 0) {
		 setParam(serialPort_fd);
		 usleep(50000);
		 } else {*/
		fprintf(stderr, "open serial failed!\n");
		//exit(FALSE);
		//}
	}
	return serialPort_fd;
}

/****************串口函数****************************************************************************************/
/**
 *@breif 打开串口
 */
int openSerial(const char *Dev) {

	int fd = open(Dev, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK); //| O_NOCTTY | O_NDELAY
	if (-1 == fd) {
		perror("Can't Open Serial Port\n"); //将上一个函数发生错误的原因输出到标准设备(stderr),加到自定义字符串后面。

		return -1;
	} else {
		fprintf(stderr, "Open Serial Port OK: %d \n", fd);
		return fd;
	}

}
