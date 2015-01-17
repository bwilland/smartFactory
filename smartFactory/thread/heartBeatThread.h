/*
 * cmdTransThread.h
 *
 *  Created on: 2014-10-26
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
#include "xmlParse.h"


void startHeartBeatServer();
void* heartBeat();
void checkConnetion();
void checkDimmer();
void sendGroupDimmer();
void ctrlDimmer(GROUPAUTODIMMER* p_groupDimmer,SERVERCOMMAND *cmd);
void energeCost();
