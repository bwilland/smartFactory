#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h> //convert function
#include "actiondispatch.h"

#include "../actions/autosearch.h"
#include "../actions/node.h"
#include "../actions/area.h"
#include "../actions/home.h"
#include "../actions/model.h"
#include "../actions/schedule.h"
#include "../actions/control.h"
#include "../actions/systemSeting.h"
#include "../actions/reproduction.h"

void actionDispatch(char *p_requestPara, char* responseMsg) {
	char* actionName = strsep(&p_requestPara, ".");
	fprintf(stderr, "actionName== %s \n", actionName);

	//printf("p_requestPara==%s\n",p_requestPara);

	char requestPara[1024];
	convert(p_requestPara, strlen(p_requestPara), requestPara, 1024);

	if (strcmp(actionName, "/ControlManage") == 0) {

		controlManage(requestPara, responseMsg);
		return;
	}

	if (strcmp(actionName, "/NodeManage") == 0) {
		nodeManage(requestPara, responseMsg);
		return;
	}
	if (strcmp(actionName, "/AreaManage") == 0) {

		areaManage(requestPara, responseMsg);
		return;
	}
	if (strcmp(actionName, "/ModelManage") == 0) {

		modelManage(requestPara, responseMsg);
		return;
	}

	if (strcmp(actionName, "/ScheduleManage") == 0) {

		scheduleManage(requestPara, responseMsg);
		return;
	}

	if (strcmp(actionName, "/HomeManage") == 0) {

		homeManage(requestPara, responseMsg);
		return;
	}
	if (strcmp(actionName, "/SystemManage") == 0) {

		systemManage(requestPara, responseMsg);
		return;
	}
	if (strcmp(actionName, "/ReproductionManage") == 0) {

		reproductionManage(requestPara, responseMsg);
		return;
	}

}

void convert(char *from, int from_len, char *str_to, int to_len) {

	char *to = str_to;
	while (*from != '\0') {
		//printf("*from = %c\n",*from);
		char fromStr[100];
		//char toStr[100];
		char *fp;
		fp = fromStr;
		if (*from != '%') {
			memcpy(to, from, 1);
			to++;
			from++;
		} else {
			memset(fp, 0, 100);
			while (*from == '%') {
				char g_tmp[5];
				char* p_tmp = g_tmp;
				strcpy(p_tmp, "0x");
				p_tmp += 2;
				from++;
				memcpy(p_tmp, from, 2);
				p_tmp += 2;
				*p_tmp = '\0';

//	printf("g_tmp==%s\n",g_tmp);
//	printf("g_tmp==%d\n",StrToInt(g_tmp));	 	 	 
				char tmp = StrToInt(g_tmp);
				memcpy(fp, &tmp, 1);
				fp++;
				from += 2;
			}
			*fp = '\0';
			//	fp = toStr;
			//	memset(toStr,0,100);
			//conv(fromStr,strlen(fromStr),toStr,100);
//	printf("fromStr==%s \n",fromStr);
			memcpy(to, fromStr, strlen(fromStr));
			to += strlen(fromStr);
		}

	}
	*to = '\0';
//	printf("to==%s \n",str_to);
}

/*
 void conv(char *from,int from_len,char *to,int to_len)
 {
 // printf("%s is to be converted!\n",from);
 iconv_t cd,cdd;
 cd=iconv_open("GB2312","UTF-8");
 char **from2=&from;
 char **to2=&to;
 if(iconv(cd,from2,&from_len,to2,&to_len)==-1)
 printf("Convert fail!\n");
 else
 printf("Convert success!\n");
 iconv_close(cd);
 return ;
 }
 */

int StrToInt(char * str) {
	int value = 0;
	int sign = 1;
	int radix;

	if (*str == '-') {
		sign = -1;
		str++;
	}
	if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
		radix = 16;
		str += 2;
	} else if (*str == '0')      // �˽������ַ�Ϊ0
			{
		radix = 8;
		str++;
	} else
		radix = 10;
	while (*str) {
		if (radix == 16) {
			if (*str >= '0' && *str <= '9')
				value = value * radix + *str - '0';
			else
				value = value * radix + (*str | 0x20) - 'a' + 10;
			// value = value * radix + *str - 'a' + 10; // Ҳû���Ⱑ
		} else
			value = value * radix + *str - '0';
		str++;
	}
	return sign * value;
}
