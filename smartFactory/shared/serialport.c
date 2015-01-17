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

#include <sys/sem.h>
#include <sys/ipc.h>

//#define BAUD_RATE	115200
#define BAUD_RATE	57600
#define DATA_BITS	8
#define STOP_BITS	1
#define PARITY		'N'

#define LOOPCOUNT	20

int serialPort_fd;

int serial_sem_id = 0;

//const char* SERIALPORT_USB = "/dev/ttyUSB0";
const char* SERIALPORT = "/dev/ttyUSB0";
//const char* SERIALPORT = "/dev/ttySP1";
int nRead = 0, nWrite = 0;

int sendSingleData(BYTE* commandBuffer, int size) {
	int retval = 0;
	fd_set wfds;
	struct timeval tv;
	tv.tv_sec = 1; //5s超时
	tv.tv_usec = 0;
	//nRead = 0;
	FD_ZERO(&wfds);
	// 清空串口接收端口集
	FD_SET(serialPort_fd, &wfds);
	// 设置串口接收端口集
	//fprintf(stderr, "serialPort_fd==%d",serialPort_fd);

	retval = select(serialPort_fd + 1, NULL, &wfds, NULL, &tv); // 检测串口是否可写
	if (retval > 0) {
		if (FD_ISSET(serialPort_fd,&wfds) > 0) {

			tcflush(serialPort_fd, TCIOFLUSH);
			BYTE *p_Command = commandBuffer;

			nWrite = write(serialPort_fd, p_Command, size);
			fprintf(stderr, "%d bytes wrote OK!! \n wait some seconds.... \n",
					nWrite);

		}
	} else {
		fprintf(stderr, "Time Over \n");
		return 0;
	}

	return nWrite;
}

int serialSingleData(BYTE* commandBuffer, int size, BYTE* readBuffer) {
	Serial_Sem_P();
	int retval = 0;
	fd_set wfds, rfds;
	struct timeval tv;
	tv.tv_sec = 1; //5s超时
	tv.tv_usec = 0;
	nRead = 0;
	FD_ZERO(&wfds);
	// 清空串口接收端口集
	FD_SET(serialPort_fd, &wfds);
	// 设置串口接收端口集
	//fprintf(stderr, "serialPort_fd==%d",serialPort_fd);

	retval = select(serialPort_fd + 1, NULL, &wfds, NULL, &tv); // 检测串口是否可写
	if (retval > 0) {
		if (FD_ISSET(serialPort_fd,&wfds) > 0) {

			tcflush(serialPort_fd, TCIOFLUSH);
			BYTE *p_Command = commandBuffer;

			nWrite = write(serialPort_fd, p_Command, size);
			fprintf(stderr, "%d bytes wrote OK!! \n wait some seconds.... \n",
					nWrite);
			//读数据
			FD_ZERO(&rfds);
			// 清空串口接收端口集
			FD_SET(serialPort_fd, &rfds);
			// 设置串口接收端口集
			retval = select(serialPort_fd + 1, &rfds, NULL, NULL, &tv); // 检测串口是否可读
			if (retval) {
				if (FD_ISSET(serialPort_fd,&rfds) > 0) {
					memset(readBuffer, 0, BUFFERSIZE);
					if ((nRead = read(serialPort_fd, readBuffer, BUFFERSIZE))
							> 0) {
						//读到数据
						Serial_Sem_V();
						return nRead;
					} else {
						Serial_Sem_V();
						return 0;
					}

				}
			} else {
				fprintf(stderr, "Time Over \n");
				Serial_Sem_V();
				return 0;
			}
		}
	} else {
		Serial_Sem_V();
		fprintf(stderr, "Time Over \n");
		return 0;
	}
	Serial_Sem_V();
	return nRead;
}

int readSerialData(BYTE* readBuffer) {

	Serial_Sem_P();
	int retval = 0;
	fd_set rfds;
	struct timeval tv;
	tv.tv_sec = 2; //2s超时
	tv.tv_usec = 0;
	nRead = 0;

	//读数据
	FD_ZERO(&rfds);
	// 清空串口接收端口集
	FD_SET(serialPort_fd, &rfds);
	// 设置串口接收端口集
	retval = select(serialPort_fd + 1, &rfds, NULL, NULL, &tv); // 检测串口是否可读
	if (retval) {
		if (FD_ISSET(serialPort_fd,&rfds) > 0) {
			memset(readBuffer, 0, BUFFERSIZE);
			if ((nRead = read(serialPort_fd, readBuffer, BUFFERSIZE)) > 0) {
				//读到数据
				Serial_Sem_V();
				return nRead;
			} else {
				Serial_Sem_V();
				return 0;
			}

		}
	} else {
		fprintf(stderr, "Time Over \n");
		Serial_Sem_V();
		return 0;
	}
	Serial_Sem_V();
	return 0;
}

int serialWriter(BYTE* commandBuffer, int size, BYTE* readBuffer, BOOL needWait) {
	Serial_Sem_P();
	int writeCount = 0;
	fprintf(stderr, "serialPort_fd===%d\n", serialPort_fd);
	tcflush(serialPort_fd, TCIOFLUSH);
//printHEX(commandBuffer, size);
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
		Serial_Sem_V();
		return nRead;
	} else {
		Serial_Sem_V();
		return 0;
	}
	Serial_Sem_V();
	return nRead;
}

void setParam(int serialPort_fd) {
	struct termios stOptions;

	bzero(&stOptions, sizeof(stOptions));
	tcgetattr(serialPort_fd, &stOptions);                           // 得到当前串口的参数
	cfsetispeed(&stOptions, B57600);                          // 将输入波特率设为115200
	cfsetospeed(&stOptions, B57600);                          // 将输出波特率设为115200
//cfsetispeed(&stOptions, B115200);                          // 将输入波特率设为115200
//cfsetospeed(&stOptions, B115200);
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

	// create lock signal
	serial_sem_id = CreateSerialSem();

	fprintf(stderr, "open serial %s \n", SERIALPORT);
//打开、设置串口
	serialPort_fd = openSerial(SERIALPORT);
	if (serialPort_fd > 0) {
		setParam(serialPort_fd);
		usleep(50000);
	} else {
		fprintf(stderr, "open serial failed!\n");
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


int CreateSerialSem()    // 建立信号量, 键值key 及信号量的初始值value
{
	union semun sem;
	sem.val = 1;

	serial_sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666); //获得信号量ID
	if (-1 == serial_sem_id) {
		printf("create semaphore error\n");
		return -1;
	}

	semctl(serial_sem_id, 0, SETVAL, sem); // 发送命令，建立value个初始值的信号量

	return serial_sem_id;
}

int Serial_Sem_V() // 增加信号量
{
	struct sembuf sops = { 0, +1, SEM_UNDO }; // 建立信号量结构值
//fprintf(stderr,"--unlock---\n");
	return (semop(serial_sem_id, &sops, 1));    // 发送命令
}

int Serial_Sem_P()    //减少信号量值
{
	struct sembuf sops = { 0, -1, SEM_UNDO };    // 建立信号量结构值
//fprintf(stderr,"--lock---\n");
	return (semop(serial_sem_id, &sops, 1));
}
