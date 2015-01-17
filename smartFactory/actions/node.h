#ifndef H_SYMBOL
  #define H_SYMBOL
	#include "../shared/symbol.h"
#endif
#ifndef DATASTRUCT
  #define DATASTRUCT
	#include "../shared/datastruct.h"
#endif
#ifndef DBADAPTER
  #define DBADAPTER
	#include "../shared/dbadapter.h"
#endif
#include <libxml/parser.h>
#include <libxml/tree.h>

void actionAllNodeList(xmlDocPtr pdoc);
void controlCMD(xmlDocPtr pdoc,SERVERCOMMAND *cmdInfo);

void addErrorMsg(xmlDocPtr pdoc,int messageID);
