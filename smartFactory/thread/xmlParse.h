/*
 * xmlParse.h
 *
 *  Created on: Oct 27, 2014
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
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>

int parseXMLType(xmlDocPtr pdoc);
int parseCommadXML(xmlDocPtr pdoc, SERVERCOMMAND* cmdInfo) ;
int parseRegistionXML(xmlDocPtr pdoc, REGISTIONINFO* regInfo);
int parseSynchronizationXML(xmlDocPtr pdoc, int *frameType);
int parseSettingXML(xmlDocPtr pdoc, GroupSettingCommand* settingInfo) ;
int parseProgramXML(xmlDocPtr pdoc, PROGRAM* program);
