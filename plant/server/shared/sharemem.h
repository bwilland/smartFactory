#ifndef DATASTRUCT
  #define DATASTRUCT
	#include "datastruct.h"
#endif
#ifndef H_SYMBOL
  #define H_SYMBOL
	#include "symbol.h"
#endif
#ifndef DBADAPTER
  #define DBADAPTER
	#include "dbadapter.h"
#endif
#ifndef SCHEDULEH
  #define SCHEDULEH
	#include "../thread/scheduleThread.h"
#endif
#include <netinet/in.h>

int createShareMem();
void fill_shm_Areas(SHAREMEM* shr_mem);
void fill_shm_Schedule(SHAREMEM* shr_mem);
SHAREMEM* get_shm_Memory(BOOL bLocked);
void release_shm_Memory(SHAREMEM* share_mem,BOOL bUnlocked);
int checkLoginStaues(char* clientIp,int* iRole);
int Sem_V();
int Sem_P();
int CreateSem();
void release_shm_User(SHAREUSER* share_mem) ;
SHAREUSER* get_shm_User() ;

