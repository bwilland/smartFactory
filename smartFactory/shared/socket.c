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
#include "socket.h"
#include "lightingpack.h"
#include "sharemem.h"
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define LOOPCOUNT	20

int socket_fd;

#define DEST_IP "192.168.1.107"
//#define DEST_IP "192.168.0.105"
//#define DEST_IP "192.168.2.64"

//#define DEST_IP "115.238.86.185"
#define DEST_PORT 9090

int nSocketRead = 0, nSocketWrite = 0;

/********************************************************************************************
 * name:        sendData
 * func:          发送socket数据
 * para:          指令，指令长度，指令数量
 * ret:         --
 * modify:
 * comment:
 *********************************************************************************************/
int sendSocketData(BYTE* commandBuffer, int size, int cCount) {
	int retval = 0;
	fd_set wfds;
	struct timeval tv;
	tv.tv_sec = 5; //2s超时
	tv.tv_usec = 0;
	FD_ZERO(&wfds);
	// 清空socket接收端口集
	FD_SET(socket_fd, &wfds);
	// 设置socket接收端口集
//printf("socket_fd==%d\n",socket_fd);
	retval = select(socket_fd + 1, NULL, &wfds, NULL, &tv); // 检测socket是否可写

	if (retval > 0) {

		if (FD_ISSET(socket_fd,&wfds) > 0) {

			tcflush(socket_fd, TCIOFLUSH);

			BYTE *p_Command = commandBuffer;
			//for (i = 0; i < cCount; i++) {
				//printHEX(p_Command, size);
				nSocketWrite = write(socket_fd, p_Command, size);
				//p_Command += size;
				//usleep(800000); //间隔250ms
		//	}
			fprintf(stderr,"%d bytes wrote OK!!! \n", nSocketWrite);
		}
	} else {
		fprintf(stderr, "Time Over \n");
		return 0;
	}
	return 1;
}

/********************************************************************************************
 * name:        readData
 * func:          读取socket数据（阻塞式读取）
 * para:          指令，指令长度，指令数量
 * ret:         --
 * modify:
 * comment:
 *********************************************************************************************/
int readSocketData(BYTE* readBuffer, int bufferSize) {
	int retval = 0;
	fd_set rfds;

	FD_ZERO(&rfds);
	// 清空socket接收端口集
	FD_SET(socket_fd, &rfds);
	// 设置socket接收端口集

	retval = select(socket_fd + 1, &rfds, NULL, NULL, NULL ); // 检测socket是否可写
	if (retval > 0) {
		if (FD_ISSET(socket_fd,&rfds) > 0) {

			memset(readBuffer, 0, bufferSize);
			//usleep(200000);
			if ((nSocketRead = read(socket_fd, readBuffer, BUFFERSIZE)) > 0) {
				//fprintf(stderr,"%d bytes read OK \n", nSocketRead);
				//读到数据
				readBuffer[nSocketRead] ='\0'; //做成字符串
				return nSocketRead;
			} else {
				return 0;
			}

		}
	}
	return 0;
}
/********************************************************************************************
 * name:        socketData
 * func:          发送/接受socket数据
 * para:          指令，指令长度，指令数量，接受buffer
 * ret:         反馈的数据位数
 * modify:
 * comment:
 *********************************************************************************************/
int socketData(BYTE* commandBuffer, int size, int cCount, BYTE* readBuffer) {
	int retval = 0, i = 0;
	fd_set wfds, rfds;
	struct timeval tv;
	tv.tv_sec = 2; //2s超时
	tv.tv_usec = 0;
	nSocketRead = 0;
	FD_ZERO(&wfds);
	// 清空socket接收端口集
	FD_SET(socket_fd, &wfds);
	// 设置socket接收端口集

	retval = select(socket_fd + 1, NULL, &wfds, NULL, &tv); // 检测socket是否可写
	if (retval > 0) {
		if (FD_ISSET(socket_fd,&wfds) > 0) {

			tcflush(socket_fd, TCIOFLUSH);
			BYTE *p_Command = commandBuffer;
			for (i = 0; i < cCount; i++) {
				//printHEX(p_Command, size);
				nSocketWrite = write(socket_fd, p_Command, size);
				p_Command += size;
				usleep(250000); //间隔250ms
			}
			//fprintf(stderr,"%d bytes wrote OK!!! \n", writeCount);

			//读数据
			FD_ZERO(&rfds);
			// 清空socket接收端口集
			FD_SET(socket_fd, &rfds);
			// 设置socket接收端口集
			retval = select(socket_fd + 1, &rfds, NULL, NULL, &tv); // 检测socket是否可读
			if (retval) {
				if (FD_ISSET(socket_fd,&rfds) > 0) {
					memset(readBuffer, 0, BUFFERSIZE);
					//usleep(200000);
					if ((nSocketRead = read(socket_fd, readBuffer, BUFFERSIZE)) > 0) {
						//fprintf(stderr,"%d bytes read OK \n", nSocketRead);
						//读到数据
						return nSocketRead;
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

	return nSocketRead;
}


int socketWriterOnly(BYTE* commandBuffer, int size) {
	int writeCount = 0;
	//fprintf(stderr, "bytes wrote OK!! \n ");
	tcflush(socket_fd, TCIOFLUSH);
	//fprintf(stderr, " wait some seconds.... \n");
//printHEX(commandBuffer, size);
	writeCount = write(socket_fd, commandBuffer, size);
	fprintf(stderr, "%d bytes wrote OK!! \n wait some seconds.... \n",
			writeCount);

	return writeCount;
}

int socketWriter(BYTE* commandBuffer, int size, BYTE* readBuffer, BOOL needWait) {
	int writeCount = 0;

	tcflush(socket_fd, TCIOFLUSH);
//printHEX(commandBuffer, size);
	writeCount = write(socket_fd, commandBuffer, size);
	fprintf(stderr, "%d bytes wrote OK!! \n wait some seconds.... \n",
			writeCount);
	if (needWait) {
		sleep(2); //等待
	} else {
		usleep(1000000);
	}

	memset(readBuffer, 0, BUFFERSIZE);
	if ((nSocketRead = read(socket_fd, readBuffer, BUFFERSIZE)) > 0) {
		//读到数据
		return nSocketRead;
	} else {
		return 0;
	}

	return nSocketRead;
}

/**************主程序***********************************************************************************************************/
int startSocketServer() {

	fprintf(stderr, "open socket %s \n", DEST_IP);
//打开、设置socket
	socket_fd = openSocket();
	if (socket_fd > 0) {

	} else {

		fprintf(stderr, "open socket failed!\n");

	}
	return socket_fd;
}

/****************socket函数****************************************************************************************/
/**
 *@breif 打开socket
 */
int openSocket() {

	struct sockaddr_in my_addr;
	struct sockaddr_in dest_addr; /* will hold the destination addr */

	//
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket Error, %d\n", errno);
		exit(1);
	}
	int on = 1;
	 setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	//socket
	my_addr.sin_family = AF_INET; /* host byte order */
	my_addr.sin_port = htons(DEST_PORT); /* see man htons for more information */
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY ); /* get our address */
	bzero(&(my_addr.sin_zero), 8); /* zero out the rest of the space */

	if (bind(socket_fd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))
			== -1) {
		printf("Bind Error, %d\n", errno);
		close(socket_fd);
		//exit(1);
	}

	//
	dest_addr.sin_family = AF_INET; /* host byte order */
	dest_addr.sin_port = htons(DEST_PORT); /* short, network byte order */
	dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
	bzero(&(dest_addr.sin_zero), 8); /* zero the rest of the struct */

	if (connect(socket_fd, (struct sockaddr *) &dest_addr,
			sizeof(struct sockaddr)) == -1) {
		printf("Connection Error, %d\n", errno);
		close(socket_fd);
		//exit(1);
	}

	return socket_fd;

}

int closeSocket(){
	shutdown(socket_fd,SHUT_RDWR);
	 close(socket_fd);
	 return 0;
}
