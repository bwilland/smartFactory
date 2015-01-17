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

void startProgramServer();
void* program();

void executeSchedule(PROGRAM *p_program, SHAREMEM *shr_mem, int isStopSchedule);

void executeScheduleImmediately(PROGRAM *p_schedule, SHAREMEM *shr_mem,
		time_t timeStamp_start) ;
void executeTask(SHAREMEM* share_mem, PROGRAM *p_program);
void closeSchedule(PROGRAM *p_program);
int analysisResponse(BYTE* p_Response, int resp_size, NODE* node,BYTE* resultCMP);
