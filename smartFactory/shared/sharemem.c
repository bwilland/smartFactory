#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/sem.h>  
#include <sys/ipc.h>  

#include "sharemem.h"
#include "../thread/scheduleThread.h"
#include "../thread/cmdTransThread.h"

int shm_id = 0, sem_id = 0, usr_shm_id = 0;
const int MEM_HEAD_LEN = 1024;
const int MEM_GROUP_LEN = 15360; //15K
const int MEM_PROGRAM_LEN = 10240; //10k
//const int MEM_USER_LEN = 4096;

int createShareMem() {

	SHAREMEM* share_mem;

	shm_id = shmget(IPC_PRIVATE, 30720, IPC_CREAT | IPC_EXCL | 0600);
	if (shm_id == -1) {
		perror("shmget error");
		return -1;
	}
	fprintf(stderr, "shm_id ==%d \n", shm_id);

	char* p_mem = (char*) shmat(shm_id, NULL, 0); //加载内存
	bzero(p_mem, 30720);

	share_mem = (SHAREMEM*) p_mem;

	fprintf(stderr,"share_mem==%p \n",share_mem);

	share_mem->p_groupDimmer = (GROUPAUTODIMMER*) (p_mem + MEM_HEAD_LEN);
	share_mem->p_Program = (PROGRAM*) (p_mem + MEM_HEAD_LEN + MEM_GROUP_LEN);

	if ((int) share_mem == -1) {
		perror("shmat error");
		return -1;
	}
	strcpy(share_mem->uuid,"0123456789.0123456");


	fill_shm_Groups(share_mem);
	fill_shm_Program(share_mem);

	shmdt(share_mem); //脱离共享
	fprintf(stderr, "==========createShareMEM OK==========\n");

//信号量
	sem_id = CreateSem();

	return 0;
}

/*初始化共享内存中的区域*/
void fill_shm_Groups(SHAREMEM* shr_mem) {

	fprintf(stderr, "Fill share memory by groups...\n");
	GROUPAUTODIMMER* pGroup = shr_mem->p_groupDimmer;
	int groupCount = getResultCount("GROUPS", NULL );
	fprintf(stderr, "group count == %d  \n", groupCount);
	if (groupCount > 0) {
		GROUPAUTODIMMER groups[groupCount];
		bzero(groups, sizeof(GROUPAUTODIMMER) * groupCount);
		getAllGroup(groups);

		memcpy(pGroup, groups, sizeof(GROUPAUTODIMMER) * groupCount);

	}
	shr_mem->groupPlaceholder = groupCount;

	fprintf(stderr, "Fill share memory by groups  END.\n");

}

/*初始化共享内存中的program*/
void fill_shm_Program(SHAREMEM* shr_mem) {

	fprintf(stderr, "Fill share memory by Program...\n");
	PROGRAM *p_program = shr_mem->p_Program;

	int programCount = getResultCount("PROGRAM", "STATUS IN(0,1,2,5)");
	fprintf(stderr, "PROGRAM count == %d  \n", programCount);
	if (programCount > 0) {
		PROGRAM programInfo[programCount];
		getProgramList(programInfo, "0,1,2,5");
		int i = 0;
		time_t timeNow;
		time(&timeNow);
		for (i = 0; i < programCount; i++) {
			setProgramTime(&programInfo[i], timeNow);
		}

		memcpy(p_program, programInfo, sizeof(PROGRAM) * programCount);

	}

	shr_mem->programPlaceholder = programCount;

	fprintf(stderr, "Fill share memory by Program  END.\n");

}

SHAREMEM* get_shm_Memory(BOOL bLocked) {

	if (bLocked) {
		Sem_P();
	}
	char* p_mem = (char*) shmat(shm_id, NULL, 0);
	SHAREMEM* share_mem = (SHAREMEM*) p_mem;
	share_mem->p_groupDimmer = (GROUPAUTODIMMER*) (p_mem + MEM_HEAD_LEN);
	share_mem->p_Program = (PROGRAM*) (p_mem + MEM_HEAD_LEN + MEM_GROUP_LEN);

	return share_mem;

}



void release_shm_Memory(SHAREMEM* share_mem, BOOL bUnlocked) {
	shmdt(share_mem); //脱离共享
	if (bUnlocked) {
		Sem_V();
	}
}

int Sem_V() // 增加信号量
{
	struct sembuf sops = { 0, +1, SEM_UNDO }; // 建立信号量结构值
//fprintf(stderr,"--unlock---\n");
	return (semop(sem_id, &sops, 1));    // 发送命令
}

int Sem_P()    //减少信号量值
{
	struct sembuf sops = { 0, -1, SEM_UNDO };    // 建立信号量结构值
//fprintf(stderr,"--lock---\n");
	return (semop(sem_id, &sops, 1));
}

int CreateSem()    // 建立信号量, 键值key 及信号量的初始值value
{
	union semun sem;
	sem.val = 1;

	sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666); //获得信号量ID
	if (-1 == sem_id) {
		printf("create semaphore error\n");
		return -1;
	}

	semctl(sem_id, 0, SETVAL, sem); // 发送命令，建立value个初始值的信号量

	return sem_id;
}
