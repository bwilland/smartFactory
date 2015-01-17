/*
 * schedule.h
 *
 *  Created on: Aug 20, 2013
 *      Author: root
 */

#ifndef H_SYMBOL
#define H_SYMBOL
#include "../shared/symbol.h"
#endif
#ifndef DATASTRUCT
#define DATASTRUCT
#include "../shared/datastruct.h"
#endif

void startShceduleServer();
void* schedule();
void checkExecResult(char *areaIds,SHAREMEM *share_mem);
void executeSchedule(SCHEDULE *p_schedule, SHAREMEM *shr_mem,
		time_t timeStamp_start,int *modelID);
void executeScheduleImmediately(SCHEDULE *p_schedule, SHAREMEM *shr_mem,
		time_t timeStamp_start) ;
void executeTask(SHAREMEM* share_mem, SCHEDULE *p_schedule, TASK *pTask,
		time_t t_start);
void closeSchedule(SCHEDULE *p_schedule, SHAREMEM *shr_mem,
		time_t timeStamp_start);
int analysisResponse(BYTE* p_Response, int resp_size, NODE* node,BYTE* resultCMP);
